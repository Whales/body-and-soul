#include "terrain.h"
#include "globals.h"

#define SUBMAP_SIZE 100

enum map_type
{
 MAP_NULL,
// Test
 MAP_TEST,
// In the city
 MAP_CITY,
 MAP_LABYRINTH,
 MAP_CITADEL,
 MAP_PARK,
 MAP_SEWERS,
// Outside the city
 MAP_WOODS,
 MAP_WASTES,
 MAP_MISTS,
 MAP_SEA,

 MAP_MAX
};

struct tile
{
  tile() { type = NULL; };
  ~tile(){};

  void set_type(terrain_id id) { type = TERRAIN_POOL[id]; };
  void set_type(terrain_type *_type) { type = _type; };
  terrain_type *type;
  
};

struct submap
{
  submap();
  ~submap();
  tile tiles[SUBMAP_SIZE][SUBMAP_SIZE];
};

struct transformer
{
  transform_type type;
  int posx;
  int posy;
};

class map
{
 public:
// Constructors
  map();

// Accessors
  tile& ter(int x, int y);

// Mutators
  void generate(map_type type);
  void update();
  void resize(int x, int y);

 private:
  submap submaps[3][3];
  
  bool bounded;
};
