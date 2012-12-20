#ifndef _BODY_H_
#define _BODY_H_

#include <list>
#include <string>
#include <istream>
#include <vector>
#include "challenge.h"
#include "glyph.h"

struct body_stat
{
 std::string name;
 int value;
 std::list<challenge_type> chals;

 std::string save_data();
 void load_data(std::istream &datastream);
};

enum body_ability_id {
BODY_ABILITY_NULL = 0,
BODY_ABILITY_FLIGHT,
BODY_ABILITY_ARMOR,
BODY_ABILITY_ENCUMBER,
BODY_ABILITY_EVASION,
BODY_ABILITY_DEFENSE,
BODY_ABILITY_ATTACK,
BODY_ABILITY_MAX
};

struct body_ability
{
 body_ability_id type;
 int level;
 std::vector<int> data;

 std::string save_data();
 void load_data(std::istream &datastream);
};

struct body_part
{
 int hp;
 std::string name;
 std::list<body_stat> stats;
 std::list<body_ability> abilities;

 std::string save_data();
 void load_data(std::istream &datastream);
};

struct body
{
 std::string name;
 std::list<body_part> body_parts;
 glyph symbol;

 int challenge_roll(challenge_type type);
 int skill_level(challenge_type type);

 std::string save_data();
 void load_data(std::istream &datastream);
};

#endif
