import json
import zipfile

from openpilot.system.ui.lib.emoji import ASSET_DIR, runs, text_units, tokens
from openpilot.system.ui.lib import emoji_render as render


def test_all_unicode_sequences_have_artwork():
  sequences = json.loads((ASSET_DIR / 'sequences.json').read_text())
  with zipfile.ZipFile(ASSET_DIR / 'images.zip') as z:
    for sequence, name in sequences.items():
      assert tokens(sequence) == ((sequence, name),)
      assert z.read(name + '.png').startswith(b'\x89PNG\r\n\x1a\n')


def test_joined_emoji_are_single_units():
  assert text_units('A👨‍👩‍👧‍👦🇨🇦👍🏽1️⃣Z') == ('A', '👨‍👩‍👧‍👦', '🇨🇦', '👍🏽', '1️⃣', 'Z')
  assert not any(image for _, image in tokens('123 # *'))
  assert not any(image for _, image in tokens('♥\ufe0e'))
  assert runs('Lebowski 👀📡')[0] == ('Lebowski ', None)


def test_plain_text_uses_original_measurement(monkeypatch):
  value = object()
  monkeypatch.setattr(render.rl, 'measure_text_ex', lambda *args: value)
  assert render.measure(None, 'normal text', 20, 0) is value


def test_draw_positions_match_measured_width(monkeypatch):
  def measure(font, text, size, spacing):
    lines = text.split('\n')
    return render.rl.Vector2(max(len(line) * size / 2 + max(0, len(line) - 1) * spacing for line in lines),
                             size + (len(lines) - 1) * (size + 5))
  monkeypatch.setattr(render.rl, 'measure_text_ex', measure)
  placed, measured = render._layout(None, 'a🇨🇦b\n👍🏽', 20, 2)
  assert [p[2] for p in placed] == [0, 12, 34, 0]
  assert measured.x == 44
  assert measured.y == 45
  assert placed[-1][3] == 25


def test_texture_cache_is_bounded_and_unloaded(monkeypatch):
  from types import SimpleNamespace
  unloaded = []
  monkeypatch.setattr(render, '_TEXTURE_LIMIT', 2)
  monkeypatch.setattr(render.rl, 'load_image_from_memory', lambda *a: object())
  monkeypatch.setattr(render.rl, 'load_texture_from_image', lambda *a: SimpleNamespace(width=72, height=72))
  monkeypatch.setattr(render.rl, 'unload_image', lambda *a: None)
  monkeypatch.setattr(render.rl, 'set_texture_filter', lambda *a: None)
  monkeypatch.setattr(render.rl, 'rl_draw_render_batch_active', lambda: None)
  monkeypatch.setattr(render.rl, 'unload_texture', unloaded.append)
  for name in ('1f440', '1f4e1', '1f600'):
    render._texture(name)
  assert len(render._textures) == 2
  assert len(unloaded) == 1
  render.unload()
  assert len(unloaded) == 3
  assert render._archive is None


def test_draw_accepts_raylib_tuple_colours_and_preserves_alpha(monkeypatch):
  from types import SimpleNamespace
  draws = []
  monkeypatch.setattr(render, '_texture', lambda name: SimpleNamespace(width=72, height=72))
  monkeypatch.setattr(render, '_layout', lambda *args: ([('👀', '1f440', 0, 0)], None))
  monkeypatch.setattr(render.rl, 'draw_texture_pro', lambda *args: draws.append(args))
  render.draw(None, '👀', (10, 20), 24, 0, (100, 100, 100, 128), None)
  assert draws[0][2].x == 10
  assert draws[0][-1].a == 128
  assert draws[0][-1].r == 255


def test_wrapping_never_splits_a_joined_emoji(monkeypatch):
  from types import SimpleNamespace
  from openpilot.system.ui.lib import wrap_text
  monkeypatch.setattr(wrap_text, 'measure_text_cached',
                      lambda font, text, size, spacing: SimpleNamespace(x=len(text_units(text)) * 10))
  assert wrap_text._break_long_word(None, 'ab👨‍👩‍👧‍👦🇨🇦c', 20, 20) == ['ab', '👨‍👩‍👧‍👦🇨🇦', 'c']
  assert wrap_text._break_long_word(None, '👍🏽', 20, 1) == ['👍🏽']
