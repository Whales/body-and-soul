#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <istream>
#include <vector>
#include "glyph.h"

enum terrain_flag
{
  TF_NULL = 0,
  TF_TEST,
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

enum transform_type
{
  TRANS_NULL = 0,

  TRANS_FOREST,
  TRANS_WASTES,
  TRANS_SEA,
  TRANS_MISTS,

  TRANS_HEAT,
  TRANS_COLD,

  TRANS_OPEN,
  TRANS_CLOSE,

  TRANS_MAX
};

transform_type lookup_transformation(std::string name);
std::string get_transformation_name(transform_type type);

terrain_flag lookup_flag(std::string name);
std::string get_flag_name(terrain_flag flag);

std::string default_terrain_name(terrain_id type);

struct terrain_type
{
  std::string name;
  int uid;
  int move_cost;
  int sight_cost;
  glyph symbol;

  std::vector<bool> flags;
  std::vector<terrain_id> transformations;

  terrain_type();
  ~terrain_type();

  std::string save_data();
  void load_data(std::istream &datastream);

  void init_transformations();

private:
  std::vector<std::string> pre_transformations;
};

#endif
