#include <sstream>
#include "terrain.h"

std::string terrain_type::save_data()
{
  std::stringstream ret;
  ret << name << "; " << uid << " " << move_cost << " " << sight_cost << " " <<
         symbol.save_data() << std::endl;

  bool has_flags = false;
  for (int i = 0; i < flags.size() && !has_flags; i++)
    has_flags |= flags[i];
  if (has_flags) {
    ret << "Flags: ";
    for (int i = 0; i < flags.size(); i++) {
      if (flags[i])
        ret << flag_name( terrain_flag(i) ) << ",";
    }
  }

  bool has_transforms = false;
  for (int i = 0; i < transformations.size() && !has_transforms; i++) {
  return ret.str();
}

void terrain_type::load_data(std::istream &datastream)
{
  std::string glyphdata;
  name = load_to_character(datastream, ";");
  datastream >> name >> move_cost >> sight_cost;
  symbol.load_data(datastream);
  
}
