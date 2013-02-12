#include <sstream>
#include "body.h"
#include "rng.h"
#include "stringfunc.h"

std::string body::save_data()
{
 std::stringstream ret;
 ret << symbol.save_data() << name << "; " << body_parts.size() << " ";
 for (std::vector<body_part>::iterator it = body_parts.begin();
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

void body::init()
{
  for (int i = 0; i < body_parts.size(); i++) {
    body_parts[i].curhp = body_parts[i].maxhp;
  }
}

int body::get_str()
{
  std::vector<int> vals;
  for (int i = 0; i < body_parts.size(); i++) {
    if (body_parts[i].strength > 0) {
      bool inserted = false;
      for (int j = 0; !inserted && j < vals.size(); j++) {
        if (vals[j] < body_parts[i].strength) {
          vals.insert( vals.begin() + j, body_parts[i].strength );
          inserted = true;
        }
      }
      if (!inserted) {
        vals.push_back( body_parts[i].strength );
      }
    }
  }
  int ret = 0;
  for (int i = 0; i < vals.size(); i++) {
    int penalty = i * 2;
    if (vals[i] >= penalty) {
      ret += vals[i] - penalty;
    }
  }
  return ret;
}

int body::get_dex()
{
  std::vector<int> vals;
  for (int i = 0; i < body_parts.size(); i++) {
    if (body_parts[i].strength > 0) {
      bool inserted = false;
      for (int j = 0; !inserted && j < vals.size(); j++) {
        if (vals[j] < body_parts[i].dexterity) {
          vals.insert( vals.begin() + j, body_parts[i].dexterity );
          inserted = true;
        }
      }
      if (!inserted) {
        vals.push_back( body_parts[i].dexterity );
      }
    }
  }
  int ret = 0;
  for (int i = 0; i < vals.size(); i++) {
    int penalty = i * 2;
    if (vals[i] >= penalty) {
      ret += vals[i] - penalty;
    }
  }
  return ret;
}

int body::get_per()
{
  std::vector<int> vals;
  for (int i = 0; i < body_parts.size(); i++) {
    if (body_parts[i].perception > 0) {
      bool inserted = false;
      for (int j = 0; !inserted && j < vals.size(); j++) {
        if (vals[j] < body_parts[i].perception) {
          vals.insert( vals.begin() + j, body_parts[i].perception );
          inserted = true;
        }
      }
      if (!inserted) {
        vals.push_back( body_parts[i].perception );
      }
    }
  }
  int ret = 0;
  for (int i = 0; i < vals.size(); i++) {
    int penalty = i * 2;
    if (vals[i] >= penalty) {
      ret += vals[i] - penalty;
    }
  }
  return ret;
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

body_part::body_part()
{
  maxhp = 100;
  weight = 1;
  strength = 0;
  dexterity = 0;
  perception = 0;
  speed = 0;
}

body_part::~body_part()
{
}

std::string body_part::save_data()
{
  std::stringstream ret;
  ret << name << "; " << maxhp << " " << curhp << " " << weight << " " <<
         strength << " " << dexterity << " " << perception << " " <<
         speed << " ";
  ret << abilities.size() << " ";
  for (std::vector<body_ability>::iterator it = abilities.begin();
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

std::string body_ability_name(body_ability_id type)
{
  switch (type) {
    case BODY_ABILITY_NULL:
    case BODY_ABILITY_MAX:      return "Null";
    case BODY_ABILITY_FLIGHT:   return "Flight";
    case BODY_ABILITY_ARMOR:    return "Armor";
    case BODY_ABILITY_ENCUMBER: return "Encumber";
    case BODY_ABILITY_EVASION:  return "Evasion";
    case BODY_ABILITY_DEFENSE:  return "Defense";
    case BODY_ABILITY_ATTACK:   return "Attack";

    default: return "Bug-Unnamed";
  }

  return "Bug-EscapedSwitch";
}

std::vector<std::string> body::body_part_names()
{
  std::vector<std::string> ret;
  for (int i = 0; i < body_parts.size(); i++) {
    ret.push_back(body_parts[i].name);
  }
  return ret;
}

std::vector<std::string> body::body_part_hps()
{
  std::vector<std::string> ret;
  for (int i = 0; i < body_parts.size(); i++) {
    std::stringstream hp_data;
    int hp = body_parts[i].curhp, maxhp = body_parts[i].maxhp;
    hp_data << percentage_coloring(hp, maxhp) << hp << "<c=/>";
    ret.push_back(hp_data.str());
  }
  return ret;
}

std::vector<std::string> body::body_part_acs()
{
  std::vector<std::string> ret;
  return ret;
}
