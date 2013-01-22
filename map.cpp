#include "map.h"
#include "globals.h"

submap::submap()
{
  for (int i = 0; i < SUBMAP_SIZE; i++) {
    for (int j = 0; j < SUBMAP_SIZE; j++)
      tiles[i][j].set_type(TERRAIN_POOL[0]);
  }
}

map::map()
{
}
