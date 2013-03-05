#include <string>
#include <sstream>
#include "globals.h"
#include "itemtype.h"
#include "interface.h"
#include "window.h"

void delete_item(int index);
void edit_item(item_type* itype);
void add_item();

int main()
{
  init_display();
  init_environment();
  init_data();

  cuss::interface i_list;
  if (!i_list.load_from_file("cuss/i_item_ed.cuss")) {
    debugmsg("Can't load cuss/i_item_ed.cuss");
    end_display();
    return 1;
  }
  Window w_list(0, 0, 80, 24);

  std::vector<std::string> item_names;
  for (int i = 0; i < ITEMS_POOL.size(); i++) {
    item_names.push_back( ITEMS_POOL[i]->name );
  }
  i_list.set_data("list_items", item_names);

  bool quit = false;
  while (!quit) {
    i_list.draw(&w_list);

    int selnum = i_list.get_int("list_items");
    item_type* curtype = NULL;
    if (selnum >= 0 && selnum < ITEMS_POOL.size()) {
      curtype = ITEMS_POOL[selnum];
    }

    long ch = getch();
    if (ch == 's' || ch == 'S') {
      quit = true;
    } else if (ch == 'd' || ch == 'D') {
      delete_item(selnum);
    } else if (ch == 'e' || ch == 'E') {
      edit_item(curtype);
    } else if (ch == 'a' || ch == 'A') {
      add_item();
    }
  }

  save_data();
  end_display();
  return 0;
}

std::vector<std::string> get_item_names()
{
  std::vector<std::string> ret;
  for (int i = 0; i < ITEMS_POOL.size(); i++) {
    ret.push_back( ITEMS_POOL[i]->name );
  }
}

void delete_item(int index)
{
}

void edit_item(item_type* itype)
{
}

void add_item()
{
}
