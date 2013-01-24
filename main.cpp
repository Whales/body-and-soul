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

  type_names = get_names();

// Main loop
  bool quit = false;
  do {
    cuss::element* selected = i_editor.selected();
    int ter_num = i_editor.get_int("list_types");
    terrain_type* current_ter;
    if (ter_num < TERRAIN_POOL.size()) {
      current_ter = TERRAIN_POOL[ i_editor.get_int("list_types") ];
      flags = get_flag_names(current_ter);
      transformations = get_transformation_names(current_ter);
    } else {
      current_ter = NULL;
    }
    i_editor.draw(&w_editor);
    long ch = getch();
    if (ch == KEY_ESC) {
      quit = true;
    } else if (ch == 'a' || ch == 'A') {
      if (selected->name == "list_flags") {
        add_flag(current_ter);
      } else if (selected->name == "list_transformations") {
        add_transformation(current_ter);
      }
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
  for (int i = 0; i < TER_MAX; i++) {
    if (i >= TERRAIN_POOL.size() || TERRAIN_POOL[i] == NULL) {
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
    if (type->transformations[i] != TER_NULL) {
      std::stringstream flagdata;
      flagdata << get_transformation_name( transform_type(i) ) << " => " <<
                  TERRAIN_POOL[ type->transformations[i] ]->name;
    }
  }
}

void add_flag(terrain_type* type)
{
}

void add_transformation(terrain_type* type)
{
}
