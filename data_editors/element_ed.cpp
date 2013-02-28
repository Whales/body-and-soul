#include <string>
#include <vector>
#include <sstream>
#include "element.h"
#include "interface.h"
#include "window.h"
#include "globals.h"
#include "terrain.h"

std::vector<std::string> get_names();
std::vector<std::string> get_tra_names(element* ele);
std::vector<std::string> get_dmg_names(element* ele);

void add_transformation(element* ele);
void add_damagetype(element* ele);

void change_color(element* ele);

void delete_element(int index);
void delete_transformation(element* ele, int index);
void delete_damagetype(element* ele, int index);


int main()
{
  init_environment();
  init_data();
  init_display();

  cuss::interface i_editor;
  Window w_editor(0, 0, 80, 24);
  if (!i_editor.load_from_file("cuss/i_element_ed.cuss")) {
    debugmsg("Can't load cuss/i_element_ed.cuss");
    end_display();
    return 1;
  }
  std::vector<std::string> element_names;
  std::vector<std::string> transform_names;
  std::vector<std::string> damage_names;
  i_editor.ref_data("list_elements", &element_names);
  i_editor.ref_data("list_transformations", &transform_names);
  i_editor.ref_data("list_damagetypes", &damage_names);
  i_editor.select("list_elements");

  bool quit = false;
  while (!quit) {
    element_names = get_names();
    cuss::element* selected = i_editor.selected();
    int ele_num = i_editor.get_int("list_elements");
    element* cur_element = NULL;
    if (ele_num < ELEMENTS_POOL.size()) {
      cur_element = &(ELEMENTS_POOL[ele_num]);
      std::stringstream color_data;
      color_data << "<c=" << color_tag_name(cur_element->color) <<
                    ">************<c=/>";
      i_editor.set_data("text_color", color_data.str());
      i_editor.ref_data("entry_name", &(cur_element->name));
    }
    transform_names = get_tra_names(cur_element);
    damage_names    = get_dmg_names(cur_element);
    i_editor.draw(&w_editor);

    long ch = getch();
    if (selected->name == "entry_name" &&
        ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == ' ')) {
      cur_element->name += ch;

    } else if (selected->name == "entry_name" && is_backspace(ch) &&
               !cur_element->name.empty()) {
      cur_element->name =
       cur_element->name.substr( 0, cur_element->name.length() - 1);

    } else if (ch == 's' || ch == 'S') {
      quit = true;

    } else if (ch == 'c' || ch == 'C') {
      change_color(cur_element);

    } else if (ch == 'a' || ch == 'A') {
// Where are we adding an element?
      if (selected->name == "list_elements") {
        element tmp;
        tmp.name = string_input_popup("Name:");
        ELEMENTS_POOL.push_back(tmp);
        i_editor.set_data("list_elements", 999);
      } else if (selected->name == "list_transformations") {
        add_transformation(cur_element);
        i_editor.set_data("list_transformations", 0);
      } else if (selected->name == "list_damagetypes") {
        add_damagetype(cur_element);
        i_editor.set_data("list_damagetypes", 0);
      }
    } else if (ch == 'd' || ch == 'D') {
// Where are we deleting an element?
      if (selected->name == "list_elements" && cur_element) {
        delete_element(ele_num);
        i_editor.set_data("list_elements", 0);
      } else if (selected->name == "list_transformations") {
        int index = i_editor.get_int("list_transformations");
        if (cur_element && index >= 0 &&
            index < cur_element->transformations.size()) {
          delete_transformation(cur_element, index);
          i_editor.set_data("list_transformations", 0);
        }
      } else if (selected->name == "list_damagetypes") {
        int index = i_editor.get_int("list_damagetypes");
        if (cur_element && index >= 0 &&
            index < cur_element->damages.size()) {
          delete_damagetype(cur_element, index);
          i_editor.set_data("list_damagetypes", 0);
        }
      }
    } else {
      i_editor.handle_action(ch);
    }
  } // while (!quit)

  save_data();
  end_display();
  return 0;
}

std::vector<std::string> get_names()
{
  std::vector<std::string> ret;
  for (int i = 0; i < ELEMENTS_POOL.size(); i++) {
    ret.push_back( ELEMENTS_POOL[i].name );
  }
  return ret;
}
  
std::vector<std::string> get_tra_names(element* ele)
{
  std::vector<std::string> ret;
  if (!ele) {
    return ret;
  }
  for (std::vector<transform_type>::iterator it =
       ele->transformations.begin(); it != ele->transformations.end(); it++) {
    ret.push_back( get_transformation_name(*it) );
  }
  return ret;
}
  
std::vector<std::string> get_dmg_names(element* ele)
{
  std::vector<std::string> ret;
  if (!ele) {
    return ret;
  }
  for (std::vector<damage_type>::iterator it = ele->damages.begin();
       it != ele->damages.end(); it++) {
    ret.push_back( get_damage_type_name(*it) );
  }
}

void add_transformation(element* ele)
{
  if (!ele) {
    return;
  }
  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 1; i < TRANS_MAX; i++) {
    options.push_back( get_transformation_name( transform_type(i) ) );
  }

  int sel = menu_vec("Transformation:", options);
  if (sel == 0) {
    return;
  }
  ele->transformations.push_back( transform_type(sel) );
}

void add_damagetype(element* ele)
{
  if (!ele) {
    return;
  }
  std::vector<std::string> options;
  options.push_back("Cancel");
  for (int i = 1; i < DAMTYPE_MAX; i++) {
    options.push_back( get_damage_type_name( damage_type(i) ) );
  }

  int sel = menu_vec("Damage Type:", options);
  if (sel == 0) {
    return;
  }
  ele->damages.push_back( damage_type(sel) );
}

void change_color(element* ele)
{
  if (!ele) {
    return;
  }
  Window w_col(1, 1, 20, 6);
  w_col.outline();
  for (int i = 0; i < c_dkgray; i++) {
    w_col.putch(i + 1, 1, nc_color(i), c_black, '#');
    w_col.putch(i + 1, 3, nc_color(i + 8), c_black, '#');
  }
  w_col.putstr(1, 2, c_white, c_black, "12345678");
  w_col.putstr(1, 4, c_white, c_black, "abcdefgh");

  w_col.refresh();
  long ch = getch();

  if (ch >= '1' && ch <= '8')
    ele->color = nc_color(ch - '1');
  if (ch >= 'a' && ch <= 'h')
    ele->color = nc_color(ch - 'a' + c_dkgray);
}
 
void delete_element(int index)
{
  if (index < 0 || index >= ELEMENTS_POOL.size()) {
    return;
  }
  ELEMENTS_POOL.erase( ELEMENTS_POOL.begin() + index );
}

void delete_transformation(element* ele, int index)
{
  if (!ele || index < 0 || index >= ele->transformations.size()) {
    return;
  }
  ele->transformations.erase( ele->transformations.begin() + index );
}

void delete_damagetype(element* ele, int index)
{
  if (!ele || index < 0 || index >= ele->damages.size()) {
    return;
  }
  ele->damages.erase( ele->damages.begin() + index );
}
