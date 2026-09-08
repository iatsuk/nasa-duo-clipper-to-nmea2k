// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstddef>
#include <cstdint>
#include <cmath>

namespace duet {
constexpr size_t frame_bits = 137;
constexpr size_t frame_bytes = 18;
enum class Result { ignored_command, malformed, display };
struct Reading {
    double depth_m = NAN;
    double speed_mps = NAN;
    bool normal_screen = false;
};
// Input is the on-wire bit sequence, packed MSB-first into each byte.
// Only a complete HT1621 write of 32 nibbles from address zero is accepted.
Result decode(const uint8_t* bytes, size_t size, size_t bits, Reading& out);

class State {
public:
    void ingest(const uint8_t* data, size_t size, size_t bits, uint32_t now);
    Reading current(uint32_t now, uint32_t timeout_ms);
    void invalidate();
    uint32_t frames = 0, rejected = 0;
private:
    Reading reading_;
    uint32_t last_ = 0;
    bool seen_ = false;
};
}
