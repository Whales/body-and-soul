#include "mapgen.h"

virtual void mapgen_spec::load_data(std::istream &datastream)
{
  debugmsg("ran load_data on flavorless mapgen_spec");
  return;
}

virtual void mapgen_spec_builings::load_data(std::istream &datastream)
{
  std::string id;
  do {
    datastream >> id;
    id = no_caps(id);
  } while (id != "done");
