// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <cstdint>
#if __has_include("local_config.h")
#include "local_config.h"
#endif

// Enable only AFTER hardware/decoder commissioning. No CAN driver starts otherwise.
#ifndef DUET_COMMISSIONED
#define DUET_COMMISSIONED 0
#endif
// Set NASA keel offset to ZERO, then verify depth against the original display.
#ifndef DUET_ZERO_OFFSET_VERIFIED
#define DUET_ZERO_OFFSET_VERIFIED 0
#endif
// Signed N2K offset, metres: negative = transducer to keel, positive = to surface.
#ifndef DUET_OUTPUT_OFFSET_M
#define DUET_OUTPUT_OFFSET_M 0.0
#endif
namespace config {
constexpr int data = 23, clock = 18, chip_select = 27;
constexpr int can_tx = 21, can_rx = 22, arm = 32;
constexpr uint32_t stale_ms = 2000, send_ms = 500;
constexpr double offset_m = DUET_OUTPUT_OFFSET_M;
static_assert(offset_m >= -20.0 && offset_m <= 20.0, "Check offset in metres");
static_assert(stale_ms < 0x80000000UL, "Wrap-safe timeout required");
}
