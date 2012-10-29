#include <list>
#include <string>
#include "challenge.h"
#include "glyph.h"

struct body_stat
{
 std::list<challenge_type> chals;
 int value;
};

struct body_part
{
 int hp;
 std::string name;
 std::list<body_stat> stats;
};

struct body
{
 std::list<body_part> body_parts;
 glyph sym;

 int challenge_roll(challenge_type type);
 int skill_level(challenge_type type);
};
