#include "body.h"
#include "rng.h"

int body::challenge_roll(challenge_type type)
{
 return rng( 0, skill_level(type) );
}

int body::skill_level(challenge_type type)
{
 int ret = 0, num = 0, biggest = 0;
 std::list<body_part>::iterator bp_it;
 std::list<body_stat>::iterator bs_it;
 std::list<challenge_type>::iterator ch_it;
 for (bp_it = body_parts.begin(); bp_it != body_parts.end(); bp_it++) {
  for (bs_it = bp_it->stats.begin(); bs_it != bp_it->stats.end(); bs_it++) {
   for (ch_it = bs_it->chals.begin(); ch_it != bs_it->chals.end(); ch_it++) {
    if (*ch_it == type) {
     num++;
     ret += bs_it->value;
     if (bs_it->value > biggest)
      biggest = bs_it->value;
    }
   }
  }
 }

// Subtract 10% for each limb past the first, preserving the biggest
 if (num > 1) {
  ret -= biggest;
  ret -= (ret * (num - 1)) / 10;
  ret += biggest;
 }

 return ret;
}
