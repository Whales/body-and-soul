#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <istream>
#include "glyph.h"

struct terrain_type
{
 std::string name;
 int move_cost;
 glyph symbol;

 terrain_type() { name = "UNINITIALIZED"; move_cost = 0; };
 terrain_type(std::string N, int M, glyph S) : name (N), move_cost (M),
                                               symbol (S) { };

 std::string save_data();
 void load_data(std::istream &datastream);
};

#endif
