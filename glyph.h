#ifndef _GLYPH_H_
#define _GLYPH_H_

#include <istream>
#include "color.h"

struct glyph
{
 long symbol;
 nc_color fg;
 nc_color bg;

 std::string save_data();
 void load_data(std::istream &datastream);

 //void draw(Window *w, point reference);
};

#endif
