"""Build the offline emoji bundle from Twemoji 17.0.3 and Unicode 17 emoji-test.txt.

Usage: python tools/ui/build_emoji_assets.py twemoji.tar.gz emoji-test.txt
Artwork: CC-BY-4.0. Unicode data: Unicode-3.0. See bundled notices.
"""
import hashlib
import json
from pathlib import Path
import sys
import tarfile
import zipfile

out = Path(__file__).resolve().parents[2] / 'openpilot/system/ui/assets/emoji'
out.mkdir(parents=True, exist_ok=True)
archive = Path(sys.argv[1])
data = Path(sys.argv[2])
with tarfile.open(archive) as tar:
  images = {Path(m.name).stem: tar.extractfile(m).read() for m in tar.getmembers() if '/assets/72x72/' in m.name and m.name.endswith('.png')}
  license_member = next(m for m in tar.getmembers() if m.name.endswith('/LICENSE-GRAPHICS'))
  (out / 'LICENSE-GRAPHICS').write_bytes(tar.extractfile(license_member).read())
normalized = {"-".join(c for c in name.split("-") if c != "fe0f"): name for name in images}
sequences = {}
missing = []
for line in data.read_text().splitlines():
  if ';' not in line or line.startswith('#'):
    continue
  codes, status = line.split(';', 1)
  points = [int(c, 16) for c in codes.split()]
  key = '-'.join(f'{c:x}' for c in points)
  stripped = '-'.join(f'{c:x}' for c in points if c != 0xfe0f)
  name = key if key in images else normalized.get(stripped, stripped)
  if name not in images:
    missing.append(key)
    continue
  sequences[''.join(map(chr, points))] = name
assert not missing, missing
(out / 'sequences.json').write_text(json.dumps(sequences, ensure_ascii=False, sort_keys=True, separators=(',', ':')) + '\n')
with zipfile.ZipFile(out / 'images.zip', 'w', zipfile.ZIP_DEFLATED) as z:
  for name in sorted(set(sequences.values())):
    info = zipfile.ZipInfo(name + '.png', (2026, 1, 1, 0, 0, 0))
    info.compress_type = zipfile.ZIP_DEFLATED
    z.writestr(info, images[name])
(out / 'SOURCE.json').write_text(json.dumps({'twemoji': 'https://github.com/jdecked/twemoji/tree/v17.0.3',
  'unicode': 'https://www.unicode.org/Public/17.0.0/emoji/emoji-test.txt',
  'archive_sha256': hashlib.sha256(archive.read_bytes()).hexdigest(),
  'unicode_sha256': hashlib.sha256(data.read_bytes()).hexdigest(),
  'sequences': len(sequences), 'images': len(set(sequences.values()))}, indent=2) + '\n')
print(len(sequences), 'sequences;', len(set(sequences.values())), 'images')
