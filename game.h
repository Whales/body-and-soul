#ifndef _GAME_H_
#define _GAME_H_

#include "entity.h"
#include "map.h"

class game
{
 public:
  entity player;
  map curMap;

 private:
  Window w_terrain;
  Window w_body;
  Window w_soul;
};

#endif
