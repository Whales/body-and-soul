#include <sstream>
#include "terrain.h"
#include "stringfunc.h"
#include "window.h"
#include "globals.h"

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
        ret << get_flag_name( terrain_flag(i) ) << ", ";
    }
    ret << "Done\n";
  }

  bool has_transforms = false;
  for (int i = 0; i < transformations.size() && !has_transforms; i++)
    has_transforms |= (transformations[i] != TER_NULL);
  if (has_transforms) {
    ret << "Transformations: ";
    for (int i = 0; i < transformations.size(); i++) {
      if (transformations[i] != TER_NULL) {
        ret << get_transformation_name( transform_type(i) ) << ">" <<
              TERRAIN_POOL[ transformations[i] ]->name << ", ";
      }
    }
    ret << "Done\n";
  }

  ret << "\nDone\n";
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
    if (no_caps(ident) == "flags:") {
      std::string flagname;
      do {
        flagname = load_to_character(datastream, ";,\n", true);
        flags[ lookup_flag(flagname) ] = true;
      } while (no_caps(flagname) != "done");
    } else if (no_caps(ident) == "transformations") {
      std::string transname, tername;
      do {
        transname = load_to_character(datastream, ">;,\n", true);
        if (no_caps(transname) != "done") {
          tername = load_to_character(datastream, ";,\n", true);
          if (no_caps(tername) != "done") {
            pre_transformations[ lookup_transformation(transname) ] =
              tername;
          }
        }
      } while (no_caps(transname) != "done" && no_caps(tername) != "done");
    }
  } while (no_caps(ident) != "done" && !datastream.eof());
}


transform_type lookup_transformation(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < TRANS_MAX; i++) {
    transform_type tmp = transform_type(i);
    if (no_caps(get_transformation_name(tmp)) == name)
      return tmp;
  }
  debugmsg("Couldn't find transformation named \"%s\".", name.c_str());
  return TRANS_NULL;
}


std::string get_transformation_name(transform_type type)
{
  switch (type) {
    case TRANS_NULL:    return "none";

    case TRANS_FOREST:  return "forest";
    case TRANS_WASTES:  return "wastes";
    case TRANS_SEA:     return "sea";
    case TRANS_MISTS:   return "mists";

    case TRANS_HEAT:    return "heat";
    case TRANS_COLD:    return "cold";

    case TRANS_OPEN:    return "open";
    case TRANS_CLOSE:   return "close";

    case TRANS_MAX:     return "none";

    default:
      debugmsg("No name defined for transform_type %d", type);
      return "error";
  }

  return "wtf";
}

terrain_flag lookup_flag(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < TF_MAX; i++) {
    terrain_flag tmp = terrain_flag(i);
    if (no_caps(get_flag_name(tmp)) == name)
      return tmp;
  }

  return TF_NULL;
}

std::string get_flag_name(terrain_flag flag)
{
  switch (flag) {
    case TF_NULL:
    case TF_MAX:
      return "none";

    case TF_TEST: return "test";

    default:
      debugmsg("Couldn't find name for terrain_flag %d.", flag);
      return "error";
  }

  return "wtf";
}
