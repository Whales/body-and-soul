#include "body.h"
#include "soul.h"

class entity
{
 point pos;
 body my_body;
 soul my_soul;

 int challenge_roll(challenge_type type);
 int skill_level(challenge_type type);
};

