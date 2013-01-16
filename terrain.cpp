#include <sstream>
#include "terrain.h"
#include "stringfunc.h"

terrain_type::terrain_type()
{
  name = "UNINITIALIZED";
  uid = -1;
  move_cost = 0;
  sight_cost = 0;
  symbol = glyph('x', c_white, c_red);

  for (int i = 0; i < TF_MAX; i++) {
    flags.push_back(false);
  }
  for (int i = 0; i < TRANS_MAX; i++) {
    transformations.push_back(TER_NULL);
  }
}

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
  name = load_to_character(datastream, ";", true);
  datastream >> name >> move_cost >> sight_cost;
  symbol.load_data(datastream);

  std::string ident;
  do {
    datastream >> ident;
    if (ident == "Flags:") {
      std::string flagname;
      do {
        flagname = load_to_character(datastream, ";,\n", true);
        flags[ lookup_flag(flagname) ] = true;
      } while (all_caps(flagname) != "Done");
    }
  }
}


