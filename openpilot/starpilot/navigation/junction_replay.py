"""Compare Galaxy's mapped junctions with recorded STOP evidence and the stock planner.

From a StarPilot host checkout:
  python -m openpilot.starpilot.navigation.junction_replay galaxy-report.txt rlog.zst

Use a local segment from the same drive. Times are UTC. A review cue means a
mapped sign and a camera candidate coincided; a human must check its approach.
"""

import argparse
from bisect import bisect_right
from dataclasses import dataclass
from datetime import datetime, timezone
import json
from pathlib import Path
import re


EVENT = re.compile(r'^(\S+Z) #\d+ \+\S+s Navigation: Mapped junction(?:: (.+), route vertex (\d+), (\d+) m ahead|( cleared))$')
SECOND = 1_000_000_000


@dataclass(frozen=True)
class Approach:
  start: float
  end: float
  features: str
  vertex: int


def approaches(report):
  events = {}
  for line in report.splitlines():
    match = EVENT.fullmatch(line.strip())
    if match:
      stamp, features, vertex, _, _ = match.groups()
      events[datetime.fromisoformat(stamp.replace('Z', '+00:00')).timestamp()] = (features, vertex)
  ordered = sorted(events.items())
  # A truncated report has no reliable end time for its last label.
  return [Approach(start, end, features, int(vertex))
          for ((start, (features, vertex)), (end, _)) in zip(ordered, ordered[1:])
          if features and end > start]


def compare(windows, messages):
  starts = [w.start for w in windows]
  rows = [dict(utc=datetime.fromtimestamp(w.start, tz=timezone.utc).isoformat(),
               vertex=w.vertex, features=w.features, planner_frames=0, camera_candidate_frames=0,
               camera_covered_frames=0, model_covered_frames=0, model_stop_frames=0,
               planner_stop_frames=0, review_cue=False) for w in windows]
  clock = camera_until = model_until = None
  camera_candidate = model_requests_stop = False
  last_ns = 0
  for msg in messages:
    ns, kind = int(msg.logMonoTime), msg.which()
    if ns < last_ns:
      raise ValueError('Replay requires one time-ordered drive segment')
    last_ns = ns
    if kind == 'clocks':
      wall = int(msg.clocks.wallTimeNanos)
      clock = (ns, wall) if msg.valid and wall > 0 else None
      continue
    if kind == 'slcVisionObservation':
      camera_until = None
      if msg.valid:
        vision = msg.slcVisionObservation.vision
        sample = vision.stopSigns
        if (str(sample.status) == 'valid' and str(vision.stream) in ('road', 'wideRoad') and
            0 < sample.observedMonoTime <= ns < sample.validUntilMonoTime <= sample.observedMonoTime + 1_500_000_000):
          camera_until = sample.validUntilMonoTime
          camera_candidate = any(.7 <= d.confidence <= 1 for d in sample.detections)
    elif kind == 'modelV2':
      model_until = ns + SECOND // 4 if msg.valid else None
      model_requests_stop = bool(msg.valid and msg.modelV2.action.shouldStop)
    elif kind == 'longitudinalPlan' and msg.valid and clock and ns - clock[0] <= 2 * SECOND:
      utc = (clock[1] + ns - clock[0]) / SECOND
      i = bisect_right(starts, utc) - 1
      if i < 0 or utc >= windows[i].end:
        continue
      row = rows[i]
      camera_fresh = camera_until is not None and ns < camera_until
      model_fresh = model_until is not None and ns < model_until
      camera = camera_fresh and camera_candidate
      model_stop = model_fresh and model_requests_stop
      planner_stop = bool(msg.longitudinalPlan.shouldStop)
      row['planner_frames'] += 1
      row['camera_covered_frames'] += int(camera_fresh)
      row['model_covered_frames'] += int(model_fresh)
      row['camera_candidate_frames'] += int(camera)
      row['model_stop_frames'] += int(model_stop)
      row['planner_stop_frames'] += int(planner_stop)
      if 'stop sign' in windows[i].features.split(', ') and camera and model_stop and not planner_stop:
        row['review_cue'] = True
  return rows


def main():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument('report', type=Path)
  parser.add_argument('rlog', type=Path)
  args = parser.parse_args()
  if not args.rlog.is_file():
    parser.error('Supply a local rlog segment')
  from openpilot.tools.lib.logreader import LogReader
  windows = approaches(args.report.read_text(encoding='utf-8-sig'))
  rows = compare(windows, LogReader(str(args.rlog.resolve()), sort_by_time=True))
  print(json.dumps(rows, indent=2))
  if not any(row['planner_frames'] for row in rows):
    parser.exit(1, 'No clock-aligned planner frames inside a completed mapped approach.\n')


if __name__ == '__main__':
  main()
