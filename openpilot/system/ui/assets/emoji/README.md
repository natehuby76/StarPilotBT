# Colour emoji

Twemoji 17.0.3 artwork by Twitter and the Twemoji contributors, licensed under
CC BY 4.0 (see LICENSE-GRAPHICS). https://github.com/jdecked/twemoji/tree/v17.0.3
The PNG artwork is unchanged, repackaged in images.zip for offline use.

Unicode 17 sequence definitions: https://www.unicode.org/Public/17.0.0/emoji/emoji-test.txt
Copyright Unicode, Inc. See LICENSE-UNICODE. sequences.json is derived from that data.
SOURCE.json records the input hashes and coverage. Rebuild with tools/ui/build_emoji_assets.py.

Covers every sequence in Unicode 17 emoji-test.txt, including qualified/unqualified
variants, flags, skin tones and joined sequences. Future Unicode additions need a
bundle update. This is native raylib text support; browser/Qt renderers keep their
own text handling. Text selection/editing is not a full Unicode grapheme editor.
