#include <istream>
#include <sstream>
#include "itemtype.h"
#include "stringfunc.h"
#include "window.h"

item_type::item_type()
{
  name = "";
  uid = 0;
  value = 0;
  symbol = glyph('x', c_white, c_red);
  weight = 0;
  artifact = false;
}

item_type::~item_type()
{
}

std::string item_type::save_data()
{
  std::stringstream ret;
  ret << name << ": " << description << " " << STD_DELIM << " " <<
         symbol.save_data() << " " << uid << " " << value << " " << weight;
  return ret.str();
}

void item_type::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":", true);
  description = load_to_delim(datastream, STD_DELIM);
  symbol.load_data(datastream);
  datastream >> uid >> value >> weight;
}

it_weapon::it_weapon()
{
  symbol.symbol = '/';
  symbol.fg = c_white;
  symbol.bg = c_black;
  damage = 0;
  accuracy = 0;
  hit_time = 0;
  block = 0;
  str_req = 0;
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
  ret << item_type::save_data() << " " << damage << " " << accuracy << " " <<
         hit_time << " " << block << " ";

  for (int i = 0; i < ATT_MAX; i++) {
    ret << attack_types[i] << " ";
  }
  
  ret << damage_types.size() << " ";
  for (int i = 0; i < damage_types.size(); i++) {
    ret << damage_types[i] << " ";
  }
  return ret.str();
}

void it_weapon::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
  datastream >> damage >> accuracy >> hit_time >> block;
  for (int i = 0; i < ATT_MAX; i++) {
    bool tmpbool;
    datastream >> tmpbool;
    attack_types[i] = tmpbool;
  }
  int tmpsize;
  datastream >> tmpsize;
  for (int i = 0; i < tmpsize; i++) {
    int tmpdam;
    datastream >> tmpdam;
    damage_types.push_back( damage_type(tmpdam) );
  }
}

it_launcher::it_launcher()
{
  symbol.symbol = ')';
  symbol.fg = c_white;
  symbol.bg = c_black;
  damage = 0;
  accuracy = 0;
  fire_time = 0;
  str_req = 0;
  ammo = MISSILECAT_NULL;
  two_handed = false;
}

it_launcher::~it_launcher()
{
}

std::string it_launcher::save_data()
{
  std::stringstream ret;
  ret << item_type::save_data() << " " << damage << " " << accuracy << " " <<
         fire_time << " " << str_req << " " << int(ammo) << " " << two_handed <<
         " ";
  ret << damage_types.size() << " ";
  for (int i = 0; i < damage_types.size(); i++) {
    ret << damage_types[i] << " ";
  }
  return ret.str();
}

void it_launcher::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
  int tmpammo, tmpsize;
  datastream >> damage >> accuracy >> fire_time >> str_req >> tmpammo >>
                two_handed >> tmpsize;
  ammo = missile_category(tmpammo);
  for (int i = 0; i < tmpsize; i++) {
    int tmpdam;
    datastream >> tmpdam;
    damage_types.push_back( damage_type(tmpdam) );
  }
}

it_missile::it_missile()
{
  symbol.symbol = ':';
  symbol.fg = c_white;
  symbol.bg = c_black;
  damage = 0;
  accuracy = 0;
  fire_time = 0;
  str_req = 0;
  ammo = MISSILECAT_NULL;
}

it_missile::~it_missile()
{
}

std::string it_missile::save_data()
{
  std::stringstream ret;
  ret << item_type::save_data() << " " << damage << " " << accuracy << " " <<
         fire_time << " " << str_req << " " << int(ammo) << " " << 
         int(att_type) << " ";
  ret << damage_types.size() << " ";
  for (int i = 0; i < damage_types.size(); i++) {
    ret << damage_types[i] << " ";
  }
  return ret.str();
}

void it_missile::load_data(std::istream &datastream)
{
  int tmpammo, tmpatt, tmpsize;
  item_type::load_data(datastream);
  datastream >> damage >> accuracy >> fire_time >> str_req >> tmpammo >>
                tmpatt >> tmpsize;
  ammo = missile_category(tmpammo);
  att_type = attack_type(tmpatt);
  for (int i = 0; i < tmpsize; i++) {
    int tmpdam;
    datastream >> tmpdam;
    damage_types.push_back( damage_type(tmpdam) );
  }
}

it_armor::it_armor()
{
  symbol.symbol = '[';
  symbol.fg = c_white;
  symbol.bg = c_black;
  ac = 0;
}

it_armor::~it_armor()
{
}

std::string it_armor::save_data()
{
  std::stringstream ret;
  ret << item_type::save_data() << " " << ac << " " << parts_covered.size() <<
         " ";
  for (int i = 0; i < parts_covered.size(); i++) {
    ret << parts_covered[i] << ", ";
  }
  return ret.str();
}

void it_armor::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
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
  symbol.fg = c_white;
  symbol.bg = c_black;
  nutrition = 0;
}

it_food::~it_food()
{
}

std::string it_food::save_data()
{
  std::stringstream ret;
  ret << item_type::save_data() << " " << nutrition;
  return ret.str();
}

void it_food::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
  datastream >> nutrition;
}

it_potion::it_potion()
{
  symbol.symbol = '!';
  symbol.fg = c_white;
  symbol.bg = c_black;
}

it_potion::~it_potion()
{
}

std::string it_potion::save_data()
{
  return item_type::save_data();
}

void it_potion::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
}

it_instrument::it_instrument()
{
  symbol.symbol = '}';
  symbol.fg = c_white;
  symbol.bg = c_black;
}

it_instrument::~it_instrument()
{
}

std::string it_instrument::save_data()
{
  return item_type::save_data();
}

void it_instrument::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
}

it_crystal::it_crystal()
{
  symbol.symbol = '*';
  symbol.fg = c_white;
  symbol.bg = c_black;
  element_id = 0;
}

it_crystal::~it_crystal()
{
}

std::string it_crystal::save_data()
{
  std::stringstream ret;
  ret << item_type::save_data() << " " << element_id;
  return ret.str();
}

void it_crystal::load_data(std::istream &datastream)
{
  item_type::load_data(datastream);
  datastream >> element_id;
}


std::string get_item_category_name(item_category cat)
{
  switch (cat) {
    case ITEMCAT_NULL:       return "nullitem";
    case ITEMCAT_WEAPON:     return "weapon";
    case ITEMCAT_LAUNCHER:   return "launcher";
    case ITEMCAT_MISSILE:    return "missile";
    case ITEMCAT_ARMOR:      return "armor";
    case ITEMCAT_FOOD:       return "food";
    case ITEMCAT_POTION:     return "potion";
    case ITEMCAT_INSTRUMENT: return "instrument";
    case ITEMCAT_CRYSTAL:    return "crystal";
    case ITEMCAT_MAX:        return "BUGmaxitem";
  }
  return "BUGescapedloop";
}

item_category lookup_item_category(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < ITEMCAT_MAX; i++) {
    item_category ret = item_category(i);
    if (no_caps( get_item_category_name(ret) ) == name) {
      return ret;
    }
  }
  return ITEMCAT_NULL;
}

item_type* new_item_type(item_category cat)
{
  switch (cat) {
  case ITEMCAT_NULL: {
    item_type* ret = new item_type;
    return ret;
  }
  case ITEMCAT_WEAPON: {
    it_weapon* ret = new it_weapon;
    return ret;
  }
  case ITEMCAT_LAUNCHER: {
    it_launcher* ret = new it_launcher;
    return ret;
  }
  case ITEMCAT_MISSILE: {
    it_missile* ret = new it_missile;
    return ret;
  }
  case ITEMCAT_ARMOR: {
    it_armor* ret = new it_armor;
    return ret;
  }
  case ITEMCAT_FOOD: {
    it_food* ret = new it_food;
    return ret;
  }
  case ITEMCAT_POTION: {
    it_potion* ret = new it_potion;
    return ret;
  }
  case ITEMCAT_INSTRUMENT: {
    it_instrument* ret = new it_instrument;
    return ret;
  }
  case ITEMCAT_CRYSTAL: {
    it_crystal* ret = new it_crystal;
    return ret;
  }
  case ITEMCAT_MAX:
    debugmsg("Whaaaaat");
    return NULL;
  }
  debugmsg("Escaped switch!");
  return NULL;
}


item_type* load_item_type(std::istream &datastream)
{
  std::string ident = load_to_character(datastream, ":;", true);
  item_type* ret = new_item_type( lookup_item_category(ident) );
  ret->load_data(datastream);
  return ret;
}

std::string get_missile_category_name(missile_category cat)
{
  switch (cat) {
    case MISSILECAT_NULL:   return "NULL";
    case MISSILECAT_THROWN: return "thrown";
    case MISSILECAT_ARROW:  return "arrow";
    case MISSILECAT_BOLT:   return "bolt";
    case MISSILECAT_AMMO:   return "ammo";
    case MISSILECAT_EC:     return "EC pack";
    default:                return "BUG - unknown missile_category";
  }
  return "BUG - escaped missile_category switch";
}
