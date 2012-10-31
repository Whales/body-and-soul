#include "terrain.h"

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

#define SUBMAP_SIZE 100

struct tile
{
 ter_id ter;
};

struct submap
{
 tile tiles[SUBMAP_SIZE][SUBMAP_SIZE];
};

class map
{
 public:
  map();
  map(int x, int y);
  terrain ter(int x, int y);
  void generate(map_type type);
  void update();
  void add_chunk(map_type type, direction side);
  void clear_old_chunks();
  void resize(int x, int y);

 private:
  submap submaps[3][3];
  
  bool bounded;
};
