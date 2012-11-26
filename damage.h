#ifndef _DAMAGE_H_
#define _DAMAGE_H_

enum damage_type_id {
DAMTYPE_NULL = 0,
// Mundane
DAMTYPE_IMPACT,
DAMTYPE_CUT,
DAMTYPE_PIERCE,
// Elemental
DAMTYPE_FIRE,
DAMTYPE_ICE,
DAMTYPE_ACID,
DAMTYPE_ELECTRIC, // Is also potentially soul-damaging
// Soul
DAMTYPE_DRAIN, // e.g. soul theft
DAMTYPE_SCATTER, // e.g. soul wind
DAMTYPE_OBLITERATE, // e.g. qlippoth
DAMTYPE_MAX
};

#endif
