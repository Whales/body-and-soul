#include <sstream>
#include "glyph.h"

std::string glyph::save_data()
{
 std::stringstream ret;
 ret << symbol << " " << int(fg) << " " << int(bg);
 return ret.str();
};

void glyph::load_data(std::istream &datastream)
{
 int fgtmp, bgtmp;
 datastream >> symbol >> fgtmp >> bgtmp;

 fg = nc_color(fgtmp);
 bg = nc_color(bgtmp);
}

glyph glyph::invert()
{
  glyph ret = (*this);
  nc_color tmp = ret.fg;
  ret.fg = ret.bg;
  ret.bg = tmp;
  return ret;
}

glyph glyph::hilite()
{
  glyph ret = (*this);
  ret.bg = HILITE_COLOR;
  return ret;
}

bool glyph::operator==(const glyph &rhs)
{
 return (rhs.fg == fg && rhs.bg == bg && rhs.symbol == symbol);
}
