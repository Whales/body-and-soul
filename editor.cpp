#include "terrain.h"
#include "window.h"
#include "globals.h"
#include "interface.h"
#include <string>

enum edit_group {
EDIT_NULL = 0,
EDIT_STATS,
EDIT_PARTS,
EDIT_BODIES,
EDIT_MAX
};

void update_interface(cuss::interface& i_editor, edit_group editing,
                      std::vector<std::string> &items,
                      std::vector<std::string> &members);
edit_group pick_edit_group();

void new_item(edit_group editing);
void add_member (cuss::interface &i_editor, edit_group editing);
void delete_item(cuss::interface& i_editor, edit_group editing);

int main()
{
  init_display();
  init_environment();
  init_data();

  Window w_editor(0, 0, 80, 24);
  cuss::interface i_editor;
  if (!i_editor.load_from_file("cuss/i_body.cuss")) {
    debugmsg("Couldn't load cuss/i_body.cuss!");
    endwin();
    return 1;
  }

  bool done = false;
  edit_group editing = EDIT_STATS;
  std::vector<std::string> items, members;
  i_editor.ref_data("list_edit", items);
  i_editor.ref_data("list_members", members);
  update_interface(i_editor, editing, items, members);
  do {
    i_editor.draw(&w_editor);
    char ch = getch();
    bool did_something = true;

    if (ch == 'S' || ch == 's') {
      save_data();
      done = true;
    } else if (ch == 'Q' || ch == 'q') {
      if (popup_yn("Quit without saving?"))
        done = true;
    } else if (ch == 'E' || ch == 'e') {
      editing = pick_edit_group();
    } else if (ch == 'N' || ch == 'n') {
      new_item(editing);
    } else if (ch == 'A' || ch == 'a') {
      add_member(i_editor, editing);
    } else if (ch == 'D' || ch == 'd') {
      delete_item(i_editor, editing);
    } else
      did_something = false;

    did_something |= i_editor.handle_action(ch);

    if (!done && did_something)
      update_interface(i_editor, editing, items, members);

  } while (!done);

  endwin();
  return 0;
}

void update_interface(cuss::interface& i_editor, edit_group editing,
                      std::vector<std::string> &items,
                      std::vector<std::string> &members)
{
  items.clear();
  members.clear();
  int selected = i_editor.get_int("list_edit"), selectables;
  bool found_item = false;

  switch (editing) {

    case EDIT_STATS:
      selectables = BODY_STATS_POOL.size();
      for (int i = 0; i < BODY_STATS_POOL.size(); i++) {
        body_stat *bp = &(BODY_STATS_POOL[i]);
        members.push_back(bp->name);
      }
      break;
          

    case EDIT_PARTS:
      selectables = BODY_PARTS_POOL.size();
      for (int i = 0; i < BODY_PARTS_POOL.size(); i++) {
        body_part *bp = &(BODY_PARTS_POOL[i]);
        members.push_back(bp->name);
      }
      break;

    case EDIT_BODIES:
      selectables = BODIES_POOL.size();
      for (int i = 0; i < BODIES_POOL.size(); i++) {
        body *bp = &(BODIES_POOL[i]);
        members.push_back(bp->name);
      }
      break;
  }

  if (selected > 0 && selected < selectables) {
    switch (editing) {

      case EDIT_STATS:
        body *bp = &(BODY_STATS_POOL[selected]);
        for (int i = 0; i < bp->chals.size(); i++)
          members.push_back( challenge_name(bp->chals[i]) );
        break;

      case EDIT_PARTS:
        body_part *bp = &(BODY_PARTS_POOL[selected]);
        for (int i = 0; i < bp->stats.size(); i++)
          members.push_back( bp->stats[i].name );
        break;

      case EDIT_BODIES:
        body *bp = &(BODIES_POOL[selected]);
        for (int i = 0; i < bp->body_parts.size(); i++)
          members.push_back( bp->body_parts[i].name );
        break;
    }
  }
}

edit_group pick_edit_group()
{
  switch (popup_menu("Edit:", "Bodies", "Body Parts", "Stats", NULL)) {
    case 1: return EDIT_BODIES;
    case 2: return EDIT_PARTS;
    case 3: return EDIT_STATS;
    default: return EDIT_BODIES;
  }
  return EDIT_BODIES;
}

void new_item(edit_group editing)
{
  switch (editing) {

    case EDIT_BODIES: {
      body tmp;
      BODIES_POOL.push_back(tmp);
    } break;

    case EDIT_PARTS: {
      body_part tmp;
      BODY_PARTS_POOL.push_back(tmp);
    } break;

    case EDIT_STATS: {
      body_stat tmp;
      BODY_STATS_POOL.push_back(tmp);
    } break;

  }
}

void add_member(cuss::interface &i_editor, edit_group editing)
{
  std::string edited = i_editor.get_str("list_edit");
  bool done = false;
  switch (editing) {

    case EDIT_BODIES:
      for (int i = 0; !done && i < BODIES_POOL.size; i++) {
        if (BODIES_POOL[i].name == edited) {
          std::vector<std::string> part_names;
          for (int j = 0; j < BODY_PARTS_POOL.size(); j++)
            part_names.push_back( BODY_PARTS_POOL[i].name );
          part_names.push_back("Cancel");
          int bp_picked = menu_vec(part_names);
          if (bp_picked != part_names.size() - 1)
            BODIES_POOL[i].body_parts.push_back( BODY_PARTS_POOL[bp_picked] );
          done = true;
        }
      }
      break;

    case EDIT_PARTS:
      for (int i = 0; !done && i < BODY_PARTS_POOL.size; i++) {
        if (BODY_PARTS_POOL[i].name == edited) {
          std::vector<std::string> part_names;
          for (int j = 0; j < BODY_STATS_POOL.size(); j++)
            part_names.push_back( BODY_STATS_POOL[i].name );
          part_names.push_back("Cancel");
          int bp_picked = menu_vec(part_names);
          if (bp_picked != part_names.size() - 1)
            BODY_PARTS_POOL[i].stats.push_back(BODY_STATS_POOL[bp_picked]);
          done = true;
        }
      }
      break;
 
    case EDIT_STATS:
      for (int i = 0; !done && i < BODY_PARTS_POOL.size; i++) {
        if (BODY_STATS_POOL[i].name == edited) {
          std::vector<std::string> part_names;
          for (int j = 1; j < CHAL_MAX; j++)
            part_names.push_back( challenge_name(j) );
          part_names.push_back("Cancel");
          int bp_picked = menu_vec(part_names);
          if (bp_picked != part_names.size() - 1)
            BODY_STATS_POOL[i].chals.push_back( challenge_type(bp_picked + 1) );
          done = true;
        }
      }
      break;
 
    default:
      debugmsg("What the heck?");
      break;
  }
}
