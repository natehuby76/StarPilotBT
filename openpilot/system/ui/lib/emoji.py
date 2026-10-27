"""Offline Unicode 17 emoji segmentation. No graphics context is needed here."""
from functools import lru_cache
import json
from pathlib import Path

ASSET_DIR = Path(__file__).resolve().parents[1] / 'assets/emoji'


@lru_cache(maxsize=1)
def _trie():
  root = {}
  for sequence, image in json.loads((ASSET_DIR / 'sequences.json').read_text()).items():
    node = root
    for char in sequence:
      node = node.setdefault(char, {})
    node[''] = image
  return root


@lru_cache(maxsize=1024)
def tokens(text: str) -> tuple[tuple[str, str | None], ...]:
  """Keep complete emoji sequences together; ordinary characters remain separate."""
  if text.isascii():
    return tuple((char, None) for char in text)
  root = _trie()
  result = []
  i = 0
  while i < len(text):
    node, j, end, image = root, i, i, None
    while j < len(text) and text[j] in node:
      node = node[text[j]]
      j += 1
      if '' in node:
        end, image = j, node['']
    # An explicit text presentation selector must not become a colour emoji.
    if end > i and text[end:end + 1] != '\ufe0e':
      result.append((text[i:end], image))
      i = end
    else:
      result.append((text[i], None))
      i += 1
  return tuple(result)


def text_units(text: str) -> tuple[str, ...]:
  return tuple(part for part, _ in tokens(text))


@lru_cache(maxsize=1024)
def runs(text: str) -> tuple[tuple[str, str | None], ...]:
  if text.isascii():
    return ((text, None),)
  result = []
  plain = ''
  for part, image in tokens(text):
    if image is None:
      plain += part
    else:
      if plain:
        result.append((plain, None))
        plain = ''
      result.append((part, image))
  if plain:
    result.append((plain, None))
  return tuple(result)
