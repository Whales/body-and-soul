#ifndef _GLYPH_H_
#define _GLYPH_H_

#include "color.h"

struct glyph
{
 long symbol;
 nc_color fg;
 nc_color bg;

 //void draw(Window *w, point reference);
};

#endif
