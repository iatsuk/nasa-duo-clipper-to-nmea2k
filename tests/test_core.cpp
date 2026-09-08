// SPDX-License-Identifier: GPL-3.0-or-later
#include "duet.h"
#include "fixtures.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>

void near(double got,double expected) { assert(std::isfinite(got)); assert(std::abs(got-expected)<1e-8); }
int main() {
    duet::Reading r;
    assert(duet::decode(metres_knots,18,137,r)==duet::Result::display);
    near(r.depth_m,12.3); near(r.speed_mps,5.4*1852/3600);
    assert(r.normal_screen);
    duet::decode(feet_mph,18,137,r); near(r.depth_m,3.048); near(r.speed_mps,4.4704);
    duet::decode(kmh,18,137,r); near(r.speed_mps,5.0);
    duet::decode(zero,18,137,r); near(r.depth_m,0); near(r.speed_mps,0);
    duet::decode(out,18,137,r); assert(std::isnan(r.depth_m)); assert(std::isfinite(r.speed_mps));
    duet::decode(trip,18,137,r); near(r.depth_m,12.3); assert(std::isnan(r.speed_mps));

    uint8_t bad[18];
    std::memcpy(bad,metres_knots,18);
    // Depth metre indicator missing (echo-lost blink); values must not persist.
    const size_t metre=9+20*4;
    bad[metre/8]&=~(1<<(7-metre%8));
    duet::decode(bad,18,137,r); assert(std::isnan(r.depth_m));
    std::memcpy(bad,metres_knots,18);
    const size_t divider=9+24*4;
    bad[divider/8]&=~(1<<(7-divider%8));
    duet::decode(bad,18,137,r); assert(!r.normal_screen && std::isnan(r.depth_m) && std::isnan(r.speed_mps));
    std::memcpy(bad,metres_knots,18); bad[1]|=0x80;
    assert(duet::decode(bad,18,137,r)==duet::Result::malformed);
    for(size_t bits=0;bits<137;++bits) assert(duet::decode(metres_knots,18,bits,r)!=duet::Result::display);
    assert(duet::decode(metres_knots,17,137,r)==duet::Result::malformed);
    assert(duet::decode(nullptr,18,137,r)==duet::Result::malformed);
    assert(duet::decode(metres_knots,18,138,r)==duet::Result::malformed);

    duet::State s;
    assert(std::isnan(s.current(0,2000).depth_m));
    s.ingest(metres_knots,18,137,0);
    near(s.current(1999,2000).depth_m,12.3);
    assert(std::isnan(s.current(2000,2000).depth_m));
    s.ingest(metres_knots,18,137,0xfffffff0u);
    near(s.current(0x10,2000).depth_m,12.3);
    assert(std::isnan(s.current(0x7c0,2000).depth_m));
    s.ingest(metres_knots,18,137,20);
    s.ingest(out,18,137,21); assert(std::isnan(s.current(21,2000).depth_m));
    s.ingest(metres_knots,18,137,22);
    s.ingest(bad,18,137,23); assert(std::isnan(s.current(23,2000).depth_m));
    s.ingest(metres_knots,18,137,100);
    uint8_t command[]={0x8c,0};
    s.ingest(command,2,12,2099);
    assert(std::isnan(s.current(2100,2000).depth_m)); // command never renews freshness

    std::mt19937 random(42);
    uint8_t noise[24];
    for(int i=0;i<20000;++i) {
        for(auto& x:noise) x=static_cast<uint8_t>(random());
        const size_t bytes=random()%25, bits=random()%200;
        duet::decode(noise,bytes,bits,r);
        assert(std::isnan(r.depth_m) || r.depth_m>=0);
        assert(std::isnan(r.speed_mps) || r.speed_mps>=0);
    }
    std::cout<<"PASS: units, zero/OUT, screens, frame bounds, invalidation, timeout/wrap, 20000 noise inputs\n";
}
