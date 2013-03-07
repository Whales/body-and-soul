#include <string>
#include <sstream>
#include "globals.h"
#include "itemtype.h"
#include "interface.h"
#include "window.h"

std::vector<std::string> get_item_names();
void delete_item(int index);
void add_item();
void edit_item(item_type* itype);
void update_item_editor(cuss::interface *i_editor, item_type *itype);

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

  i_list.set_data("list_items", get_item_names());

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
    } else {
      i_list.handle_action(ch);
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
  if (index < 0 || index >= ITEMS_POOL.size()) {
    return;
  }
  ITEMS_POOL.erase( ITEMS_POOL.begin() + index);
}

void add_item()
{
  std::vector<std::string> menu_options;
  menu_options.push_back("Cancel");
  for (int i = 1; i < ITEMCAT_MAX; i++) {
    menu_options.push_back( get_item_category_name( item_category(i) ) );
  }
  int sel = menu_vec( menu_options );
  if (sel == 0) {
    return;
  }
  item_type* newitype = new_item_type( item_category(sel) );
  ITEMS_POOL.push_back(newitype);
  edit_item(newitype);
}
  
void edit_item(item_type* itype)
{
  if (!itype) {
    return;
  }
  Window w_editor(0, 0, 80, 25);
  cuss::interface i_editor;
  std::string fname = "cuss/";
  switch (itype->type()) {
    case ITEMCAT_NULL: return;
    case ITEMCAT_WEAPON:      fname += "i_edit_weapon.cuss";
    case ITEMCAT_LAUNCHER:    fname += "i_edit_launcher.cuss";
    case ITEMCAT_MISSILE:     fname += "i_edit_missile.cuss";
    case ITEMCAT_ARMOR:       fname += "i_edit_armor.cuss";
    case ITEMCAT_FOOD:        fname += "i_edit_food.cuss";
    case ITEMCAT_POTION:      fname += "i_edit_potion.cuss";
    case ITEMCAT_INSTRUMENT:  fname += "i_edit_instrument.cuss";
    case ITEMCAT_CRYSTAL:     fname += "i_edit_crystal.cuss";
    default: return;
  }

  if (!i_editor.load_from_file(fname)) {
    debugmsg("Can't load %s", fname.c_str());
    return;
  }

  update_item_editor(&i_editor, itype);
  bool quit = false;
  while (!quit) {
}

void update_item_editor(cuss::interface *i_editor, item_type *itype)
{
  i_editor->ref_data("entry_name", &(itype->name));
  i_editor->ref_data("entry_description", &(itype->description));
  switch (itype->type()) {
    case ITEMCAT_WEAPON: {
      it_weapon* weap = static_cast<it_weapon*>(itype);
      i_editor->ref_data("num_damage", &(weap->damage));
      i_editor->ref_data("num_accuracy", &(weap->accuracy));
      i_editor->ref_data("num_hit_time", &(weap->hit_time));
      i_editor->ref_data("num_block", &(weap->block));
      i_editor->ref_data("num_str_req", &(weap->str_req));
