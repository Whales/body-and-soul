#ifndef _COLOR_H_
#define _COLOR_H

#include <curses.h>

void init_colors();

enum nc_color {
c_black = 0,
c_ltgray,
c_red,
c_green,
c_blue,
c_cyan,
c_magenta,
c_brown,
// "Bright" colors
c_dkgray,
c_white,
c_ltred,
c_ltgreen,
c_ltblue,
c_ltcyan,
c_pink,
c_yellow
};

long get_color_pair(nc_color fg, nc_color bg);

#endif
