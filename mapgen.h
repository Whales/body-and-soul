#ifndef _MAPGEN_H_
#define _MAPGEN_H_

#include <istream>

enum mapgen_type
{
  MAPGEN_NULL,

  MAPGEN_BUILDINGS, // Rooms with doors and stuff
  MAPGEN_LABYRINTH, // Maze-like
  MAPGEN_HEIGHTMAP, // Perlin noise; "height" is used loosely here

  MAPGEN_MAX
};

struct mapgen_spec
{
  virtual mapgen_type gentype() { return MAPGEN_NULL; };

  virtual void load_data(std::istream &datastream);
};

struct mapgen_spec_buildings : public mapgen_spec
{
  virtual mapgen_type gentype() { return MAPGEN_BUILDINGS; };
  virtual void load_data(std::istream &datastream);

  int wall_id;
  int floor_id;
  int door_id;
  int outside_id;
  
  int size_low, size_high;
  int spacing_low, spacing_high;
};

struct mapgen_spec_labyrinth : public mapgen_spec
{
  virtual mapgen_type gentype() { return MAPGEN_LABYRINTH; };
  virtual void load_data(std::istream &datastream);

  int wall_id;
  int floor_id;

  int width_low, width_high;
};

struct heightmap
{
  std::vector<int> terrain;
};

struct mapgen_spec_heightmap : public mapgen_spec
{
  virtual mapgen_type gentype() { return MAPGEN_HEIGHTMAP; };
  virtual void load_data(std::istream &datastream);

  std::vector<heightmap> maps;
};

struct mapgen_spec_container()
{
  void load_data(std::istream &datastream);
  std::vector<mapgen_spec*> specs;
};
  
#endif
