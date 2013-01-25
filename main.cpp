#include <string>
#include <sstream>
#include "globals.h"
#include "terrain.h"
#include "interface.h"
#include "window.h"

std::vector<std::string> get_names(bool editing_parts);
std::vector<std::string> get_members(body* _body, body_part* _body_part);

int main()
{
  init_environment();
  init_data();
  init_display();

  cuss::interface i_editor;
  Window w_editor(0, 0, 80, 24);
  if (!i_editor.load_from_file("cuss/i_body.cuss")) {
    debugmsg("Couldn't load cuss/i_body.cuss!");
    end_display();
    return 1;
  }

  bool editing_parts = true;
  std::vector<std::string> names;
  std::vector<std::string> members;
  i_editor.ref_data("list_names", &names);
  i_editor.ref_data("list_members", &members);
  i_editor.set_data("text_editing", "Body Parts: (hit / to edit bodies)");

  bool quit = false;
  do {
    int active_index = i_editor.get_int("list_names");
    cuss::element* selected = i_editor.selected();

    body* active_body = NULL;
    body_part* active_part = NULL;

    if (editing_parts && active_index >= 0 &&
        active_index < BODY_PARTS_POOL.size()) {
      active_part = &(BODY_PARTS_POOL[active_index]);
    } else if (!editing_parts && active_index >= 0 &&
               active_index < BODIES_POOL.size()) {
      active_body = &(BODIES_POOL[active_index]);
    }

    names = get_names(editing_parts);
    members = get_members(active_body, active_part);
    i_editor.draw(&w_editor);
    long ch = getch();
    
  } while (!quit);

  save_data();
  end_display();
  return 0;
}
