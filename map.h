#include "terrain.h"

enum map_type
{
 MAP_NULL,
 MAP_TEST,
 MAP_CITY,
 MAP_LABYRINTH,
 MAP_CITADEL,
 MAP_
class map
{
 public:
  terrain ter(int x, int y);
  void generate(map_type type);

 private:
  terrain termap[MAPSIZEX][MAPSIZEY];
};
