// SPDX-License-Identifier: MIT
// Copyright 2023 Soenke J. Peters
// Extracted from speters/ClipperDuet2N2k, commit
// 27619d4b321be46d96fcb02a9dbee318b089d9bc.
// Changes: const input pointers; only segment mapping retained.
// Full license: third_party/ClipperDuet2N2k-LICENSE.txt
#pragma once
#include <cstdint>
namespace duet { namespace detail {
inline uint8_t segdata(uint8_t seg, uint8_t com, const uint8_t *buf)
{
  const uint8_t shift_by = 3 + 6;
  return ((buf[(seg * 4 + com + shift_by) / 8] >> (7 - ((seg * 4 + com + shift_by) % 8))) & 1);
}

// Bit order 7..0: (dot) g f e d c b a
inline uint8_t mkdigit0(const uint8_t *buf) { return (segdata(13, 1, buf) << 6) | (segdata(14, 0, buf) << 5) | (segdata(14, 1, buf) << 4) |
                                               (segdata(15, 1, buf) << 3) | (segdata(12, 1, buf) << 2) | (segdata(12, 0, buf) << 1) | (segdata(13, 0, buf)); }

inline uint8_t mkdigit1(const uint8_t *buf) { return (segdata(11, 1, buf) << 6) | (segdata(15, 0, buf) << 5) | (segdata(7, 1, buf) << 4) |
                                               (segdata(7, 0, buf) << 3) | (segdata(10, 1, buf) << 2) | (segdata(10, 0, buf) << 1) | (segdata(11, 0, buf)); }

inline uint8_t mkdigit2(const uint8_t *buf) { return (segdata(8, 1, buf) << 6) | (segdata(9, 0, buf) << 5) | (segdata(9, 1, buf) << 4) |
                                               (segdata(6, 1, buf) << 3) | (segdata(16, 1, buf) << 2) | (segdata(16, 0, buf) << 1) | (segdata(8, 0, buf)) |
                                               (segdata(6, 0, buf) << 7); }

inline uint8_t mkdigit3(const uint8_t *buf) { return (segdata(26, 1, buf) << 6) | (segdata(25, 0, buf) << 5) | (segdata(25, 1, buf) << 4) |
                                               (segdata(28, 1, buf) << 3) | (segdata(27, 1, buf) << 2) | (segdata(27, 0, buf) << 1) | (segdata(26, 0, buf)); }

inline uint8_t mkdigit4(const uint8_t *buf) { return (segdata(3, 1, buf) << 6) | (segdata(4, 1, buf) << 5) | (segdata(4, 0, buf) << 4) |
                                               (segdata(3, 0, buf) << 3) | (segdata(2, 0, buf) << 2) | (segdata(2, 1, buf) << 1) | (segdata(5, 1, buf)); }

inline uint8_t mkdigit5(const uint8_t *buf) { return (segdata(0, 1, buf) << 6) | (segdata(1, 1, buf) << 5) | (segdata(1, 0, buf) << 4) |
                                               (segdata(0, 0, buf) << 3) | (segdata(17, 0, buf) << 2) | (segdata(17, 1, buf) << 1) | (segdata(23, 1, buf)) |
                                               (segdata(23, 0, buf) << 7); }

inline uint8_t mkdigit6(const uint8_t *buf) { return (segdata(18, 1, buf) << 6) | (segdata(22, 1, buf) << 5) | (segdata(22, 0, buf) << 4) |
                                               (segdata(18, 0, buf) << 3) | (segdata(19, 0, buf) << 2) | (segdata(19, 1, buf) << 1) | (segdata(21, 1, buf)); }


}}
