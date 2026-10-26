"""Gameplay, local score files and lifecycle checks without a window or audio device."""

from pathlib import Path
from types import SimpleNamespace as NS
from unittest.mock import Mock
import wave

import pyray as rl
import pytest

from openpilot.selfdrive.ui.layouts.arabesque.tools import rebecca
from openpilot.system.ui.lib.application import gui_app, MouseEvent, MousePos


@pytest.fixture
def game(monkeypatch, tmp_path):
  import openpilot.system.ui.widgets as widgets
  monkeypatch.setattr(widgets, 'device', NS(awake=True))
  monkeypatch.setattr(rebecca, 'device', NS(awake=True))
  monkeypatch.setattr(gui_app, '_mouse_events', [])
  monkeypatch.setattr(gui_app, '_show_touches', False)
  monkeypatch.setattr(gui_app, 'texture', Mock(return_value=NS(width=256, height=235)))
  monkeypatch.setattr(rebecca, 'gui_label', Mock())
  score_file = tmp_path / 'rebecca-score.txt'
  score_file.write_text('12\n', encoding='ascii')
  monkeypatch.setattr(rebecca, '_SCORE_FILE', score_file)
  for name in ('init_audio_device', 'close_audio_device', 'load_sound', 'set_sound_volume', 'play_sound', 'stop_sound',
               'unload_sound', 'draw_rectangle_rec', 'draw_circle_v', 'draw_poly', 'draw_ring', 'draw_line_ex', 'draw_texture_pro'):
    monkeypatch.setattr(rl, name, Mock())
  monkeypatch.setattr(rl, 'is_audio_device_ready', lambda: True)
  monkeypatch.setattr(rl, 'get_frame_time', lambda: 1 / 60)
  rl.load_sound.side_effect = lambda path: NS(frameCount=100, name=Path(path).stem)
  view = rebecca.Rebecca()
  view.set_rect(rl.Rectangle(0, 104, 2160, 976))
  view.show_event()
  yield view
  view.hide_event()


def press(game, pos, slot=0):
  gui_app._mouse_events = [MouseEvent(pos, slot, True, False, True, 1.0)]
  game.render()


def test_only_primary_button_press_starts_and_flaps(game):
  assert game._best == 12
  press(game, MousePos(500, 400))
  assert game._state == 'ready'
  pos = MousePos(1000, 1000)
  press(game, pos, slot=1)
  assert game._state == 'ready'
  press(game, pos)
  assert game._state == 'flying' and game._vy == -160
  game._vy = 100
  press(game, pos)
  assert game._vy == -160
  assert rebecca._SCORE_FILE.read_text() == '12\n'


def test_fixed_steps_match_across_frame_rates_and_cap_stalls(game, monkeypatch):
  states = []
  for fps in (20, 60):
    game._reset()
    monkeypatch.setattr(rl, 'get_frame_time', lambda fps=fps: 1 / fps)
    for _ in range(fps // 2):
      game._update_state()
    states.append((game._y, game._vy, game._distance))
  assert states[0] == pytest.approx(states[1])
  monkeypatch.setattr(rl, 'get_frame_time', lambda: 30)
  game._reset()
  game._update_state()
  assert game._elapsed == pytest.approx(4 / 60)


def test_disabled_or_sleeping_simulation_does_not_catch_up(game, monkeypatch):
  game._reset()
  game._accumulator = 0.01
  game.set_enabled(False)
  game._update_state()
  assert game._distance == 0 and game._accumulator == 0
  game.set_enabled(True)
  monkeypatch.setattr(rebecca.device, 'awake', False)
  game._update_state()
  assert game._distance == 0
  monkeypatch.setattr(rebecca.device, 'awake', True)
  game._update_state()
  assert game._distance == pytest.approx(2.5)


@pytest.mark.parametrize('dx,dy', [(0, -100), (0, 100), (-100, 60), (100, 60)])
def test_black_holes_pull_in_both_axes(game, dx, dy):
  game._reset()
  game._vy = 0
  game._next = float('inf')
  game._objects = [(2, 230 + dx, 208 + dy, 26)]
  game._step()
  assert (game._vy - 330 / 60) * dy > 0
  if dx:
    assert (game._vx - 150) * dx > 0


def test_collision_freezes_scene_and_debounces_restart(game):
  game._reset()
  game._distance = 200
  game._objects = [(0, 430, 208, 50)]
  game._step()
  assert game._state == 'lost'
  frozen = game._distance, game._y
  game._update_state()
  press(game, MousePos(1000, 1000))
  assert game._state == 'lost' and (game._distance, game._y) == frozen
  for _ in range(25):
    game._update_state()
  press(game, MousePos(1000, 1000))
  assert game._state == 'flying' and game._distance == 0


def test_scenery_is_bounded_and_old_objects_expire(game):
  for _ in range(100):
    game._spawn()
    assert len(game._objects) <= 6
    assert sum(o[0] == 2 for o in game._objects) <= 1
    assert 156 <= game._corridor <= 260
  game._objects = [(0, -300, 60, 50)]
  game._next = float('inf')
  game._step()
  assert not game._objects


def test_only_new_records_are_saved_including_exit_mid_run(game):
  game._distance = 150
  game.hide_event()
  assert rebecca._SCORE_FILE.read_text() == '15\n'
  modified = rebecca._SCORE_FILE.stat().st_mtime_ns
  game._distance = 100
  game._save_best()
  assert rebecca._SCORE_FILE.stat().st_mtime_ns == modified
  assert game._best == 15
  assert rl.unload_sound.call_count == 3
  rl.close_audio_device.assert_not_called()  # A preexisting device belongs to its original owner.


def test_storage_failure_preserves_previous_record_and_keeps_game_playable(game, monkeypatch):
  monkeypatch.setattr(Path, 'replace', Mock(side_effect=OSError('unavailable')))
  game._distance = 200
  game._save_best()
  assert game._notice and game._best == 12
  assert rebecca._SCORE_FILE.read_text() == '12\n'
  press(game, MousePos(1000, 1000))
  assert game._state == 'flying'


def test_record_survives_reopening(game):
  game._distance = 240
  game.hide_event()
  reopened = rebecca.Rebecca()
  reopened.show_event()
  assert reopened._best == 24
  reopened.hide_event()


@pytest.mark.parametrize('contents', [None, '', 'not a score', '-50', '\N{SNOWMAN}', '9' * 5000])
def test_missing_or_invalid_score_defaults_to_zero(game, contents):
  if contents is None:
    rebecca._SCORE_FILE.unlink()
  else:
    rebecca._SCORE_FILE.write_text(contents, encoding='utf-8')
  game.show_event()
  assert game._best == 0
  press(game, MousePos(1000, 1000))
  assert game._state == 'flying'


def test_unreadable_score_keeps_game_playable(game, monkeypatch):
  monkeypatch.setattr(Path, 'read_text', Mock(side_effect=OSError('unavailable')))
  game.show_event()
  assert game._notice
  press(game, MousePos(1000, 1000))
  assert game._state == 'flying'


def test_audio_unavailable_is_silent_and_owned_audio_is_released(game, monkeypatch):
  game.hide_event()
  monkeypatch.setattr(rl, 'is_audio_device_ready', lambda: False)
  game.show_event()
  assert not game._sounds
  game._play('flap')
  game.hide_event()
  monkeypatch.setattr(rl, 'is_audio_device_ready', Mock(side_effect=[False, True, True]))
  game.show_event()
  game.hide_event()
  rl.close_audio_device.assert_called_once()


@pytest.mark.parametrize('bounds', [(0, 104, 2160, 976), (45, 100, 1280, 620), (0, 104, 2560, 976)])
def test_full_width_button_and_world_fit(game, bounds):
  game.set_rect(rl.Rectangle(*bounds))
  rect, button, s = game.rect, game._button, game._scale
  assert button.x == rect.x and button.width == rect.width
  assert button.y + button.height == pytest.approx(rect.y + rect.height)
  assert game._ox >= rect.x and game._ox + 1080 * s <= rect.x + rect.width + .01
  assert game._oy >= rect.y and game._oy + 416 * s <= button.y + .01
  assert button.height >= 44


def test_audio_assets_are_tiny_mono_pcm():
  folder = Path(__file__).resolve().parents[2] / 'assets' / 'arabesque'
  assert sum(path.stat().st_size for path in folder.glob('*.wav')) < 11 * 1024
  for name in ('flap', 'loss', 'best'):
    with wave.open(str(folder / (name + '.wav'))) as sound:
      assert (sound.getnchannels(), sound.getsampwidth(), sound.getframerate()) == (1, 2, 16000)
      assert sound.getnframes() <= 3200
