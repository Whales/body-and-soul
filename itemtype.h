#ifndef _ITEMTYPE_H_
#define _ITEMTYPE_H_

enum attack_type {
ATT_NULL = 0,
ATT_SLASH,
ATT_STAB,
ATT_BLUDGEON,
ATT_MAX
};

struct itemtype
{
  int weight;
  int damage;
  int to_hit;

#endif
