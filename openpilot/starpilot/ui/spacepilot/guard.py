"""Fail-closed admission using physical vehicle telemetry, never a UI road-mode flag."""

from dataclasses import dataclass
import math
from typing import Any

from openpilot.starpilot.parked_evidence import PANDA_TTL_NS, RESUME_SKEW_NS

FAST_AGE_NS = 250_000_000
SLOW_AGE_NS = 1_000_000_000
DWELL_NS = 1_000_000_000
SERVICES = ('carState', 'selfdriveState', 'carControl', 'deviceState', 'pandaStates')


@dataclass(frozen=True)
class Witness:
  mono_ns: int
  boot_ns: int
  stamps: tuple[int, ...]
  buttons: tuple[tuple[str, bool], ...]


def read_witness(ui: Any, mono_ns: int, boot_ns: int, *, c4: bool) -> Witness | None:
  """Original message and receipt clocks must both be fresh; Panda uses BOOTTIME.

  The supported onroad producers must remain running. Force Offroad stops them,
  so it is neither an admission signal nor a resource-management option here.
  """
  try:
    if (not c4 or mono_ns <= 0 or boot_ns <= 0 or not ui.started or ui.CP is None or ui.CP.notCar or
        ui.projection_read_only or ui.replay_clock is not None or getattr(ui.sm, 'replay_sample', None) is not None):
      return None
    sm = ui.sm
    stamps = []
    for name in SERVICES:
      limit = SLOW_AGE_NS if name == 'deviceState' else PANDA_TTL_NS if name == 'pandaStates' else FAST_AGE_NS
      now = boot_ns if name == 'pandaStates' else mono_ns
      stamp, received = int(sm.logMonoTime[name]), float(sm.recv_time[name])
      if (not sm.valid[name] or not sm.alive[name] or sm.recv_frame[name] <= ui.started_frame or stamp <= 0 or
          not 0 <= now - stamp <= limit or not math.isfinite(received) or
          not 0 <= mono_ns - int(received * 1e9) <= limit):
        return None
      stamps.append(stamp)
    cs, ss, cc, ds, pandas = (sm[name] for name in SERVICES)
    speeds = (cs.vEgo, cs.vEgoRaw, cs.wheelSpeeds.fl, cs.wheelSpeeds.fr, cs.wheelSpeeds.rl, cs.wheelSpeeds.rr)
    if (not cs.canValid or cs.canTimeout or not cs.standstill or str(cs.gearShifter) != 'park' or
        any(not math.isfinite(value) or abs(value) > .01 for value in speeds) or
        cs.gasPressed or cs.cruiseState.enabled or ss.enabled or ss.active or str(ss.state) != 'disabled' or
        cc.enabled or cc.latActive or cc.longActive or not ds.started or
        str(ss.alertSize) != 'none' or str(ss.alertStatus) != 'normal' or str(ss.alertSound) != 'none' or
        not pandas or not any(p.ignitionLine or p.ignitionCan for p in pandas)):
      return None
    for panda in pandas:
      if (str(panda.pandaType) == 'unknown' or str(panda.harnessStatus) == 'notConnected' or panda.controlsAllowed or
          panda.heartbeatLost or panda.safetyRxChecksInvalid or str(panda.faultStatus) != 'none' or len(panda.faults)):
        return None
    return Witness(mono_ns, boot_ns, tuple(stamps), tuple((str(event.type), bool(event.pressed)) for event in cs.buttonEvents))
  except (AttributeError, KeyError, TypeError, ValueError, OverflowError, OSError, RuntimeError):
    return None


class ParkGate:
  def __init__(self):
    self.reset()

  def reset(self) -> None:
    self.since: int | None = None
    self.previous: Witness | None = None
    self.ready = False

  def update(self, witness: Witness | None) -> bool:
    previous = self.previous
    if witness is None:
      self.reset()
      return False
    if previous is not None and (not 0 <= witness.mono_ns - previous.mono_ns <= FAST_AGE_NS or
                                 abs((witness.boot_ns - witness.mono_ns) - (previous.boot_ns - previous.mono_ns)) > RESUME_SKEW_NS or
                                 any(new < old for new, old in zip(witness.stamps, previous.stamps, strict=True))):
      self.reset()
    self.previous = witness
    if self.since is None:
      self.since = witness.mono_ns
    self.ready = witness.mono_ns - self.since >= DWELL_NS
    return self.ready
