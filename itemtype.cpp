#include <istream>
#include <sstream>
#include "itemtype.h"
#include "stringfunc.h"

itemtype::itemtype()
{
  name = "nothing";
  uid = 0;
  value = 0;
  symbol = glyph('x', c_white, c_red);
  weight = 0;
}

itemtype::~itemtype()
{
}

std::string itemtype::save_data()
{
  std::stringstream ret;
  ret << name << ": " << symbol.save_data() << " " << uid << " " << value <<
         " " << weight;
  return ret.str();
}

void itemtype::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":", true);
  symbol.load_data(datastream);
  datastream >> uid >> value >> weight;
}

it_weapon::it_weapon()
{
  symbol.symbol = ')';
  damage = 0;
  accuracy = 0;
  hit_time = 10;
  block = 0;
  for (int i = 0; i < ATT_MAX; i++) {
    attack_types.push_back(false);
  }
}

it_weapon::~it_weapon()
{
}

std::string it_weapon::save_data()
{
  std::stringstream ret;
  ret << itemtype::save_data() << " " << damage << " " << accuracy << " " <<
         hit_time << " " << block << " ";

  for (int i = 0; i < ATT_MAX; i++) {
    ret << attack_types[i] << " ";
  }
  
  ret << damage_types.size() << " ";
  for (int i = 0; i < damage_types.size(); i++) {
    ret << damage_types[i] << " ";
  }
}

void it_weapon::load_data(std::istream &datastream)
{
  itemtype::load_data(datastream);
  datastream >> damage >> accuracy >> hit_time >> block;
  for (int i = 0; i < ATT_MAX; i++) {
    int tmpatt;
    datastream >> tmpatt;
    attack_types.push_back( attack_type(tmpatt) );
  }
  int tmpsize;
  datastream >> tmpsize;
  for (int i = 0; i < tmpsize; i++) {
    int tmpdam;
    datastream >> tmpdam;
    damage_types.push_back( damage_type(tmpdam) );
  }
}

it_armor::it_armor()
{
  symbol.symbol = '[';
  ac = 0;
}

it_armor::~it_armor()
{
}

std::string it_armor::save_data()
{
  std::stringstream ret;
  ret << itemtype::save_data() << " " << ac << " " << parts_covered.size() <<
         " ";
  for (int i = 0; i < parts_covered.size(); i++) {
    ret << parts_covered[i] << ", ";
  }
  return ret.str();
}

void it_armor::load_data(std::istream &datastream)
{
  itemtype::load_data(datastream);
  int tmpsize;
  datastream >> ac >> tmpsize;
  for (int i = 0; i < tmpsize; i++) {
    std::string tmppart = load_to_character(datastream, ',', true);
    parts_covered.push_back(tmppart);
  }
}

bool it_armor::covers(std::string partname)
{
  partname = no_caps(partname);
  for (int i = 0; i < parts_covered.size(); i++) {
    if (no_caps(parts_covered[i]) == partname) {
      return true;
    }
  }
  return false;
}

it_food::it_food()
{
  symbol.symbol = '%';
  nutrition = 0;
}

it_food::~it_food()
{
}

std::string it_food::save_data()
{
  std::stringstream ret;
  ret << itemtype::save_data() << " " << nutrition;
  return ret.str();
}

void it_food::load_data(std::istream &datastream)
{
  itemtype::load_data(datastream);
  datastream >> nutrition;
}

it_potion::it_potion()
{
  symbol.symbol = '!';
}

it_potion::~it_potion()
{
}

std::string it_potion::save_data()
{
  return itemtype::save_data();
}

void it_potion::load_data(std::istream &datastream)
{
  itemtype::load_data(datastream);
}

it_instrument::it_instrument()
{
  symbol.symbol = '}';
}

it_instrument::~it_instrument()
{
}

std::string it_instrument::save_data()
{
  return itemtype::save_data();
}

void it_instrument::load_data(std::istream &datastream)
{
  itemtype::load_data(datastream);
}

it_crystal::it_crystal()
{
  symbol.symbol = '*';
  element_id = 0;
}

it_crystal::~it_crystal()
{
}

std::string it_crystal::save_data()
{
  std::stringstream ret;
  ret << itemtype::save_data() << " " << element_id;
  return ret.str();
}

void it_crystal::load_data(std::istream &datastream)
{
  itemtype::load_data(datastream);
  datastream >> element_id;
}

