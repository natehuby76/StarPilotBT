"""Native widget routing, actual schema compatibility and subscribed button edges."""

from types import SimpleNamespace as NS
from unittest.mock import Mock

import pyray as rl
import pytest

from openpilot.cereal import messaging
from openpilot.starpilot.ui.spacepilot import native
from openpilot.starpilot.ui.spacepilot.state import Phase
from openpilot.starpilot.ui.tests.test_spacepilot_state import mature_gate, parked_ui, read
from openpilot.system.ui.lib.application import MouseEvent, MousePos, gui_app


def test_real_cereal_schema_matches_the_physical_witness():
  ui = parked_ui()
  for name in ('carState', 'selfdriveState', 'carControl', 'deviceState'):
    data = ui.sm[name]
    def plain(value):
      return {k: plain(v) for k, v in vars(value).items()} if isinstance(value, NS) else value
    event = messaging.new_message(name, valid=True)
    setattr(event, name, plain(data))
    ui.sm[name] = getattr(event.as_reader(), name)
  event = messaging.new_message('pandaStates', 1, valid=True)
  event.pandaStates[0] = vars(ui.sm['pandaStates'][0])
  ui.sm['pandaStates'] = event.as_reader().pandaStates
  assert read(ui) is not None


@pytest.fixture
def page(monkeypatch):
  monkeypatch.setattr(native, 'live_witness', lambda: read(parked_ui()))
  monkeypatch.setattr(native.time, 'monotonic', lambda: 10.)
  monkeypatch.setattr(native.time, 'monotonic_ns', lambda: 10_000_000_000)
  wheel = Mock()
  wheel.poll.return_value = False
  monkeypatch.setattr(native, 'WheelReader', lambda _: wheel)
  widget = native.SpacePilotPage(mature_gate(), Mock())
  widget.set_rect(rl.Rectangle(0, 0, 536, 240))
  monkeypatch.setattr(gui_app, 'get_active_widget', lambda: widget)
  monkeypatch.setattr(gui_app, 'pop_widget', Mock())
  monkeypatch.setattr(gui_app, '_mouse_events', [])
  widget.show_event()
  yield widget
  widget.hide_event()


def test_native_touch_starts_hold_does_not_repeat_exit_and_cleanup(page):
  page._handle_mouse_press(MousePos(160, 120))
  assert page.flight.phase == Phase.FLIGHT
  page.flight.vy = 10
  page._handle_mouse_press(MousePos(160, 120))
  assert page.flight.vy == 10
  page._handle_mouse_event(MouseEvent(MousePos(160, 120), 0, False, True, False, 10.1))
  page._handle_mouse_press(MousePos(500, 20))
  wheel = page.wheel
  page._handle_mouse_event(MouseEvent(MousePos(500, 20), 0, False, True, False, 10.2))
  assert page.closed and page.wheel is None
  wheel.close.assert_called_once()
  gui_app.pop_widget.assert_called_once()
  assert page._tick not in gui_app._nav_stack_ticks
  assert not page.flight.gates


@pytest.mark.parametrize('reason', ['stale', 'hidden', 'backlog', 'no_socket'])
def test_safety_exit_reveals_main_and_never_renders_or_continues_input(page, monkeypatch, reason):
  wheel = page.wheel
  if reason == 'stale':
    monkeypatch.setattr(native, 'live_witness', lambda: None)
  elif reason == 'hidden':
    monkeypatch.setattr(gui_app, 'get_active_widget', lambda: object())
  elif reason == 'backlog':
    wheel.poll.side_effect = ValueError('backlog')
  else:
    page.wheel = None
  page._tick()
  page.on_unsafe.assert_called_once()
  assert page.closed and page.wheel is None
  page.renderer = Mock()
  page._render(page.rect)
  page.renderer.render.assert_not_called()
  page._tick()
  page.on_unsafe.assert_called_once()


def test_wheel_only_observes_admitted_game_and_drives_same_action(page):
  page.wheel.poll.return_value = True
  page._tick()
  assert page.flight.phase == Phase.FLIGHT
  page.wheel.poll.assert_called_once_with(10_000_000_000)


def test_optional_render_failure_returns_to_native_alerts(page, monkeypatch):
  page.renderer = Mock()
  page.renderer.render.side_effect = RuntimeError('GL fixture failure')
  monkeypatch.setattr(native.cloudlog, 'exception', Mock())
  page._render(page.rect)
  assert page.closed and page.wheel is None
  page.on_unsafe.assert_called_once()


@pytest.mark.parametrize('vector', [True, False])
def test_gradient_supports_comma_and_desktop_native_signatures(monkeypatch, vector):
  renderer = native.SpaceRenderer()
  renderer.vector_gradient = vector
  draw = Mock()
  monkeypatch.setattr(rl, 'draw_circle_gradient', draw)
  renderer.gradient(12, 13, 20, rl.WHITE, rl.BLANK)
  args = draw.call_args.args
  assert len(args) == (4 if vector else 5)
  assert args[-3:] == (20, rl.WHITE, rl.BLANK)


@pytest.mark.parametrize('interruption', ['long_press', 'drag', 'scroll_cancel', 'other_tile', 'hide', 'unsafe'])
def test_version_sequence_resets_on_interruption(monkeypatch, interruption):
  monkeypatch.setattr(gui_app, 'texture', lambda *_a, **_k: NS(width=64, height=64))
  monkeypatch.setattr(gui_app, 'font', lambda *_a: rl.Font())
  monkeypatch.setattr(native, 'ui_state', NS(params=NS(get=lambda _: '0.10.3')))
  monkeypatch.setattr(native, 'live_witness', lambda: read(parked_ui()))
  pushed = Mock()
  monkeypatch.setattr(gui_app, 'push_widget', pushed)
  version = native.VersionButton(Mock())
  version.set_rect(rl.Rectangle(0, 0, 402, 180))
  version._sub_label.set_rect(rl.Rectangle(40, 110, 322, 46))
  version.gate = mature_gate()
  for i in range(4):
    version.taps.tap(10 + i * .2)
  if interruption == 'hide':
    version.hide_event()
  elif interruption == 'unsafe':
    monkeypatch.setattr(native, 'live_witness', lambda: None)
    version._update_state()
  elif interruption == 'scroll_cancel':
    version.set_touch_valid_callback(lambda: False)
    monkeypatch.setattr(gui_app, '_mouse_events', [])
    version._process_mouse_events()
  else:
    start = MousePos(100, 130)
    finish = MousePos(130, 130) if interruption == 'drag' else start
    if interruption == 'other_tile':
      start = finish = MousePos(500, 130)
    monkeypatch.setattr(gui_app, '_mouse_events', [MouseEvent(start, 0, True, False, True, 10.7),
                       MouseEvent(finish, 0, False, True, False, 11.3 if interruption == 'long_press' else 10.75)])
    version._process_mouse_events()
  assert version.taps.count == 0
  pushed.assert_not_called()


def car_event(stamp, *, pressed=None, kind='accelCruise'):
  event = messaging.new_message('carState', valid=True)
  event.logMonoTime = stamp
  event.carState.canValid = True
  event.carState.standstill = True
  event.carState.gearShifter = 'park'
  if pressed is not None:
    event.carState.buttonEvents = [{'type': kind, 'pressed': pressed}]
  return event.as_reader()


def test_nonconflated_reader_retains_short_press_release_and_closes(monkeypatch):
  socket = Mock()
  subscribe = Mock(return_value=socket)
  monkeypatch.setattr(messaging, 'sub_sock', subscribe)
  reader = native.WheelReader(1_000_000_000)
  subscribe.assert_called_once_with('carState', conflate=False)
  messages = iter([car_event(1_010_000_000, pressed=False), car_event(1_020_000_000, pressed=True),
                   car_event(1_030_000_000, pressed=False), None])
  monkeypatch.setattr(messaging, 'recv_one_or_none', lambda _: next(messages))
  assert reader.poll(1_040_000_000)
  assert 'accelCruise' in reader.input.armed
  reader.close()
  assert reader.socket is None and not reader.input.armed


@pytest.mark.parametrize('fault', ['stale', 'future', 'invalid', 'drive', 'moving', 'backlog'])
def test_reader_rejects_untrustworthy_queued_input(monkeypatch, fault):
  monkeypatch.setattr(messaging, 'sub_sock', Mock())
  reader = native.WheelReader(1_000_000_000)
  message = car_event(1_100_000_000).as_builder()
  if fault == 'future':
    message.logMonoTime = 2_000_000_000
  elif fault == 'invalid':
    message.valid = False
  elif fault == 'drive':
    message.carState.gearShifter = 'drive'
  elif fault == 'moving':
    message.carState.vEgo = 2.
  messages = iter([message.as_reader(), None])
  monkeypatch.setattr(messaging, 'recv_one_or_none', lambda _: message.as_reader() if fault == 'backlog' else next(messages))
  with pytest.raises(ValueError):
    reader.poll(1_500_000_000 if fault == 'stale' else 1_110_000_000)
  reader.close()


def test_five_tap_native_value_region_movement_and_page_reset(monkeypatch):
  monkeypatch.setattr(gui_app, 'texture', lambda *_a, **_k: NS(width=64, height=64))
  monkeypatch.setattr(gui_app, 'font', lambda *_a: rl.Font())
  monkeypatch.setattr(native, 'ui_state', NS(params=NS(get=lambda _: '0.10.3')))
  monkeypatch.setattr(native, 'live_witness', lambda: read(parked_ui()))
  pushed = Mock()
  monkeypatch.setattr(gui_app, 'push_widget', pushed)
  version = native.VersionButton(Mock())
  version.set_rect(rl.Rectangle(0, 0, 402, 180))
  version._sub_label.set_rect(rl.Rectangle(40, 110, 322, 46))
  version.gate = mature_gate()
  for i in range(5):
    now = 10. + i * .2
    monkeypatch.setattr(native.time, 'monotonic', lambda now=now: now)
    pos = MousePos(100, 130)
    monkeypatch.setattr(gui_app, '_mouse_events', [MouseEvent(pos, 0, True, False, True, now),
                                               MouseEvent(pos, 0, False, True, False, now + .04)])
    version._process_mouse_events()
  pushed.assert_called_once()
  assert isinstance(pushed.call_args.args[0], native.SpacePilotPage)
  pushed.reset_mock()
  for i in range(5):
    now = 12. + i * .2
    monkeypatch.setattr(gui_app, '_mouse_events', [MouseEvent(MousePos(100, 130), 0, True, False, True, now),
                    MouseEvent(MousePos(130, 130), 0, False, False, True, now + .02),
                    MouseEvent(MousePos(100, 130), 0, False, True, False, now + .04)])
    version._process_mouse_events()
  pushed.assert_not_called()
  version.taps.tap(20.)
  version.hide_event()
  assert version.taps.count == 0 and not version.gate.ready


def test_return_to_road_is_instant_even_when_still_standing(monkeypatch):
  from openpilot.starpilot.ui import runtime_app
  owner = object.__new__(runtime_app.StarMiciMainLayout)
  owner._scroller = Mock()
  owner._car_onroad_layout = Mock(rect=rl.Rectangle(1072, 0, 536, 240))
  owner._home_layout = Mock(rect=rl.Rectangle(536, 0, 536, 240))
  pop = Mock()
  monkeypatch.setattr(gui_app, 'pop_widgets_to', pop)
  monkeypatch.setattr(runtime_app, 'ui_state', NS(started=True, is_body=False))
  runtime_app.StarMiciMainLayout._exit_spacepilot(owner)
  pop.assert_called_once_with(owner, instant=True)
  owner._scroller.scroll_to.assert_called_once_with(1072, smooth=False)


@pytest.mark.parametrize(('hardware', 'has_owner', 'expected'), [('mici', True, 1), ('tici', True, 0), ('mici', False, 0)])
def test_developer_installs_version_gesture_only_for_c4_with_exit_owner(monkeypatch, tmp_path, hardware, has_owner, expected):
  from openpilot.common.hardware import HARDWARE
  from openpilot.common.params import Params
  from openpilot.selfdrive.ui.mici.layouts.settings import developer
  from openpilot.starpilot.connect import provider
  params = Params(str(tmp_path))
  ui = NS(params=params, is_offroad=lambda: True, is_release=False, add_offroad_transition_callback=Mock())
  monkeypatch.setattr(developer, 'ui_state', ui)
  monkeypatch.setattr(native, 'ui_state', ui)
  monkeypatch.setattr(HARDWARE, 'get_device_type', lambda: hardware)
  monkeypatch.setattr(provider, 'status', lambda: {'selected': 'comma'})
  monkeypatch.setattr(gui_app, 'texture', lambda *_a, **_k: NS(width=64, height=64))
  monkeypatch.setattr(gui_app, 'font', lambda *_a: rl.Font())
  panel = developer.DeveloperLayoutMici(on_spacepilot_exit=Mock() if has_owner else None)
  assert sum(isinstance(item, native.VersionButton) for item in panel._scroller.items) == expected
