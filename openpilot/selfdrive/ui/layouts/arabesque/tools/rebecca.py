"""One frog, one button, bounded scenery; Arabesque owns the window and input."""

from importlib.resources import as_file
from pathlib import Path
import random

import pyray as rl

from openpilot.common.swaglog import cloudlog
from openpilot.selfdrive.ui.layouts.arabesque.theme import DEFAULT_THEME as THEME
from openpilot.system.ui.lib.application import ASSETS_DIR, gui_app, TextAlignment
from openpilot.system.ui.lib.multilang import tr
from openpilot.system.ui.widgets import Widget, device
from openpilot.system.ui.widgets.label import gui_label

_STEP = 1 / 60
_SCORE_FILE = Path(__file__).with_name('rebecca-score.txt')
_INK = rl.Color(118, 98, 139, 255)
_STONE = rl.Color(169, 139, 153, 255)
_CREAM = rl.Color(239, 221, 183, 255)
_CORAL = rl.Color(222, 145, 136, 255)
_FIELD = rl.Color(118, 98, 139, 70)


class Rebecca(Widget):
  def __init__(self):
    super().__init__()
    self._sprite = None
    self._sounds, self._owns_audio = {}, False
    self._best, self._notice = 0, ''
    self._pressed_button = False
    self._rng = random.Random()
    self._reset()
    self._state = 'ready'

  def _reset(self):
    self._state = 'flying'
    self._y, self._vy, self._vx = 208.0, -160.0, 150.0
    self._distance = self._elapsed = self._accumulator = self._cooldown = 0.0
    self._objects = []
    self._next, self._encounter, self._corridor = 0.0, 0, 208.0
    self._record = False

  def show_event(self):
    super().show_event()
    self._sprite = gui_app.texture('arabesque/rebecca.png', 256, 256)
    try:
      self._best = max(0, int(_SCORE_FILE.read_text(encoding='ascii')))
    except (FileNotFoundError, ValueError):
      self._best = 0
    except OSError:
      self._notice = tr('Best score unavailable')
    try:
      self._owns_audio = not rl.is_audio_device_ready()
      if self._owns_audio:
        rl.init_audio_device()
      if rl.is_audio_device_ready():
        for name in ('flap', 'loss', 'best'):
          with as_file(ASSETS_DIR.joinpath('arabesque', name + '.wav')) as path:
            sound = rl.load_sound(str(path))
          if sound.frameCount:
            self._sounds[name] = sound
            rl.set_sound_volume(sound, 0.35)
    except (OSError, RuntimeError) as error:
      cloudlog.warning(f'Rebecca audio unavailable: {error}')

  def hide_event(self):
    self._save_best()
    for sound in self._sounds.values():
      rl.stop_sound(sound)
      rl.unload_sound(sound)
    self._sounds.clear()
    if self._owns_audio and rl.is_audio_device_ready():
      rl.close_audio_device()
    self._owns_audio = False
    self._state, self._accumulator = 'ready', 0.0
    super().hide_event()

  def _play(self, name):
    if name in self._sounds:
      rl.play_sound(self._sounds[name])

  def _save_best(self):
    score = int(self._distance / 10)
    if score > self._best:
      try:
        # shortcut: no power-loss durability; add fsync if records must survive abrupt power cuts.
        temporary = _SCORE_FILE.with_suffix('.tmp')
        temporary.write_text(f'{score}\n', encoding='ascii')
        temporary.replace(_SCORE_FILE)
        self._best, self._record = score, True
      except OSError:
        self._notice = tr('Best score could not be saved')

  def _update_layout_rects(self):
    rect = self._rect
    self._scale = s = min(rect.width / 1080, rect.height / 488)
    self._ox = rect.x + (rect.width - 1080 * s) / 2
    self._oy = rect.y + (rect.height - 488 * s) / 2
    self._button = rl.Rectangle(rect.x, rect.y + rect.height - 72 * s, rect.width, 72 * s)

  def _handle_mouse_press(self, pos):
    self._pressed_button = rl.check_collision_point_rec(pos, self._button)
    if not self._pressed_button or self._cooldown:
      return
    if self._state != 'flying':
      self._reset()
    self._vy = -160.0
    self._play('flap')

  def _spawn(self):
    self._corridor = max(156, min(260, self._corridor + self._rng.uniform(-60, 60)))
    x, y = self._distance + 1140, self._corridor
    kind = self._encounter % 3
    if kind == 0 and len(self._objects) <= 4:
      self._objects.extend((0, x, y + offset, self._rng.uniform(44, 54)) for offset in (-120, 120))
    elif len(self._objects) < 6:
      kind = 2 if kind == 2 and not any(o[0] == 2 for o in self._objects) else 1
      self._objects.append((kind, x, self._rng.choice((76, 340)), 26))
    self._encounter += 1
    self._next = self._distance + 440

  def _update_state(self):
    if not self.enabled or not device.awake:
      self._accumulator = 0.0
      return
    dt = max(0.0, min(rl.get_frame_time(), 4 * _STEP))
    self._cooldown = max(0.0, self._cooldown - dt)
    if self._state != 'flying':
      return
    self._accumulator += dt
    while self._accumulator >= _STEP and self._state == 'flying':
      self._accumulator -= _STEP
      self._step()

  def _step(self):
    ax, ay = (min(200, 150 + self._distance / 1600) - self._vx) * 3, 330.0
    for kind, x, y, _radius in self._objects:
      dx, dy = x - self._distance - 230, y - self._y
      d2 = dx * dx + dy * dy
      if kind == 2 and d2 < 185 ** 2:
        pull = min(4.5, 90000 / (d2 + 6400))
        ax, ay = ax + dx * pull, ay + dy * pull
    self._vx = max(90, min(240, self._vx + ax * _STEP))
    self._vy = max(-240, min(260, self._vy + ay * _STEP))
    self._distance += self._vx * _STEP
    self._y += self._vy * _STEP
    self._elapsed += _STEP
    self._objects = [o for o in self._objects if o[1] - self._distance > -200]
    if self._distance >= self._next:
      self._spawn()
    if not 30 < self._y < 386 or any(
      (x - self._distance - 230) ** 2 + (y - self._y) ** 2 < (radius + 30) ** 2 for _, x, y, radius in self._objects
    ):
      self._state, self._cooldown = 'lost', 0.4
      self._save_best()
      self._play('loss')
      if self._record:
        self._play('best')

  def _label(self, x, y, width, height, text, size=18, color=_CREAM):
    s = self._scale
    gui_label(rl.Rectangle(self._ox + x * s, self._oy + y * s, width * s, height * s),
              text, round(size * s), color)

  def _render(self, rect):
    s, ox, oy = self._scale, self._ox, self._oy
    rl.draw_rectangle_rec(rect, THEME.background)
    for i in range(32):
      x = (i * 137.51 - self._distance * (0.12 + i % 3 * 0.08)) % 1080
      y = 24 + i * 83 % 360
      rl.draw_circle_v(rl.Vector2(ox + x * s, oy + y * s), (0.7 + i % 3 * 0.35) * s, _INK if i % 4 else _CREAM)
    streak = self._elapsed % 11
    if 0 < streak < 0.7:
      x, y = ox + (1000 - streak * 400) * s, oy + (45 + streak * 110) * s
      rl.draw_line_ex(rl.Vector2(x, y), rl.Vector2(x + 36 * s, y - 10 * s), s, _CREAM)
    for kind, x, y, radius in self._objects:
      center = rl.Vector2(ox + (x - self._distance) * s, oy + y * s)
      r = radius * s
      if kind == 0:
        rl.draw_poly(center, 7, r, x % 360, _STONE)
        rl.draw_circle_v(rl.Vector2(center.x - r * 0.25, center.y - r * 0.22), r * 0.23, _INK)
        rl.draw_circle_v(rl.Vector2(center.x + r * 0.32, center.y + r * 0.3), r * 0.12, _INK)
      elif kind == 1:
        rl.draw_line_ex(rl.Vector2(center.x - r, center.y), rl.Vector2(center.x + r, center.y), 2 * s, _CREAM)
        for offset in (-0.75, 0.75):
          rl.draw_rectangle_rec(rl.Rectangle(center.x + offset * r - 8 * s, center.y - 13 * s, 16 * s, 26 * s), _INK)
        rl.draw_poly(center, 4, 12 * s, 0, _CREAM)
      else:
        rl.draw_ring(center, 184 * s, 185 * s, 0, 360, 48, _FIELD)
        rl.draw_ring(center, 32 * s, 34 * s, 0, 360, 32, _CORAL)
        rl.draw_ring(center, 26 * s, 27 * s, 0, 360, 32, _CREAM)
        rl.draw_circle_v(center, r, THEME.background)
    if self._sprite is not None:
      height = 96 * self._sprite.height / self._sprite.width
      rl.draw_texture_pro(self._sprite, rl.Rectangle(0, 0, self._sprite.width, self._sprite.height),
                          rl.Rectangle(ox + 182 * s, oy + (self._y - height / 2) * s, 96 * s, height * s),
                          rl.Vector2(0, 0), 0, rl.WHITE)
    self._label(24, 12, 400, 36, f'{int(self._distance / 10)} m', 24)
    self._label(810, 12, 246, 36, tr('Best') + f'  {max(self._best, int(self._distance / 10))} m', color=_INK)
    if self._state != 'flying':
      self._label(440, 142, 600, 64, tr('Rebecca, Adrift') if self._state == 'ready' else tr('Still unbothered.'), 36)
      caption = tr('Tap to Float. Dodge everything.') if self._state == 'ready' else tr('New best!') if self._record else tr('Space is rude. Try again.')
      self._label(444, 208, 580, 48, caption, color=THEME.secondary_text)
    if self._notice:
      self._label(24, 368, 1032, 40, self._notice, 16, _CORAL)
    rl.draw_rectangle_rec(self._button, THEME.pressed if self._pressed_button and self.is_pressed and self.enabled else THEME.control)
    label = {'ready': tr('Start floating'), 'flying': tr('Float'), 'lost': tr('Again')}[self._state]
    gui_label(self._button, label, round(24 * s), THEME.background, alignment=TextAlignment.CENTER)
