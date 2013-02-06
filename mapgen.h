#ifndef _MAPGEN_H_
#define _MAPGEN_H_

#include <istream>
#include <vector>

void load_mapgen_specs();

enum mapgen_type
{
  MAPGEN_NULL,

  MAPGEN_BUILDINGS, // Rooms with doors and stuff
  MAPGEN_LABYRINTH, // Maze-like
  MAPGEN_HEIGHTMAP, // Perlin noise; "height" is used loosely here

  MAPGEN_MAX
};

mapgen_type lookup_mapgen_type(std::string name);
std::string mapgen_type_name(mapgen_type type);

struct mapgen_spec
{
  std::string name;

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
  int road_id;
  
  int density;
  int numroads;
  int minsize, maxsize;
  int minspacing, maxspacing;
  int minroad, maxroad;
};

struct mapgen_spec_labyrinth : public mapgen_spec
{
  virtual mapgen_type gentype() { return MAPGEN_LABYRINTH; };
  virtual void load_data(std::istream &datastream);

  int wall_id;
  int floor_id;

  int minwidth, maxwidth;
};

struct height_point
{
  int ter_id;
  int percentile;
};

struct mapgen_spec_heightmap : public mapgen_spec
{
  virtual mapgen_type gentype() { return MAPGEN_HEIGHTMAP; };
  virtual void load_data(std::istream &datastream);

  std::list< std::vector<height_point> > maps;
};

struct map_connect
{
  map_connect(int P, int W) : pos(P), width(W) { };
  ~map_connect(){};
  int pos;
  int width;
};

#endif
