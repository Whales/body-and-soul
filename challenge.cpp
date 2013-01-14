#include "challenge.h"


std::string challenge_name(int type)
{
  return challenge_name( challenge_type(type) );
}

std::string challenge_name(challenge_type type)
{
  switch (type) {
    case CHAL_NULL:
      return "Nothing";
// Intelligence
    case CHAL_REASON:
      return "Reasoning";
    case CHAL_PERSUADE:
      return "Persuasion";
// Perception
    case CHAL_SPOT:
      return "Detection";
    case CHAL_TARGET:
      return "Targeting";
// Strength: Arms
    case CHAL_PUSH:
      return "Push";
    case CHAL_PUNCH_DAMAGE:
      return "Punch Damage";
// Dexterity: Arms
    case CHAL_BLOCK:
      return "Block";
    case CHAL_PUNCH:
      return "Punch Accuracy";
// Strength: Torso
    case CHAL_LIFT:
      return "Lifting";
// Endurance
    case CHAL_RESIST:
      return "Resistance";
// Strength: Legs
    case CHAL_KICK_DAMAGE:
      return "Kick Damage";
    case CHAL_JUMP:
      return "Jumping";
// Dexterity: Legs
    case CHAL_KICK:
      return "Kick Accuracy";
    case CHAL_DODGE:
      return "Dodging";
    default:
      return "Unknown";
  }
  return "Missed default";

}
