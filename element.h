#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string>
#include <vector>
#include <istream>
#include "terrain.h"
#include "attack.h"

struct element
{
  element();
  ~element();

  std::string save_data();
  void load_data(std::istream &datastream);

  std::string name;
  int id;
  nc_color color;
  std::vector<transform_type> transformations;
  std::vector<damage_type> damages;
};

#endif
