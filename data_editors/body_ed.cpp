#include <string>
#include <sstream>
#include "globals.h"
#include "terrain.h"
#include "interface.h"
#include "window.h"

std::vector<std::string> get_names(bool editing_parts);
std::vector<std::string> get_members(body* _body, body_part* _body_part);

void set_symbol(body* _body);
void add_ability(body_part* _body_part);
void add_part(body* _body);

void update_numbers(cuss::interface &i_editor, body_part* _part);

void delete_part(int index);
void delete_body(int index);
void delete_member(body* _body, body_part* _body_part, int index);

int main()
{
  init_display();
  init_environment();
  init_data();

  cuss::interface i_editor;
  Window w_editor(0, 0, 80, 24);
  if (!i_editor.load_from_file("cuss/i_body_ed.cuss")) {
    debugmsg("Couldn't load cuss/i_body_ed.cuss!");
    end_display();
    return 1;
  }

  bool editing_parts = true;
  std::vector<std::string> names;
  std::vector<std::string> members;
  i_editor.ref_data("list_names", &names);
  i_editor.ref_data("list_members", &members);
  i_editor.select("list_names");

  bool quit = false;
  do {
    int active_index = i_editor.get_int("list_names");
    cuss::element* selected = i_editor.selected();

    body* active_body = NULL;
    body_part* active_part = NULL;
    std::string* active_name = NULL;

    if (editing_parts) {
      i_editor.set_data("text_editing", "Parts: (hit / to edit bodies)");
      i_editor.set_data("text_members_name", "Abilities:");
      if (active_index >= 0 && active_index < BODY_PARTS_POOL.size()) {
        active_part = &(BODY_PARTS_POOL[active_index]);
        active_name = &(active_part->name);
        i_editor.ref_data("entry_name", active_name);
      }
    } else {
      i_editor.set_data("text_editing", "Bodies: (hit / to edit parts)");
      i_editor.set_data("text_members_name", "Parts:");
      if (active_index >= 0 && active_index < BODIES_POOL.size()) {
        active_body = &(BODIES_POOL[active_index]);
        active_name = &(active_body->name);
        i_editor.ref_data("entry_name", active_name);
      }
    }

    update_numbers(i_editor, active_part);
    names   = get_names(editing_parts);
    members = get_members(active_body, active_part);
    if (active_index < 0)
      i_editor.set_data("list_names", 1);
    if (active_index >= names.size())
      i_editor.set_data("list_names", names.size() - 1);

    i_editor.draw(&w_editor);
    long ch = getch();

    if (selected->name == "entry_name" && active_name &&
        ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == ' ')) {
      (*active_name) += ch;

    } else if (selected->name == "entry_name" && active_name &&
               is_backspace(ch) && !active_name->empty()) {
      (*active_name) = active_name->substr(0, active_name->length() - 1);

    } else if (selected->type() == cuss::ELE_NUMBER && active_part &&
               (is_backspace(ch) || (ch >= '0' && ch <= '9'))) {
      int *editint = NULL;
      if (selected->name == "num_hp") {
        editint = &(active_part->maxhp);
      } else if (selected->name == "num_wgt") {
        editint = &(active_part->weight);
      } else if (selected->name == "num_str") {
        editint = &(active_part->strength);
      } else if (selected->name == "num_dex") {
        editint = &(active_part->dexterity);
      } else if (selected->name == "num_per") {
        editint = &(active_part->perception);
      } else if (selected->name == "num_spd") {
        editint = &(active_part->speed);
      }
      if (editint) {
        if (ch >= '0' && ch <= '9') {
          *editint *= 10;
          *editint += (ch - '0');
        } else if (is_backspace(ch) && editint > 0) {
          *editint /= 10;
        }
      }

    } else if (ch == '/') {
      editing_parts = !editing_parts;

    } else if (ch == 's' || ch == 'S') {
      quit = true;

    } else if (ch == 'c' || ch == 'C') {
      set_symbol(active_body);

    } else if (ch == 'a' || ch == 'A') {
      if (selected->name == "list_names") {
        if (editing_parts) {
          body_part newpart;
          newpart.name = string_input_popup("Name:");
          BODY_PARTS_POOL.push_back(newpart);
        } else {
          body newbod;
          newbod.name = string_input_popup("Name:");
          set_symbol(&newbod);
          BODIES_POOL.push_back(newbod);
        }
      } else if (selected->name == "list_members") {
        if (editing_parts) {
          add_ability(active_part);
        } else {
          add_part(active_body);
        }
      }

    } else if (ch == 'd' || ch == 'D') {
      if (selected->name == "list_names") {
        if (editing_parts) {
          delete_part(active_index);
        } else {
          delete_body(active_index);
        }
      } else if (selected->name == "list_members") {
        int index = i_editor.get_int("list_members");
        delete_member(active_body, active_part, index);
      }

    } else {
      i_editor.handle_action(ch);
    }
    
  } while (!quit);

  save_data();
  end_display();
  return 0;
}

std::vector<std::string> get_names(bool editing_parts)
{
  std::vector<std::string> ret;
  if (editing_parts) {
    for (int i = 0; i < BODY_PARTS_POOL.size(); i++) {
      ret.push_back( BODY_PARTS_POOL[i].name );
    }
  } else {
    for (int i = 0; i < BODIES_POOL.size(); i++) {
      body* cur = &(BODIES_POOL[i]);
      std::stringstream typedata;
      typedata << "<c=" << color_tag_name( cur->symbol.fg ) << "," <<
                  color_tag_name( cur->symbol.bg ) << ">" <<
                  char(cur->symbol.symbol) << "<c=/> " << cur->name;
      ret.push_back( typedata.str() );
    }
  }

  return ret;
}

std::vector<std::string> get_members(body* _body, body_part* _body_part)
{
  std::vector<std::string> ret;
  if (_body) {
    for (std::vector<body_part>::iterator it = _body->body_parts.begin();
         it != _body->body_parts.end();
         it++) {
      ret.push_back(it->name);
    }
  } else if (_body_part) {
    for (std::vector<body_ability>::iterator it = _body_part->abilities.begin();
         it != _body_part->abilities.end();
         it++) {
      std::stringstream text;
      text << body_ability_name(it->type) << ": " << it->level;
      ret.push_back( text.str() );
    }
  }

  return ret;
}

void set_symbol(body* _body)
{
  if (!_body)
    return;

  long ch = popup_getkey("Enter symbol for %s.", _body->name.c_str());
  if (ch == KEY_ESC)
    return;

  _body->symbol.symbol = ch;

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

  if (ch >= '1' && ch <= '8') {
    _body->symbol.fg = nc_color(ch - '1');
  }
  if (ch >= 'a' && ch <= 'h') {
    _body->symbol.fg = nc_color(ch - 'a' + c_dkgray);
  }
  _body->symbol.bg = c_black;
}

void add_ability(body_part* _body_part)
{
  if (!_body_part)
    return;

  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 1; i < BODY_ABILITY_MAX; i++) {
    options.push_back( body_ability_name( body_ability_id(i) ) );
  }

  int choice = menu_vec("Ability:", options);
  if (choice == 0)
    return;
  int level = int_input_popup("Level:");
  if (level == 0)
    return;

  _body_part->abilities.push_back( body_ability( body_ability_id(choice),
                                                 level                   ) );
}

void add_part(body* _body)
{
  if (!_body)
    return;

  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 0; i < BODY_PARTS_POOL.size(); i++) {
    options.push_back(BODY_PARTS_POOL[i].name);
  }
  int choice = menu_vec("Part:", options) - 1;
  if (choice == -1)
    return;
  _body->body_parts.push_back( BODY_PARTS_POOL[choice] );
}

void update_numbers(cuss::interface &i_editor, body_part* _part)
{
  if (!_part) {
    i_editor.self_reference("num_hp");
    i_editor.self_reference("num_wgt");
    i_editor.self_reference("num_str");
    i_editor.self_reference("num_dex");
    i_editor.self_reference("num_per");
    i_editor.self_reference("num_spd");
    i_editor.set_selectable("num_hp",  false);
    i_editor.set_selectable("num_wgt", false);
    i_editor.set_selectable("num_str", false);
    i_editor.set_selectable("num_dex", false);
    i_editor.set_selectable("num_per", false);
    i_editor.set_selectable("num_spd", false);
    return;
  }

  i_editor.set_selectable("num_hp",  true);
  i_editor.set_selectable("num_wgt", true);
  i_editor.set_selectable("num_str", true);
  i_editor.set_selectable("num_dex", true);
  i_editor.set_selectable("num_per", true);
  i_editor.set_selectable("num_spd", true);

  i_editor.ref_data("num_hp",  &(_part->maxhp));
  i_editor.ref_data("num_wgt", &(_part->weight));
  i_editor.ref_data("num_str", &(_part->strength));
  i_editor.ref_data("num_dex", &(_part->dexterity));
  i_editor.ref_data("num_per", &(_part->perception));
  i_editor.ref_data("num_spd", &(_part->speed));
}

void delete_part(int index)
{
  if (index < 0 || index >= BODY_PARTS_POOL.size())
    return;

  BODY_PARTS_POOL.erase( BODY_PARTS_POOL.begin() + index );
}

void delete_body(int index)
{
  if (index < 0 || index >= BODIES_POOL.size())
    return;

  BODIES_POOL.erase( BODIES_POOL.begin() + index );
}
  
void delete_member(body* _body, body_part* _body_part, int index)
{
  if (_body) {
    if (index < 0 || index >= _body->body_parts.size())
      return;
    _body->body_parts.erase( _body->body_parts.begin() + index);
  } else if (_body_part) {
    if (index < 0 || index >= _body_part->abilities.size())
      return;
    _body_part->abilities.erase( _body_part->abilities.begin() + index);
  }
}
