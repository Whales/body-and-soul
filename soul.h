enum soul_class {
SOUL_NULL = 0, // Never used in gameplay
SOUL_NEGATIVE, // Qlippoth
SOUL_NONE, // Zombie
SOUL_LOW, // Dead Inside
SOUL_NORMAL, // Normal person
SOUL_HIGH, // Sensitive
SOUL_VERY_HIGH, // Mage
SOUL_MAX // Never used in gameplay
};

struct soul
{
 soul_class sclass;
 int mp; // For casting spells
 int points; // Permanent soul points
 int karma; // Karma; -100 to 100
 int level[SOUL_ABIL_MAX]; // Ability levels
};
