#include "map.h"
#include "mapgen.h"
#include "stringfunc.h"
#include "window.h"

void load_mapgen_specs()
{
  for (int i = 0; i < MAP_MAX; i++) {
    std::vector<mapgen_spec*> tmp;
    MAPGEN_POOL.push_back(tmp);
  }

  
}

void mapgen_spec::load_data(std::istream &datastream)
{
  debugmsg("ran load_data on flavorless mapgen_spec");
  return;
}

void mapgen_spec_buildings::load_data(std::istream &datastream)
{
  std::string id;
  do {
    datastream >> id;
    id = no_caps(id);
  } while (id != "done");
}
