// SPDX-License-Identifier: GPL-3.0-or-later
// Parametric dry-location ADAPTER enclosure. Does not include the NASA instrument.
// Measure the complete module assembly and change these dimensions before printing.
part = "both"; // [base,lid,both]
inner_x = 120;
inner_y = 85;
inner_z = 38;
wall = 2.4;
floor_t = 2.4;
lid_t = 2.4;
fit = 0.3;
screw_d = 2.7; // pilot for chosen self-tapping screw; test print first
clearance_d = 3.4;
corner_r = 5;
post_d = 9;
post_offset = 7;
// Optional holes along X walls; zero means drill after fitting actual cable glands.
gland_d = 0;
outer_x = inner_x+2*wall;
outer_y = inner_y+2*wall;
outer_z = inner_z+floor_t;
$fn = 48;
assert(inner_x>40 && inner_y>40 && inner_z>15);
assert(wall>1.5 && fit>0 && fit<wall/2);

module rounded_box(x,y,z,r) {
    hull() for (a=[r,x-r], b=[r,y-r]) translate([a,b,0]) cylinder(h=z,r=r);
}
module screw_positions() {
    for (x=[wall+post_offset,outer_x-wall-post_offset],
         y=[wall+post_offset,outer_y-wall-post_offset]) translate([x,y,0]) children();
}
module base() {
    difference() {
        union() {
            difference() {
                rounded_box(outer_x,outer_y,outer_z,corner_r);
                translate([wall,wall,floor_t])
                    rounded_box(inner_x,inner_y,inner_z+1,corner_r-wall);
            }
            screw_positions() cylinder(h=outer_z,d=post_d);
            // Raised tie-down rails: hold a removable insulating mounting plate.
            for(y=[outer_y/2-15,outer_y/2+15])
                translate([wall+15,y,floor_t]) cube([inner_x-30,4,3]);
        }
        screw_positions() translate([0,0,floor_t+3]) cylinder(h=inner_z,d=screw_d);
        if(gland_d>0)
            for(x=[-1,outer_x-wall-1])
                translate([x,outer_y/2,outer_z/2]) rotate([0,90,0]) cylinder(h=wall+2,d=gland_d);
    }
}
// Printed flat, outer face on bed; lip points up during printing, down in use.
module lid() {
    difference() {
        union() {
            rounded_box(outer_x,outer_y,lid_t,corner_r);
            translate([wall+fit,wall+fit,lid_t]) difference() {
                rounded_box(inner_x-2*fit,inner_y-2*fit,2,corner_r-wall);
                translate([1.4,1.4,-0.1])
                    rounded_box(inner_x-2*fit-2.8,inner_y-2*fit-2.8,2.2,1);
            }
        }
        screw_positions() translate([0,0,-1]) cylinder(h=lid_t+4,d=clearance_d);
        // Clearance to base posts: lip never collides with corner pillars.
        screw_positions() translate([0,0,lid_t]) cylinder(h=3,d=post_d+2*fit);
    }
}
if(part=="base" || part=="both") base();
if(part=="lid") lid();
if(part=="both") translate([outer_x+10,0,0]) lid();
