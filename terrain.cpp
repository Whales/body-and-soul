#include <sstream>
#include "terrain.h"

std::string terrain_type::save_data()
{
 std::stringstream ret;
 ret << name << " " << move_cost << " " << symbol.save_data();
 return ret.str();
}

void terrain_type::load_data(std::istream &datastream)
{
 std::string glyphdata;
 datastream >> name >> move_cost;
 symbol.load_data(datastream);
}
