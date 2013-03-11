#include "attack.h"

std::string get_damage_type_name(damage_type type)
{
  switch (type) {
    case DAMTYPE_NULL:       return "Null";
    case DAMTYPE_MUNDANE:    return "Physical";
    case DAMTYPE_FIRE:       return "Heat";
    case DAMTYPE_ICE:        return "Cold";
    case DAMTYPE_CORROSIVE:  return "Corrosive";
    case DAMTYPE_ELECTRIC:   return "Electric";
    case DAMTYPE_DRAIN:      return "Soul-Draining";
    case DAMTYPE_SCATTER:    return "Soul-Scattering";
    case DAMTYPE_OBLITERATE: return "Soul-Obliterating";
    case DAMTYPE_MAX: return "BUG - DAMTYPE_MAX";
    default:          return "BUG - Forgot damtype name";
  }
  return "BUG - Escaped damtype switch";
}

std::string get_attack_type_name(attack_type type)
{
  switch (type) {
    case ATT_NULL:     return "None";
    case ATT_BLUDGEON: return "Bludgeon";
    case ATT_SWIPE:    return "Swipe";
    case ATT_STAB:     return "Stab";
    default:           return "BUG - get_attack_type_name";
  }
  return "BUG - Escaped get_attack_type_name switch!";
}
