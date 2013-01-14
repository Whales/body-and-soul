#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <istream>
#include "glyph.h"

enum terrain_flag
{
  TF_NULL = 0,
  TF_MAX
};

enum terrain_id
{
  TER_NULL = 0,
// Floor-like tiles
  TER_FLOOR,
  TER_DIRT,
  TER_SAND,
// Water-like tiles
  TER_WATER,
// Wall-like tiles
  TER_WALL,
  TER_ROCK,
// Other tiles
  TER_TREE,
  TER_MAX
};

struct terrain_type
{
  std::string name;
  int uid;
  int move_cost;
  int sight_cost;
  glyph symbol;

  std::vector<bool> flags;
  std::vector<tile_id> transformations;
 

  terrain_type();
  ~terrain_type()

  std::string save_data();
  void load_data(std::istream &datastream);
};

#endif
