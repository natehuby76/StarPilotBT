"""C4 native page and Developer version gesture. All vehicle access is read-only."""

from collections.abc import Callable
import math
import time

import pyray as rl

import openpilot.cereal.messaging as messaging
from openpilot.common.hardware import HARDWARE
from openpilot.common.swaglog import cloudlog
from openpilot.selfdrive.ui.mici.widgets.button import BigButton
from openpilot.selfdrive.ui.ui_state import ui_state
from openpilot.starpilot.ui.spacepilot.guard import FAST_AGE_NS, ParkGate, Witness, read_witness
from openpilot.starpilot.ui.spacepilot.render import EXIT, SpaceRenderer
from openpilot.starpilot.ui.spacepilot.state import FiveTaps, Flight, WheelInput
from openpilot.system.ui.lib.application import MouseEvent, MousePos, gui_app
from openpilot.system.ui.widgets import Widget


def live_witness() -> Witness | None:
  try:
    before = time.monotonic_ns()
    boot = time.clock_gettime_ns(time.CLOCK_BOOTTIME)
    after = time.monotonic_ns()
    if not 0 <= after - before <= 1_000_000:
      return None
    return read_witness(ui_state, after, boot, c4=HARDWARE.get_device_type() == 'mici')
  except (AttributeError, OSError, ValueError):
    return None


class WheelReader:
  """Separate non-conflated subscription keeps 100 Hz press/release edges at 60 Hz UI.

  It exists only while admitted. It never consumes another subscriber's input or
  publishes any messages. Dropping the sole socket reference closes native IPC.
  """
  def __init__(self, after_ns: int):
    self.socket = messaging.sub_sock('carState', conflate=False)
    self.input = WheelInput(after_ns)

  def poll(self, now_ns: int) -> bool:
    fire = False
    for _ in range(32):
      message = messaging.recv_one_or_none(self.socket)
      if message is None:
        return fire
      stamp = int(message.logMonoTime)
      if stamp <= self.input.stamp:
        continue
      cs = message.carState
      if (not message.valid or not 0 <= now_ns - stamp <= FAST_AGE_NS or not cs.canValid or cs.canTimeout or
          str(cs.gearShifter) != 'park' or not cs.standstill or
          any(not math.isfinite(v) or abs(v) > .01 for v in (cs.vEgo, cs.vEgoRaw))):
        raise ValueError('Wheel observation is no longer trustworthy')
      events = tuple((str(event.type), bool(event.pressed)) for event in cs.buttonEvents)
      fire = self.input.update(stamp, events, stamp / 1e9) or fire
    raise ValueError('Wheel input backlog exceeded the bounded drain')

  def close(self) -> None:
    self.socket = None
    self.input = WheelInput()


class SpacePilotPage(Widget):
  def __init__(self, gate: ParkGate, on_unsafe: Callable[[], None]):
    super().__init__()
    self.gate = gate
    self.on_unsafe = on_unsafe
    self.flight = Flight(time.monotonic_ns())
    self.renderer = SpaceRenderer()
    self.wheel: WheelReader | None = None
    self.last_tick = 0.
    self.closed = True
    self.touch_held = self.exit_pressed = False

  def show_event(self) -> None:
    super().show_event()
    witness = live_witness()
    if not self.gate.update(witness) or witness is None:
      # push_widget invokes show_event before enabling this page; defer stack
      # mutation to its first tick, and never acquire a socket after refusal.
      self.closed = False
    else:
      self.closed = False
      try:
        self.wheel = WheelReader(witness.mono_ns)
      except (OSError, RuntimeError):
        self.gate.reset()
    self.last_tick = time.monotonic()
    self.touch_held = self.exit_pressed = False
    gui_app.add_nav_stack_tick(self._tick)

  def hide_event(self) -> None:
    self.closed = True
    gui_app.remove_nav_stack_tick(self._tick)
    if self.wheel is not None:
      self.wheel.close()
      self.wheel = None
    self.flight.close()
    self.gate.reset()
    self.touch_held = self.exit_pressed = False
    super().hide_event()

  def _safe(self) -> bool:
    return bool(not self.closed and gui_app.get_active_widget() is self and self.gate.update(live_witness()))

  def _tick(self) -> None:
    if self.closed:
      return
    if not self._safe() or self.wheel is None:
      self._exit(unsafe=True)
      return
    now = time.monotonic()
    try:
      if self.wheel.poll(time.monotonic_ns()):
        self.flight.act(now)
    except (OSError, RuntimeError, ValueError, AttributeError):
      self._exit(unsafe=True)
      return
    self.flight.update(now - self.last_tick)
    self.last_tick = now

  def _exit(self, *, unsafe: bool) -> None:
    if self.closed:
      return
    self.hide_event()
    gui_app.mouse_events.clear()
    if unsafe:
      self.on_unsafe()
    elif gui_app.get_active_widget() is self:
      gui_app.pop_widget()

  def _in_exit(self, pos: MousePos) -> bool:
    x, y, w, h = EXIT
    return bool(rl.check_collision_point_rec(pos, rl.Rectangle(self.rect.x + x * self.rect.width / 1072,
                self.rect.y + y * self.rect.height / 480, w * self.rect.width / 1072, h * self.rect.height / 480)))

  def _handle_mouse_press(self, pos: MousePos) -> None:
    if self.touch_held:
      return
    self.touch_held = True
    if not self._safe():
      self._exit(unsafe=True)
      return
    self.exit_pressed = self._in_exit(pos)
    if not self.exit_pressed:
      self.flight.act(time.monotonic())

  def _handle_mouse_event(self, mouse_event: MouseEvent) -> None:
    event = mouse_event
    if event.left_released:
      self.touch_held = False
      if self.exit_pressed and self._in_exit(event.pos):
        self._exit(unsafe=False)
      self.exit_pressed = False

  def _render(self, rect: rl.Rectangle) -> None:
    # The check also protects rendering when another native caller renders us
    # directly instead of through the application's navigation tick.
    if not self._safe():
      self._exit(unsafe=True)
      return
    try:
      self.renderer.render(self.flight, rect, wheel_ready=bool(self.wheel and self.wheel.input.armed))
    except Exception:
      # An optional Easter egg must not take the driving/alert UI down with it.
      cloudlog.exception('SpacePilot render failed')
      self._exit(unsafe=True)


class VersionButton(BigButton):
  """Hidden gesture lives on the version number, using native touch cancellation."""
  def __init__(self, on_unsafe: Callable[[], None]):
    super().__init__('version', ui_state.params.get('Version') or 'N/A')
    self.on_unsafe = on_unsafe
    self.taps = FiveTaps()
    self.gate = ParkGate()
    self.press: MouseEvent | None = None
    self.completed = False

  def show_event(self) -> None:
    self.taps.reset()
    self.gate.reset()
    self.press = None
    self.set_value(ui_state.params.get('Version') or 'N/A')
    super().show_event()

  def hide_event(self) -> None:
    self.taps.reset()
    self.gate.reset()
    self.press = None
    super().hide_event()

  def _update_state(self) -> None:
    super()._update_state()
    if not self.enabled or not self.gate.update(live_witness()):
      self.taps.reset()

  def _process_mouse_events(self) -> None:
    if not self._touch_valid():
      self.taps.reset()
      self.press = None
      self.completed = False
    for event in gui_app.mouse_events:
      if event.slot == 0 and event.left_pressed and not rl.check_collision_point_rec(event.pos, self._sub_label.rect):
        self.taps.reset()
    super()._process_mouse_events()

  def _handle_mouse_event(self, mouse_event: MouseEvent) -> None:
    event = mouse_event
    if event.left_pressed:
      self.press = event if rl.check_collision_point_rec(event.pos, self._sub_label.rect) else None
      self.completed = False
    elif self.press is not None:
      if (not 0 <= event.t - self.press.t <= .45 or
          math.hypot(event.pos.x - self.press.pos.x, event.pos.y - self.press.pos.y) > 12 or not self._touch_valid()):
        self.press = None
        self.taps.reset()
      elif event.left_released:
        self.completed = bool(rl.check_collision_point_rec(event.pos, self._sub_label.rect))
        self.press = None

  def _handle_mouse_release(self, pos: MousePos) -> None:
    if not self.completed or not self.gate.update(live_witness()):
      self.taps.reset()
      return
    self.completed = False
    if self.taps.tap(time.monotonic()):
      gui_app.mouse_events.clear()
      gui_app.push_widget(SpacePilotPage(self.gate, self.on_unsafe))
