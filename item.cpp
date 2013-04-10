#include "item.h"
#include "stringfunc.h"
#include "globals.h"

item::item()
{
  type = NULL;
}

item::~item()
{
}

void item::set_type(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < ITEMS_POOL.size(); i++) {
    if (no_caps(ITEMS_POOL[i]->name) == name) {
      type = ITEMS_POOL[i];
    }
  }
  type = NULL;
}

void item::set_type(int index)
{
  if (index < 0 || index >= ITEMS_POOL.size()) {
    type = NULL;
    return;
  }
  type = ITEMS_POOL[index];
}

glyph item::get_symbol()
{
  if (!type) {
    return glyph('x', c_red, c_white);
  }
  return type->symbol;
}
