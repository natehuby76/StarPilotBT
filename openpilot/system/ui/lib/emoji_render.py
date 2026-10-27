"""Colour emoji alongside raylib text, using the same layout for drawing and sizing."""
from collections import OrderedDict
import zipfile

import pyray as rl
from openpilot.system.ui.lib.emoji import ASSET_DIR, runs

_TEXTURE_LIMIT = 128  # At most ~2.6 MiB of RGBA emoji textures (72 x 72).
_textures: OrderedDict = OrderedDict()
_archive = None


def unload():
  global _archive
  for texture in _textures.values():
    rl.unload_texture(texture)
  _textures.clear()
  if _archive is not None:
    _archive.close()
    _archive = None


def _texture(name):
  global _archive
  if name in _textures:
    _textures.move_to_end(name)
    return _textures[name]
  if _archive is None:
    _archive = zipfile.ZipFile(ASSET_DIR / 'images.zip')
  data = _archive.read(name + '.png')
  image = rl.load_image_from_memory('.png', data, len(data))
  try:
    texture = rl.load_texture_from_image(image)
  finally:
    rl.unload_image(image)
  rl.set_texture_filter(texture, rl.TextureFilter.TEXTURE_FILTER_BILINEAR)
  _textures[name] = texture
  if len(_textures) > _TEXTURE_LIMIT:
    _, old = _textures.popitem(last=False)
    rl.rl_draw_render_batch_active()  # Finish queued draws before freeing their texture.
    rl.unload_texture(old)
  return texture


def _layout(font, text, size, spacing):
  lines = text.split('\n')
  height = rl.measure_text_ex(font, 'M', size, spacing).y  # noqa: TID251
  step = rl.measure_text_ex(font, 'M\nM', size, spacing).y - height  # noqa: TID251
  placed, width = [], 0.0
  for row, line in enumerate(lines):
    x = 0.0
    for part, image in runs(line):
      if not part:
        continue
      if x:
        x += spacing
      advance = size if image else rl.measure_text_ex(font, part, size, spacing).x  # noqa: TID251
      placed.append((part, image, x, row * step))
      x += advance
    width = max(width, x)
  return placed, rl.Vector2(width, height + (len(lines) - 1) * step)


def measure(font, text, size, spacing):
  if not any(image for _, image in runs(text)):
    return rl.measure_text_ex(font, text, size, spacing)  # noqa: TID251
  return _layout(font, text, size, spacing)[1]


def draw(font, text, position, size, spacing, tint, draw_plain):
  if not any(image for _, image in runs(text)):
    return draw_plain(font, text, position, size, spacing, tint)
  placed, _ = _layout(font, text, size, spacing)
  px, py = (position.x, position.y) if hasattr(position, "x") else position
  alpha = tint.a if hasattr(tint, "a") else tint[3]
  for part, image, x, y in placed:
    pos = rl.Vector2(px + x, py + y)
    if image:
      texture = _texture(image)
      rl.draw_texture_pro(texture, rl.Rectangle(0, 0, texture.width, texture.height),
                          rl.Rectangle(pos.x, pos.y, size, size), rl.Vector2(0, 0), 0,
                          rl.Color(255, 255, 255, alpha))
    else:
      draw_plain(font, part, pos, size, spacing, tint)
