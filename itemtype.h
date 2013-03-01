#ifndef _item_type_H_
#define _item_type_H_

#include <istream>
#include <vector>
#include <string>
#include "body.h"
#include "attack.h"

enum item_category {
ITEMCAT_NULL = 0,
ITEMCAT_WEAPON,
ITEMCAT_ARMOR,
ITEMCAT_FOOD,
ITEMCAT_POTION,
ITEMCAT_INSTRUMENT,
ITEMCAT_CRYSTAL,
ITEMCAT_MAX
};

std::string get_item_category_name(item_category cat);
item_category lookup_item_category(std::string name);

struct item_type
{
  item_type();
  ~item_type();

  virtual item_category type() { return ITEMCAT_NULL; };

  virtual std::string save_data();
  virtual void load_data(std::istream &datastream);

  virtual bool covers(std::string partname) { return false; }

  std::string name;

  int uid;
  int value;
  glyph symbol;

  int weight;
};

struct it_weapon : public item_type
{
  it_weapon();
  ~it_weapon();

  virtual item_category type() { return ITEMCAT_WEAPON; };
  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  int damage;
  int accuracy;
  int hit_time;
  int block;

  std::vector<bool> attack_types;
  std::vector<damage_type> damage_types;
};

struct it_launcher : public item_type
{
  
}

struct it_armor : public item_type
{
  it_armor(); // Must init parts_covered!
  ~it_armor();

  virtual item_category type() { return ITEMCAT_ARMOR; };
  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  int ac;
  virtual bool covers(std::string partname);

private:
  std::vector<std::string> parts_covered;
};

struct it_food : public item_type
{
  it_food();
  ~it_food();

  virtual item_category type() { return ITEMCAT_FOOD; };
  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  int nutrition;
};

struct it_potion : public item_type
{
  it_potion();
  ~it_potion();

  virtual item_category type() { return ITEMCAT_POTION; };

  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);
// Nothing special - this is handled in a switch somewhere
// TODO: Or is it?????
};

struct it_instrument : public item_type
{
  it_instrument();
  ~it_instrument();

  virtual item_category type() { return ITEMCAT_INSTRUMENT; };

  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  //std::vector<song> songs;
};

struct it_crystal : public item_type
{
  it_crystal();
  ~it_crystal();

  virtual item_category type() { return ITEMCAT_CRYSTAL; };

  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  int element_id;
};

item_type* load_item_type(std::istream &datastream);

#endif
