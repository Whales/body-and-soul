#include <string>
#include <sstream>
#include "globals.h"
#include "terrain.h"
#include "interface.h"
#include "window.h"

std::vector<std::string> get_names();
std::vector<std::string> get_flag_names(terrain_type *type);
std::vector<std::string> get_transformation_names(terrain_type *type);

void add_flag(terrain_type *type);
void add_transformation(terrain_type *type);

void remove_flag(terrain_type *type, int index);
void remove_transformation(terrain_type *type, int index);

void set_symbol(terrain_type *type);

int main()
{
  init_environment();
  init_data();
  init_display();

  cuss::interface i_editor;
  Window w_editor(0, 0, 80, 24);
  if (!i_editor.load_from_file("cuss/i_terrain.cuss")) {
    debugmsg("Couldn't load cuss/i_terrain.cuss!");
    end_display();
    return 1;
  }

  std::vector<std::string> type_names;
  std::vector<std::string> flags;
  std::vector<std::string> transformations;
  i_editor.ref_data("list_types", &type_names);
  i_editor.ref_data("list_flags", &flags);
  i_editor.ref_data("list_transformations", &transformations);
  i_editor.select("list_types");

  for (int i = 0; i < TER_MAX; i++) {
    if (i >= TERRAIN_POOL.size() || TERRAIN_POOL[i] == NULL) {
      TERRAIN_POOL.push_back( new terrain_type );
      TERRAIN_POOL[i]->name = default_terrain_name( terrain_id(i) );
    }
  }


// Main loop
  bool quit = false;
  do {
    type_names = get_names();
    cuss::element* selected = i_editor.selected();
    int ter_num = i_editor.get_int("list_types");
    terrain_type* current_ter;
    if (ter_num < TERRAIN_POOL.size()) {
      current_ter = TERRAIN_POOL[ i_editor.get_int("list_types") ];
      flags = get_flag_names(current_ter);
      transformations = get_transformation_names(current_ter);
      i_editor.ref_data("num_move", &(current_ter->move_cost));
      i_editor.ref_data("num_sight", &(current_ter->sight_cost));
      i_editor.ref_data("text_name", &(current_ter->name));
    } else {
      current_ter = NULL;
    }
    i_editor.draw(&w_editor);
    long ch = getch();
    if (selected->name == "text_name" &&
        ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == ' ')) {
      current_ter->name += ch;
    } else if (selected->name == "text_name" && is_backspace(ch) &&
               !current_ter->name.empty()) {
      current_ter->name =
        current_ter->name.substr(0, current_ter->name.length() - 1);
    } else if (ch == 's' || ch == 'S') {
      quit = true;
    } else if (ch == 'c' || ch == 'C') {
      set_symbol(current_ter);
    } else if (ch == 'a' || ch == 'A') {
      if (selected->name == "list_types") {
        terrain_type *tmp = new terrain_type;
        tmp->move_cost = 10;
        tmp->name = string_input_popup("Name:");
        set_symbol(tmp);
        TERRAIN_POOL.push_back(tmp);
        i_editor.set_data("list_types", 999);
      } else if (selected->name == "list_flags") {
        add_flag(current_ter);
        i_editor.set_data("list_flags", 0);
      } else if (selected->name == "list_transformations") {
        add_transformation(current_ter);
        i_editor.set_data("list_transformations", 0);
      }
    } else if (ch == 'd' || ch == 'D') {
      if (selected->name == "list_flags") {
        int index = i_editor.get_int("list_flags");
        if (index < flags.size()) {
          remove_flag(current_ter, index);
          i_editor.set_data("list_flags", 0);
        }
      } else if (selected->name == "list_transformations") {
        int index = i_editor.get_int("list_transformations");
        if (index < transformations.size()) {
          remove_transformation(current_ter, index);
          i_editor.set_data("list_transformations", 0);
        }
      }
    } else if (ch == '?') {
      debugmsg("%d of %d", i_editor.get_int("list_transformations"),
                           transformations.size());
      debugmsg("%d (\\n = %d", TERRAIN_POOL[1]->name[0], '\n');
    } else {
      i_editor.handle_action(ch);
    }
  } while (!quit);

  save_data();
  end_display();
  return 0;
}

std::vector<std::string> get_names()
{
  std::vector<std::string> type_names;
  for (int i = 0; i < TERRAIN_POOL.size(); i++) {
    if (TERRAIN_POOL[i] == NULL) {
      type_names.push_back("Bug in editor");
    } else {
      terrain_type* cur = TERRAIN_POOL[i];
      std::stringstream typedata;
      typedata << "<c=" << color_tag_name( cur->symbol.fg ) << "," <<
                  color_tag_name( cur->symbol.bg ) << ">" <<
                  char(cur->symbol.symbol) << "<c=/> " << cur->name;
      type_names.push_back( typedata.str() );
    }
  }
  return type_names;
}

std::vector<std::string> get_flag_names(terrain_type *type)
{
  std::vector<std::string> ret;
  if (!type)
    return ret;

  for (int i = 1; i < type->flags.size(); i++) {
    if (type->flags[i]) {
      ret.push_back( get_flag_name( terrain_flag(i) ) );
    }
  }
  return ret;
}

std::vector<std::string> get_transformation_names(terrain_type *type)
{
  std::vector<std::string> ret;
  if (!type)
    return ret;

  for (int i = 1; i < type->transformations.size(); i++) {
    if (type->transformations[i].result != TER_NULL) {
      std::stringstream flagdata;
      flagdata << type->transformations[i].resistance << "x " <<
                  get_transformation_name( transform_type(i) ) << " => " <<
                  TERRAIN_POOL[ type->transformations[i].result ]->name;
      ret.push_back(flagdata.str());
    }
  }

  return ret;
}

void add_flag(terrain_type* type)
{
  std::vector<std::string> flag_names;
  flag_names.push_back("Cancel");
  std::vector<int> flag_indices;
  for (int i = 1; i < TF_MAX; i++) {
    if (!type->flags[i]) {
      flag_names.push_back( get_flag_name( terrain_flag(i) ) );
      flag_indices.push_back(i);
    }
  }

  if (flag_names.size() == 1) {
    popup("All flags are turned on.");
    return;
  }

  int picked = menu_vec("Apply flag:", flag_names);
  if (picked != 0) {
    picked--;
    type->flags[ flag_indices[picked] ] = true;
  }
}

void add_transformation(terrain_type* type)
{
  std::vector<std::string> transform_names;
  transform_names.push_back("Cancel");
  for (int i = 1; i < TRANS_MAX; i++) {
    transform_names.push_back( get_transformation_name( transform_type(i) ) );
  }

  int picked = menu_vec("Transformation type:", transform_names);
  if (picked == 0)
    return;

  std::vector<std::string> terrain_names;
  terrain_names.push_back("Cancel");
  for (int i = 1; i < TERRAIN_POOL.size(); i++) {
    terrain_names.push_back(TERRAIN_POOL[i]->name);
  }
  int result = menu_vec("Transform to:", terrain_names);
  if (result == 0)
    return;

  int resistance = menu(
"Amount required:",
"Miniscule",    //  1
"Small",        //  4
"Moderate",     //  9
"Large",        // 16
"Huge",         // 25
"Incredible",   // 36
NULL);
  resistance++;
  resistance *= resistance;

  type->transformations[picked].result = result;
  type->transformations[picked].resistance = resistance;
}

void remove_flag(terrain_type *type, int index)
{
  for (int i = 0; i < type->flags.size(); i++) {
    if (type->flags[i]) {
      if (index == 0) {
        type->flags[i] = false;
        return;
      } else {
        index--;
      }
    }
  }
}

void remove_transformation(terrain_type *type, int index)
{
  for (int i = 0; i < type->transformations.size(); i++) {
    if (type->transformations[i].result != TER_NULL) {
      if (index == 0) {
        type->transformations[i].result = TER_NULL;
        return;
      } else {
        index--;
      }
    }
  }
}

void set_symbol(terrain_type *type)
{
  if (!type)
    return;

  long ch = popup_getkey("Enter symbol for %s.", type->name.c_str());
  if (ch == KEY_ESC)
    return;

  type->symbol.symbol = ch;

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
    type->symbol.fg = nc_color(ch - '1');
  if (ch >= 'a' && ch <= 'h')
    type->symbol.fg = nc_color(ch - 'a' + c_dkgray);
  type->symbol.bg = c_black;
}
