"""Compare nominal and corrected motion against current observed road edges."""

from dataclasses import dataclass
import math

import numpy as np


@dataclass(frozen=True)
class BodyDimensions:
  width: float
  length: float

  @property
  def valid(self):
    return (
      type(self.width) is float
      and type(self.length) is float
      and math.isfinite(self.width)
      and math.isfinite(self.length)
      and 1.3 <= self.width <= 2.6
      and 2.5 <= self.length <= 7.0
    )


def interface_dimensions(interface, cp) -> BodyDimensions | None:
  provider = getattr(interface, 'get_lateral_body_dimensions', None)
  if not callable(provider):
    return None
  dimensions = provider(cp)
  if dimensions is None:
    return None
  try:
    body = BodyDimensions(*dimensions)
  except (TypeError, ValueError):
    return None
  return body if body.valid else None


def body_dimensions_required(interface, cp) -> bool:
  provider = getattr(interface, 'requires_lateral_body_dimensions', None)
  return bool(provider(cp)) if callable(provider) else False


def current_delay(sm, now_ns: int) -> float | None:
  from openpilot.cereal.services import SERVICE_LIST
  stamp = int(sm.logMonoTime['lateralDelay'])
  if (not sm.all_checks(['lateralDelay']) or not 0 < stamp <= now_ns or
      now_ns - stamp > int(2.5e9 / SERVICE_LIST['lateralDelay'].frequency)):
    return None
  value = float(sm['lateralDelay'].lateralDelay)
  return value if math.isfinite(value) and 0. <= value <= 1. else None


def _center_motion(curvature: float, measured: float, speed: float, delay: float):
  horizon = min(35.0, max(8.0, speed * 1.5))
  ds = horizon / 16.0
  distances = (np.arange(16, dtype=float) + 0.5) * ds
  # Test the measured-to-command handoff rather than substituting the auxiliary
  # model position path for the commanded motion.
  blend = np.clip((distances / speed - delay) / 0.2, 0.0, 1.0)
  ks = measured + (curvature - measured) * blend
  headings = np.cumsum(ks * ds)
  midpoint = headings - ks * ds * 0.5
  x, y = np.cumsum(np.cos(midpoint) * ds), np.cumsum(np.sin(midpoint) * ds)
  return x, y, headings


def _motion(curvature: float, measured: float, speed: float, delay: float, body: BodyDimensions):
  x, y, headings = _center_motion(curvature, measured, speed, delay)
  # Model coordinates originate below the camera. With no measured mounting
  # offset, full length/width on EACH side conservatively cover any camera
  # position inside the body rectangle. These are not assumed axle coordinates.
  longitudinal = np.array([-body.length, -body.length, body.length, body.length])
  lateral = np.array([-body.width, body.width, -body.width, body.width])
  c, s = np.cos(headings)[:, None], np.sin(headings)[:, None]
  return x[:, None] + c * longitudinal - s * lateral, y[:, None] + s * longitudinal + c * lateral


def limit_tracking(path, base: float, correction: float, measured: float, speed: float, delay: float) -> float:
  """A line-tracking contribution must improve its predicted spacing objective."""
  if not all(math.isfinite(v) for v in (base, correction, measured, speed, delay)) or speed < 5.0 or not 0.0 <= delay <= 1.0:
    return 0.0
  x, y, _ = _center_motion(base, measured, speed, delay)
  selected = (x >= max(2.0, path[0][0])) & (x <= path[0][-1])
  if np.count_nonzero(selected) < 4:
    return 0.0
  original = float(np.mean((y[selected] - np.interp(x[selected], *path)) ** 2))
  for factor in (1.0, 0.75, 0.5, 0.25):
    cx, cy, _ = _center_motion(base + correction * factor, measured, speed, delay)
    if not np.all((cx[selected] >= path[0][0]) & (cx[selected] <= path[0][-1])):
      continue
    candidate = float(np.mean((cy[selected] - np.interp(cx[selected], *path)) ** 2))
    if candidate <= original + 1e-12:
      return correction * factor
  return 0.0


def limit_correction(model, base: float, correction: float, measured: float, speed: float, delay: float, body: BodyDimensions) -> float:
  """Retain the largest tested correction that does not worsen edge intrusion.

  This guards only the added lane contribution. The original model command and
  native curvature, jerk, authority and driver-override limits remain upstream.
  """
  if not body.valid or not all(math.isfinite(v) for v in (base, correction, measured, speed, delay)):
    return 0.0
  if speed < 5.0 or not 0.0 <= delay <= 1.0 or abs(base) > 0.2 or abs(measured) > 0.2:
    return 0.0
  if correction == 0.0:
    return 0.0
  edges = []
  try:
    for index in (0, 1):
      if len(model.roadEdges) <= index or len(model.roadEdgeStds) <= index:
        continue
      std = float(model.roadEdgeStds[index])
      if not math.isfinite(std) or std < 0.0:
        return 0.0
      if std > 0.3:
        continue
      edge = model.roadEdges[index]
      x, y = np.asarray(edge.x, dtype=float), np.asarray(edge.y, dtype=float)
      if x.ndim != 1 or y.ndim != 1 or len(x) < 2 or len(x) != len(y) or not np.isfinite(x).all() or not np.isfinite(y).all() or np.any(np.diff(x) <= 0):
        return 0.0
      edges.append((index, x, y, np.gradient(y, x), std))
  except (AttributeError, IndexError, TypeError, ValueError, OverflowError):
    return 0.0
  if not edges:
    return correction
  original_x, original_y = _motion(base, measured, speed, delay, body)
  for factor in (1.0, 0.75, 0.5, 0.25):
    candidate_x, candidate_y = _motion(base + correction * factor, measured, speed, delay, body)
    accepted = True
    for side, x, y, slope, std in edges:
      # Use the same future body samples in both arms; never extrapolate an edge
      # behind the camera or beyond the observed geometry.
      rows = np.all((original_x >= x[0]) & (original_x <= x[-1]), axis=1)
      if np.count_nonzero(rows) < 4 or not np.all((candidate_x[rows] >= x[0]) & (candidate_x[rows] <= x[-1])):
        accepted = False
        break
      ox, oy = original_x[rows].ravel(), original_y[rows].ravel()
      cx, cy = candidate_x[rows].ravel(), candidate_y[rows].ravel()
      sign = 1.0 if side == 0 else -1.0
      old = float(np.min(sign * (oy - np.interp(ox, x, y)) / np.hypot(1.0, np.interp(ox, x, slope))))
      new = float(np.min(sign * (cy - np.interp(cx, x, y)) / np.hypot(1.0, np.interp(cx, x, slope))))
      margin = 0.1 + std
      if min(new, margin) < min(old, margin) - 1e-6:
        accepted = False
        break
    if accepted:
      return correction * factor
  return 0.0
