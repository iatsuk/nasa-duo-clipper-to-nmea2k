# Attribution

`lib/duet_core/src/segment_map.h` contains the segment-to-wire mapping extracted
from Soenke J. Peters' `speters/ClipperDuet2N2k`, commit
`27619d4b321be46d96fcb02a9dbee318b089d9bc`, `src/main.cpp`.
Copyright 2023 Soenke J. Peters. MIT license reproduced in
`ClipperDuet2N2k-LICENSE.txt`. Changes: const buffer arguments and namespace;
only the seven digit mappings and bit accessor retained.

The rest of that firmware (SPI wrapper, web/OTA, seven-segment font, NMEA0183,
alarm code, default offsets) is not vendored or used. New application code
and documentation are GPL-3.0-or-later, preserving this repository's license.

The two NMEA libraries by Timo Lappalainen are downloaded at pinned commits
in platformio.ini. Their MIT notices are distributed in their original source.
PlatformIO/Espressif framework and toolchain licenses remain with those packages.
