"""Independent pedal and smartDSU receivers for the RAV4 hybrid.

The exact parent configuration, parser, source leases and tunes are retained.
A valid pedal source supplements that parent; it cannot replace it.
"""

from opendbc.car import structs
from opendbc.car.toyota.values import CAR, ToyotaFlags

PARENT_WORDS = {0xB049: 0x9049, 0xB249: 0x9249}


def marked(cp):
  return bool(
    cp is not None
    and cp.brand == 'toyota'
    and cp.carFingerprint == CAR.TOYOTA_RAV4H
    and cp.flags & ToyotaFlags.GAS_INTERCEPTOR
    and len(cp.safetyConfigs) == 1
    and cp.safetyConfigs[0].safetyModel == structs.CarParams.SafetyModel.toyota
    and cp.safetyConfigs[0].safetyParam in PARENT_WORDS
  )


def parent_cp(cp):
  if not marked(cp):
    return cp
  parent = cp.as_reader().as_builder() if hasattr(cp, 'as_reader') else cp.as_builder()
  parent.flags &= ~int(ToyotaFlags.GAS_INTERCEPTOR)
  parent.safetyConfigs[0].safetyParam = PARENT_WORDS[cp.safetyConfigs[0].safetyParam]
  return parent


def configured(cp):
  from opendbc.car.toyota import rav4h_filter

  return marked(cp) and not cp.secOcRequired and rav4h_filter.configured(parent_cp(cp))


def hardware_supported(candidate, fingerprint, car_fw):
  from opendbc.car.toyota import rav4h_filter

  if candidate != CAR.TOYOTA_RAV4H or fingerprint.get(0, {}).get(0x201) != 6 or any(0x201 in frames for bus, frames in fingerprint.items() if bus != 0):
    return False
  return rav4h_filter.hardware_supported(candidate, without_pedal(fingerprint), car_fw)


def without_pedal(fingerprint):
  return {bus: {address: size for address, size in frames.items() if address != 0x201} for bus, frames in fingerprint.items()}


def apply(cp):
  from opendbc.car.toyota import rav4h_filter

  if not rav4h_filter.configured(cp) or cp.carFingerprint != CAR.TOYOTA_RAV4H:
    return False
  cp.flags |= int(ToyotaFlags.GAS_INTERCEPTOR)
  cp.safetyConfigs[0].safetyParam |= 0x2000
  return True


def prepare_stock(cp):
  from opendbc.car.toyota import rav4h_filter

  if not configured(cp):
    return False
  parent = parent_cp(cp)
  assert rav4h_filter.prepare_stock(parent)
  cp.flags = parent.flags | int(ToyotaFlags.GAS_INTERCEPTOR)
  cp.alternativeExperience = parent.alternativeExperience
  cp.openpilotLongitudinalControl = False
  cp.safetyConfigs[0].safetyParam = parent.safetyConfigs[0].safetyParam | 0x2000
  cp.autoResumeSng = parent.autoResumeSng
  return True


class ParentPedalParser:
  def __init__(self, parent):
    self.parent = parent

  def __getattr__(self, name):
    return getattr(self.parent, name)

  def update(self, strings, sendcan=False):
    # Only the enrolled primary six-byte pedal belongs to the second parser.
    # Malformed lengths and foreign-bus contradictions still reach the parent.
    batches = [(stamp, [frame for frame in frames if not (frame[0] == 0x201 and len(frame[1]) == 6 and frame[2] == 0)]) for stamp, frames in strings]
    return self.parent.update(batches, sendcan)
