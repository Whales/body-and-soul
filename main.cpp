#include <string>
#include <sstream>
#include "globals.h"
#include "terrain.h"

int main()
{
  init_environment();
  init_data();
  init_display();

  terrain_type* nullter = new terrain_type;
 
  tmp->name = "null";
  tmp->uid = 0;
  tmp->move_cost = 0;
  tmp->sight_cost = 0;
  tmp->symbol = glyph('x', c_white, c_red);

  std::vector<std::string> type_names;
  for (int i = 0; i < TER_MAX; i++) {
    if (i >= TERRAIN_POOL.size() || TERRAIN_POOL[i] == NULL) {
      type_names.push_back("UNINITIALIZED");
    } else {
      terrain_type* cur = TERRAIN_POOL[i];
      std::stringstream typedata;
      typedata << "<c=" << color_name( cur->symbol.fg ) << "," <<
                  color_name( cur->symbol.bg ) << ">" <<
                  char(cur->symbol.symbol) << "<c=/> " << cur->name;
      type_names.push_back( typedata.str() );
    }
  }
  
    tmp = new terrain_type;
    tmp->name = "floor";
    tmp->uid = 1;
    tmp->move_cost = 10;
    tmp->sight_cost = 0;
    tmp->symbol = glyph('.', c_white, c_black);
    tmp->flags[TF_TEST] = true;
    tmp->transformations[TRANS_HEAT] = TER_DIRT;
    TERRAIN_POOL.push_back(tmp);
  
    tmp = new terrain_type;
    tmp->name = "dirt";
    tmp->uid = 2;
    tmp->move_cost = 10;
    tmp->sight_cost = 0;
    tmp->symbol = glyph('.', c_brown, c_black);
    tmp->flags[TF_TEST] = true;
    tmp->transformations[TRANS_COLD] = TER_FLOOR;
    TERRAIN_POOL.push_back(tmp);
  
  }
  debugmsg("Size: %d", TERRAIN_POOL.size());

  save_data();
  end_display();
  return 0;
}
