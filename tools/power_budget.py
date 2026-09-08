#!/usr/bin/env python3
"""Explicit power assumptions, not a claim of measured consumption."""
import argparse
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--voltage', type=float, default=12)
p.add_argument('--esp-ma', type=float, default=70)
p.add_argument('--can-ma', type=float, default=45)
p.add_argument('--aux-ma', type=float, default=5)
p.add_argument('--nasa-ma', type=float, default=20)
p.add_argument('--efficiency', type=float, default=0.85)
p.add_argument('--diode-v', type=float, default=0.35)
a = p.parse_args()
if not (0 < a.efficiency <= 1 and a.voltage > a.diode_v >= 0
        and min(a.esp_ma, a.can_ma, a.aux_ma, a.nasa_ma) >= 0):
    p.error('Need positive input headroom, efficiency 0..1, non-negative currents')
bridge = 5*(a.esp_ma+a.can_ma+a.aux_ma)/1000/a.efficiency/(a.voltage-a.diode_v)
total = bridge+a.nasa_ma/1000
print('ESTIMATE; standalone sonar and MFD are excluded.')
print(f'Bridge: {bridge*1000:.1f} mA, {bridge*a.voltage:.3f} W at {a.voltage:g} V')
print(f'Bridge + assumed NASA: {total*1000:.1f} mA, {total*a.voltage:.3f} W')
print(f'24 h: {total*24:.2f} Ah, {total*a.voltage*24:.2f} Wh')
