import unittest
from types import SimpleNamespace as NS

from openpilot.starpilot.navigation.junction_replay import SECOND, approaches, compare


REPORT = """2026-11-23T03:00:01Z #1 +1.000s Navigation: Mapped junction: stop sign, route vertex 5, 89 m ahead
2026-11-23T03:00:04Z #2 +4.000s Navigation: Mapped junction cleared
2026-11-23T03:00:06Z #3 +6.000s Navigation: Mapped junction: traffic light, route vertex 12, 100 m ahead
2026-11-23T03:00:09Z #4 +9.000s Navigation: Mapped junction cleared"""


def msg(name, seconds, value, valid=True):
  return NS(which=lambda: name, logMonoTime=round(seconds * SECOND), valid=valid, **{name: value})


class JunctionReplayTest(unittest.TestCase):
  def run_sample(self, *, window=0, model=True, planner=False, camera=True, age=.1, clock=True):
    windows = approaches(REPORT)
    sample = NS(status='valid', observedMonoTime=2 * SECOND, validUntilMonoTime=3 * SECOND,
                detections=[NS(confidence=.8)] if camera else [])
    events = [msg('clocks', 2, NS(wallTimeNanos=int(windows[window].start * SECOND)), valid=clock),
              msg('slcVisionObservation', 2, NS(vision=NS(stopSigns=sample, stream='road'))),
              msg('modelV2', 2, NS(action=NS(shouldStop=model))),
              msg('longitudinalPlan', 2 + age, NS(shouldStop=planner))]
    return compare(windows, events)[window]

  def test_would_flag_model_and_plan_disagreement_at_mapped_stop(self):
    self.assertTrue(self.run_sample()['review_cue'])
    self.assertFalse(self.run_sample(planner=True)['review_cue'])

  def test_cross_street_candidate_alone_does_not_trigger(self):
    self.assertFalse(self.run_sample(model=False)['review_cue'])
    self.assertFalse(self.run_sample(camera=False)['review_cue'])

  def test_light_alone_and_stale_samples(self):
    self.assertFalse(self.run_sample(window=1)['review_cue'])
    stale = self.run_sample(age=1.2)
    self.assertEqual((stale['review_cue'], stale['camera_covered_frames'], stale['model_covered_frames']), (False, 0, 0))
    self.assertEqual(self.run_sample(clock=False)['planner_frames'], 0)

  def test_truncation_and_duplicates(self):
    self.assertEqual(approaches(REPORT + '\n' + REPORT), approaches(REPORT))
    self.assertEqual(approaches(REPORT.splitlines()[0]), [])
