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

void set_symbol(item_type* itype);
void add_value (item_type* itype, const std::string &form_name);
void del_value (item_type* itype, const std::string &form_name);

void reference_item_editor(cuss::interface *i_editor, item_type *itype);
void    update_item_editor(cuss::interface *i_editor, item_type *itype);

int pick_attack();
int pick_damage();

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
  item_category itype_cat = itype->type();
  switch (itype_cat) {
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

// Set up data references
  reference_item_editor(&i_editor, itype);

  bool quit = false;
  while (!quit) {
    update_item_editor(&i_editor, itype);
    long ch = getch();
    if (!i_editor.handle_keypress(ch)) {
      cuss::element *selected_ele = i_editor.selected();
             if (ch == 'c' || ch == 'C') {
        set_symbol(itype);
      } else if ((ch == 'a' || ch == 'A') &&
                 selected_ele->type() == cuss::ELE_LIST) {
        add_value(itype, selected_ele->name);
      } else if ((ch == 'd' || ch == 'D') &&
                 selected_ele->type() == cuss::ELE_LIST) {
        del_value(itype, selected_ele->name);
      } else if ((ch == 'h' || ch == 'H') && itype_cat == ITEMCAT_LAUNCHER) {
        it_launcher* it = static_cast<it_launcher*>(itype);
        it->two_handed = !it->two_handed;
      } else if (ch == 't' || ch == 'T') {
        if (itype_cat == ITEMCAT_LAUNCHER) {
          it_launcher* it = static_cast<it_launcher*>(itype);
          int sel = pick_ammo();
          if (sel != 0) {
            it->ammo = sel;
          }
        } else if (itype_cat == ITEMCAT_MISSILE) {
          it_missile* it = static_cast<it_missile*>(itype);
          int sel = pick_ammo();
          if (sel != 0) {
            it->ammo = sel;
          }
        }
      } else if ((ch == 'm' || ch == 'M') && itype_cat == ITEMCAT_MISSILE) {
        it_missile* it = static_cast<it_missile*>(itype);
        int sel = pick_attack();
        if (sel != 0) {
          it->att_type = attack_type(sel);
        }
      } else if (ch == 's' || ch == 'S' && query_yn("Save and quit?")) {
        quit = true;
      }
    }
  }
}

void reference_item_editor(cuss::interface *i_editor, item_type *itype)
{
  if (!itype) {
    return;
  }
  i_editor->ref_data("entry_name",        &(itype->name));
  i_editor->ref_data("entry_description", &(itype->description));
  i_editor->ref_data("num_value",         &(itype->value));
  i_editor->ref_data("num_weight",        &(itype->weight));

  switch (itype->type()) {

    case ITEMCAT_WEAPON: {
      it_weapon* it = static_cast<it_weapon*>(itype);
      i_editor->ref_data("num_damage",   &(it->damage));
      i_editor->ref_data("num_accuracy", &(it->accuracy));
      i_editor->ref_data("num_hit_time", &(it->hit_time));
      i_editor->ref_data("num_block",    &(it->block));
      i_editor->ref_data("num_str_req",  &(it->str_req));
    } break;

    case ITEMCAT_LAUNCHER: {
      it_launcher* it = static_cast<it_launcher*>(itype);
      i_editor->ref_data("num_damage",    &(it->damage));
      i_editor->ref_data("num_accuracy",  &(it->accuracy));
      i_editor->ref_data("num_fire_time", &(it->fire_time));
      i_editor->ref_data("num_str_req",   &(it->str_req));
    } break;

    case ITEMCAT_MISSILE: {
      it_missile* it = static_cast<it_missile*>(itype);
      i_editor->ref_data("num_damage",    &(it->damage));
      i_editor->ref_data("num_accuracy",  &(it->accuracy));
      i_editor->ref_data("num_fire_time", &(it->fire_time));
      i_editor->ref_data("num_str_req",   &(it->str_req));
    } break;

    case ITEMCAT_ARMOR: {
      it_armor* it = static_cast<it_armor*>(itype);
      i_editor->ref_data("num_ac",       &(it->ac));
      i_editor->ref_data("list_covers",  &(it->parts_covered));
    } break;

    case ITEMCAT_FOOD: {
      it_food* it = static_cast<it_food*>(itype);
      i_editor->ref_data("num_nutrition", &(it->nutrition));
    } break;
// TODO: The rest of the item categories
  }
}

void update_item_editor(cuss::interface *i_editor, item_type *itype)
{
  if (!itype) {
    return;
  }
  std::stringstream symstring;
  symstring << "<c=" << color_tag_name(itype->symbol.fg) << ">" <<
               itype->symbol.symbol << "<c=/>";
  i_editor->set_data("text_symbol", symstring.str());
  switch (itype->type()) {

    case ITEMCAT_WEAPON: {
      it_weapon* it = static_cast<it_weapon*>(itype);
      std::vector<std::string> attack_names;
      for (int i = 0; i < ATT_MAX; i++) {
        if (it->attack_types[i]) {
          attack_names.push_back( get_attack_type_name( attack_type(i) ) );
        }
      }
      i_editor->set_data("list_attacks", attack_names);
      std::vector<std::string> damage_names;
      for (int i = 0; i < it->damage_types.size(); i++) {
        damage_names.push_back( get_damage_type_name( it->damage_types[i] ) );
      }
      i_editor->set_data("list_damages", damage_names);
    } break;

    case ITEMCAT_LAUNCHER: {
      it_launcher* it = static_cast<it_launcher*>(itype);
      std::vector<std::string> damage_names;
      for (int i = 0; i < it->damage_types.size(); i++) {
        damage_names.push_back( get_damage_type_name( it->damage_types[i] ) );
      }
      i_editor->set_data("list_damages", damage_names);
      i_editor->set_data("text_ammo_type", get_missile_category_name(it->ammo));
    } break;

    case ITEMCAT_MISSILE: {
      it_missile* it = static_cast<it_missile*>(itype);
      std::vector<std::string> damage_names;
      for (int i = 0; i < it->damage_types.size(); i++) {
        damage_names.push_back( get_damage_type_name( it->damage_types[i] ) );
      }
      i_editor->set_data("list_damages", damage_names);
      i_editor->set_data("text_ammo_type", get_missile_category_name(it->ammo));
      i_editor->set_data("text_attack_type",get_attack_type_name(it->att_type));
    } break;
// TODO: The rest of them!
  }
}

void set_symbol(item_type* itype)
{
  Window w_col(1, 1, 20, 6);
  w_col.outline();
  for (int i = 0; i < c_dkgray; i++) {
    w_col.putch(i + 1, 1, nc_color(i), c_black, '#');
    w_col.putch(i + 1, 3, nc_color(i + 8), c_black, '#');
  }
  w_col.putstr(1, 2, c_white, c_black, "12345678");
  w_col.putstr(1, 4, c_white, c_black, "abcdefgh");

  w_col.refresh();
  ch = getch();

  if (ch >= '1' && ch <= '8')
    itype->symbol.fg = nc_color(ch - '1');
  if (ch >= 'a' && ch <= 'h')
    itype->symbol.fg = nc_color(ch - 'a' + c_dkgray);
  itype->symbol.bg = c_black;
}

void add_value (item_type* itype, const std::string &form_name)
{
  switch (itype->type()) {
    case ITEMCAT_WEAPON: {
      it_weapon* it = static_cast<it_weapon*>(itype);
      if (form_name == "list_attacks") {
        int sel = pick_attack();
        if (sel != 0) {
          it->attack_types[i] = true;
        }
      } else if (form_name == "list_damages") {
        int sel = pick_damage();
        if (sel != 0) {
          it->damage_types.push_back( damage_type(sel) );
        }
      }
    } break;

    case ITEMCAT_LAUNCHER: {
      it_launcher* it = static_cast<it_launcher*>(itype);
      int sel = pick_damage();
      if (sel != 0) {
        it->damage_types.push_back( damage_type(sel) );
      }
    } break;

    case ITEMCAT_MISSLE: {
      it_missile* it = static_cast<it_missile*>(itype);
      int sel = pick_damage();
      if (sel != 0) {
        it->damage_types.push_back( damage_type(sel) );
      }
    } break;

    case ITEMCAT_ARMOR: {
      it_armor* it = static_cast<it_armor*>(itype);
      if (form_name == "list_covers") {
        std::vector<std::string> options;
        options.push_back("Cancel");
        for (int i = 0; i < BODY_PARTS_POOL.size(); i++) {
          options.push_back( BODY_PARTS_POOL[i].name );
        }
        int sel = menu_vec(options);
        if (sel != 0) {
          it->parts_covered.push_back( BODY_PARTS_POOL[sel - 1].name );
        }
      }
    } break;
  } // switch (itype->type())
}

void del_value (item_type* itype, const std::string &form_name)
{

}

int pick_attack()
{
  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 1 ; i < ATT_MAX; i++) {
    options.push_back( get_attack_type_name( attack_type(i) ) );
  }
  return menu_vec(options);
}

int pick_damage()
{
  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 1 ; i < DAMTYPE_MAX; i++) {
    options.push_back( get_damage_type_name( damage_type(i) ) );
  }
  return menu_vec(options);
}

int pick_ammo()
{
  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 1; i < MISSILECAT_MAX; i++) {
    options.push_back( get_missile_category_name( missile_category(i) ) );
  }
  return menu_vec(options);
}
