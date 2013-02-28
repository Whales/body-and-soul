#include <sstream>
#include "element.h"
#include "stringfunc.h"

element::element()
{
  name = "null";
  id = 0;
  color = c_white;
}

element::~element()
{
}

std::string element::save_data()
{
  std::stringstream ret;
  ret << name << ": " << id << " " << int(color) << " ";
  ret << transformations.size() << " ";
  for (int i = 0; i < transformations.size(); i++) {
    ret << int(transformations[i]) << " ";
  }
  ret << damages.size() << " ";
  for (int i = 0; i < damages.size(); i++) {
    ret << int(damages[i]) << " ";
  }

  return ret.str();
}

void element::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ':', true);
  int tmpcolor, tmpsize;
  datastream >> id >> tmpcolor >> tmpsize;
  color = nc_color(tmpcolor);
  for (int i = 0; i < tmpsize; i++) {
    int tmptrans;
    datastream >> tmptrans;
    transformations.push_back( transform_type(tmptrans) );
  }
  datastream >> tmpsize;
  for (int i = 0; i < tmpsize; i++) {
    int tmpdam;
    datastream >> tmpdam;
    damages.push_back( damage_type(tmpdam) );
  }
}
