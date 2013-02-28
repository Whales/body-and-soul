#ifndef _BODY_H_
#define _BODY_H_

#include <list>
#include <string>
#include <istream>
#include <vector>
#include "glyph.h"

enum body_ability_id {
BODY_ABILITY_NULL = 0,
BODY_ABILITY_FLIGHT,  // Takes a power amount (against weight)
BODY_ABILITY_ARMOR,   // Takes a damage resistance
BODY_ABILITY_ENCUMBER,// Takes an encumbrance amount
BODY_ABILITY_EVASION, // Takes an evasion boost/penalty
BODY_ABILITY_DEFENSE, // Takes a blocking chance
BODY_ABILITY_ATTACK,  // Takes a damage amount
BODY_ABILITY_ANCHOR,  // Anchors other body parts
BODY_ABILITY_MAX
};

enum armor_type {
ARMOR_TYPE_NULL = 0,
ARMOR_TYPE_HELMET,
ARMOR_TYPE_BODY,
ARMOR_TYPE_BOOTS,
ARMOR_TYPE_GLOVES,
ARMOR_TYPE_MAX
};

std::string body_ability_name(body_ability_id type);

struct body_ability
{
 body_ability_id type;
 int level;

 body_ability(body_ability_id _type = BODY_ABILITY_NULL, int _level = 0) :
  type(_type), level(_level) { };
 ~body_ability() { };

 std::string save_data();
 void load_data(std::istream &datastream);
};

struct body_part
{
 std::string name;
 int maxhp, curhp;
 int weight;
 int strength;
 int dexterity;
 int perception;
 int speed;
 std::vector<body_ability> abilities;

 body_part();
 ~body_part();

 std::string save_data();
 void load_data(std::istream &datastream);
};

struct body
{
 std::string name;
 std::vector<body_part> body_parts;
 glyph symbol;
 int size;

 void init();

 int get_str();
 int get_dex();
 int get_per();

 int get_dodge();
 int get_block();
 int get_damage();

 std::vector<std::string> body_part_names();
 std::vector<std::string> body_part_hps();
 std::vector<std::string> body_part_acs();

 std::string save_data();
 void load_data(std::istream &datastream);
};

#endif
