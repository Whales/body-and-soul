#ifndef _ITEMTYPE_H_
#define _ITEMTYPE_H_

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

struct itemtype
{
  itemtype();
  ~itemtype();

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

struct it_weapon : public itemtype
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

struct it_armor : public itemtype
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

struct it_food : public itemtype
{
  it_food();
  ~it_food();

  virtual item_category type() { return ITEMCAT_FOOD; };
  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  int nutrition;
};

struct it_potion : public itemtype
{
  it_potion();
  ~it_potion();

  virtual item_category type() { return ITEMCAT_POTION; };

  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);
// Nothing special - this is handled in a switch somewhere
// TODO: Or is it?????
};

struct it_instrument : public itemtype
{
  it_instrument();
  ~it_instrument();

  virtual item_category type() { return ITEMCAT_INSTRUMENT; };

  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  //std::vector<song> songs;
};

struct it_crystal : public itemtype
{
  it_crystal();
  ~it_crystal();

  virtual item_category type() { return ITEMCAT_CRYSTAL; };

  virtual std::string save_data();
  virtual void        load_data(std::istream &datastream);

  int element_id;
};

#endif
