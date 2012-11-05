#ifndef _GLYPH_H_
#define _GLYPH_H_

#include <istream>
#include "color.h"

struct glyph
{
 long symbol;
 nc_color fg;
 nc_color bg;

 glyph() { symbol = ' '; fg = c_black; bg = c_black; };
 glyph(long S, nc_color F, nc_color B) : symbol (S), fg (F), bg (B) { };

 bool operator==(const glyph &rhs);

 std::string save_data();
 void load_data(std::istream &datastream);

 //void draw(Window *w, point reference);
};

#endif
