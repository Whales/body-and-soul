#ifndef _ITEM_H_
#define _ITEM_H_

#include <string>
#include "itemtype.h"

struct item
{
  item();
  ~item();

  void set_type(std::string name);

  glyph get_symbol();

  item_type *type;
};


#endif
