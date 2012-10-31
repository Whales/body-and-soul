#ifndef _BODY_H_
#define _BODY_H_

#include <list>
#include <string>
#include <istream>
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

struct body_part
{
 int hp;
 std::string name;
 std::list<body_stat> stats;
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
