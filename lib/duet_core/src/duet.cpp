// SPDX-License-Identifier: GPL-3.0-or-later
#include "duet.h"
#include "segment_map.h"

namespace duet {
namespace {
int digit(uint8_t segments) {
    static constexpr uint8_t digits[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
    segments &= 0x7f;
    for (int n=0; n<10; ++n) if (segments==digits[n]) return n;
    return segments==0 ? -2 : -1;  // blank versus unknown/error text
}
double number(const uint8_t* segments, size_t count, bool decimal) {
    double value=0;
    bool started=false;
    for (size_t n=0; n<count; ++n) {
        int d=digit(segments[n]);
        if (d==-2 && !started) continue;
        if (d<0) return NAN; // embedded/trailing blanks or letters never mean zero
        started=true;
        value=value*10+d;
    }
    return started ? value/(decimal ? 10.0 : 1.0) : NAN;
}
}

Result decode(const uint8_t* p, size_t size, size_t bits, Reading& out) {
    out=Reading{};
    if (!p || bits<3 || size < (bits+7)/8) return Result::malformed;
    const uint8_t command=p[0]>>5;
    // Command traffic does not refresh readings. Read/unknown traffic invalidates them.
    if (command==4) return Result::ignored_command;
    if (command!=5 || bits!=frame_bits || size<frame_bytes) return Result::malformed;
    const uint8_t address=((p[0]&0x1f)<<1) | (p[1]>>7);
    if (address!=0) return Result::malformed;
    using namespace detail;
    out.normal_screen=segdata(24,0,p);
    if (!out.normal_screen) return Result::display;

    uint8_t lower[]={mkdigit4(p),mkdigit5(p),mkdigit6(p)};
    const bool metres=segdata(20,0,p), feet=segdata(20,1,p);
    if (metres!=feet) {
        out.depth_m=number(lower,3,segdata(23,0,p));
        if (feet) out.depth_m*=0.3048;
        // Zero depth remains zero; OUT, blanks, and missing units produce NA.
    }

    // TRIP/TOTAL replace speed on the LCD: invalidate speed immediately.
    if (segdata(28,0,p) || segdata(29,0,p)) return Result::display;
    const bool knots=segdata(31,1,p), mph=segdata(30,0,p);
    const bool km=segdata(30,1,p), per_hour=segdata(31,0,p);
    double factor=NAN;
    if (knots && !mph && !km && !per_hour) factor=1852.0/3600.0;
    if (mph && !knots && !km && !per_hour) factor=0.44704;
    if (km && per_hour && !knots && !mph) factor=1.0/3.6;
    if (segdata(29,1,p) || segdata(24,1,p)) return Result::display;
    uint8_t upper[]={mkdigit0(p),mkdigit1(p),mkdigit2(p),mkdigit3(p)};
    out.speed_mps=number(upper,4,segdata(6,0,p))*factor;
    return Result::display;
}

void State::ingest(const uint8_t* p,size_t size,size_t bits,uint32_t now) {
    Reading next;
    const auto result=decode(p,size,bits,next);
    if (result==Result::ignored_command) return;
    if (result==Result::malformed) { ++rejected; invalidate(); return; }
    ++frames;
    reading_=next;
    seen_=true;
    last_=now;
}
Reading State::current(uint32_t now,uint32_t timeout_ms) {
    if (seen_ && uint32_t(now-last_)>=timeout_ms) invalidate();
    return seen_ ? reading_ : Reading{};
}
void State::invalidate() { seen_=false; reading_=Reading{}; }
}
