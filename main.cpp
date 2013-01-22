#include "globals.h"
#include "terrain.h"

int main()
{
  init_environment();
  init_data();
  init_display();

  terrain_type* tmp = new terrain_type;

  tmp->name = "null";
  tmp->uid = 0;
  tmp->move_cost = 0;
  tmp->sight_cost = 0;
  tmp->symbol = glyph('x', c_white, c_red);
  TERRAIN_POOL.push_back(tmp);

  tmp = new terrain_type;
  tmp->name = "floor";
  tmp->uid = 1;
  tmp->move_cost = 10;
  tmp->sight_cost = 0;
  tmp->symbol = glyph('.', c_white, c_black);
  tmp->flags[TF_TEST] = true;
  TERRAIN_POOL.push_back(tmp);

  save_data();
  return 0;
}
