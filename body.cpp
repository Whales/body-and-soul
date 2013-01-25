#include <sstream>
#include "body.h"
#include "rng.h"
#include "stringfunc.h"

std::string body::save_data()
{
 std::stringstream ret;
 ret << symbol.save_data() << name << "; " << body_parts.size() << " ";
 for (std::list<body_part>::iterator it = body_parts.begin();
      it != body_parts.end(); it++)
  ret << it->save_data() << " ";

 return ret.str();
}

void body::load_data(std::istream &datastream)
{
 symbol.load_data(datastream);
 name = load_to_character(datastream, ";", true);

 int num_parts;
 datastream >> num_parts;

 for (int i = 0; i < num_parts; i++) {
  body_part tmppart;
  tmppart.load_data(datastream);
  body_parts.push_back(tmppart);
 }
}

std::string body_ability::save_data()
{
  std::stringstream ret;
  ret << type << " " << level;
  return ret.str();
}

void body_ability::load_data(std::istream &datastream)
{
  int tmptype;
  datastream >> tmptype >> level;
  type = body_ability_id(tmptype);
}

std::string body_part::save_data()
{
  std::stringstream ret;
  ret << name << "; " << maxhp << " " << curhp << " " << weight << " " <<
         strength << " " << dexterity << " " << perception << " " <<
         speed << " ";
  ret << abilities.size() << " ";
  for (std::list<body_ability>::iterator it = abilities.begin();
       it != abilities.end();
       it++) {
    ret << it->save_data() << " ";
  }

  return ret.str();
}

void body_part::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ";", true);

  int abilsize;
  datastream >> maxhp >> curhp >> weight >> strength >> dexterity >>
                perception >> speed >> abilsize;
  for (int i = 0; i < abilsize; i++) {
    body_ability tmp;
    tmp.load_data(datastream);
    abilities.push_back(tmp);
  }
}
