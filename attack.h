#ifndef _ATTACK_H_
#define _ATTACK_H_

#include <string>

enum damage_type {
DAMTYPE_NULL = 0,
// Mundane
DAMTYPE_MUNDANE,
// Elemental
DAMTYPE_FIRE,
DAMTYPE_ICE,
DAMTYPE_CORROSIVE,
DAMTYPE_ELECTRIC, // Is also potentially soul-damaging
// Soul
DAMTYPE_DRAIN, // e.g. soul theft
DAMTYPE_SCATTER, // e.g. soul wind
DAMTYPE_OBLITERATE, // e.g. qlippoth
DAMTYPE_MAX
};

enum attack_type {
ATT_NULL = 0,
ATT_BLUDGEON,
ATT_SWIPE,
ATT_STAB,
ATT_MAX
};

std::string get_damage_type_name(damage_type type);



#endif
