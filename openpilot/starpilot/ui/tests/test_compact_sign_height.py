from dataclasses import replace
from unittest.mock import Mock, patch

import pytest

from openpilot.starpilot.ui.large_speed_geometry import LARGE_SIGN_SCALE
from openpilot.starpilot.ui.onroad_state import speed_limit_bounds, slc_controls, compact_sign_obscured_by_actions
from openpilot.starpilot.ui.presentation import Profile
from openpilot.starpilot.ui.speed_limit_widget import SpeedLimitWidget
from openpilot.starpilot.ui.tests.test_unified_speed_native import fonts, state


@pytest.mark.parametrize('profile', list(Profile))
@pytest.mark.parametrize('offset', [False, True])
def test_transparent_outline_and_touch_bounds_stay_fixed_when_offset_changes(profile, offset):
  current = state(show_slc_offset=offset)
  widget = SpeedLimitWidget(fonts(profile), profile)
  widget.shapes = Mock()
  with patch('openpilot.starpilot.ui.speed_limit_widget.rl.draw_rectangle_rounded') as fill:
    widget.render(current)
  fill.assert_not_called()
  scale = LARGE_SIGN_SCALE if profile == Profile.LARGE else 1
  bounds = speed_limit_bounds(profile, current)
  assert bounds == speed_limit_bounds(profile, replace(current, show_slc_offset=not offset))
  assert bounds is not None
  widget.shapes.outline.assert_called_once()
  assert widget.shapes.outline.call_args.args[:2] == (bounds[0]+12+59*scale, bounds[1]+12+66*scale)
  assert ('+5' in [call.args[0] for call in widget.fonts.draw.call_args_list]) == offset


@pytest.mark.parametrize('y', [20, 50, 108])
def test_compact_confirmation_overlap_uses_the_whole_fixed_sign(y):
  current = state(slc_prompt_open=True)
  current.customization['layouts']['compact']['speed_limit']['y'] = y
  obscured = compact_sign_obscured_by_actions(current)
  assert obscured is (y == 108)
  assert (speed_limit_bounds(Profile.COMPACT, current) is None) is obscured
  assert slc_controls(Profile.COMPACT, current)
