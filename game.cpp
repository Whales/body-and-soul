#include "game.h"
#include "rng.h"
#include "globals.h"

void game::init()
{
  w_body.init    ( 0,  0, 27, 24);
  w_terrain.init (27,  0, 25, 17);
  w_soul.init    (52,  0, 28, 17);
  w_messages.init(27, 17, 53,  7);

  int_body.load_from_file("cuss/i_body.cuss");
  int_soul.load_from_file("cuss/i_soul.cuss");
  int_messages.load_from_file("cuss/i_messages.cuss");

  curMap.generate(MAP_WOODS);
  player.pos = point(80, 80);
  player.my_body = BODIES_POOL[ lookup_body_id("human") ];
  player.my_body.init();

  messages_to_print = 0;
}

bool game::do_turn()
{
  draw();
  switch (getch()) {
    case 'j': pl_move( 0,  1); break;
    case 'k': pl_move( 0, -1); break;
    case 'h': pl_move(-1,  0); break;
    case 'l': pl_move( 1,  0); break;
    case 'y': pl_move(-1, -1); break;
    case 'u': pl_move( 1, -1); break;
    case 'b': pl_move(-1,  1); break;
    case 'n': pl_move( 1,  1); break;
<<<<<<< HEAD:game.cpp
    case 'i': {
      item tmp;
      tmp.set_type( rng(0, ITEMS_POOL.size() - 1) );
      curMap.add_item( player.pos.x, player.pos.y, tmp);
    } break;
=======
    case 'i':
      //item
>>>>>>> 2efa22bff7c63dbbe52deca64bb4645d71f7a238:game.cpp
    case 'q': return false;
  }
  curMap.process_transformations();
  return true;
}

void game::draw()
{
// TODO: Replace 10 with player's sight distance
  //debugmsg("%d/%d", player.pos.x, player.pos.y);
  //debugmsg("%d:%d", w_terrain.sizex(), w_terrain.sizey());
  w_terrain.clear();
  curMap.draw(&w_terrain, player.pos.x, player.pos.y, 8,
              player.my_body.symbol);

  int_body.set_data("num_str", player.my_body.get_str());
  int_body.set_data("num_dex", player.my_body.get_str());
  int_body.set_data("num_per", player.my_body.get_str());
  int_body.set_data("list_parts", player.my_body.body_part_names());
  int_body.set_data("list_hp",    player.my_body.body_part_hps());
  int_body.set_data("list_ac",    player.my_body.body_part_acs());
  int_body.draw(&w_body);
  int_soul.draw(&w_soul);
  int_messages.draw(&w_messages);
  refresh_all();
}

void game::pl_move(int dx, int dy)
{
  int destx = player.pos.x + dx, desty = player.pos.y + dy;
  tile* dest = &(curMap.ter(destx, desty));
  if (dest->type->move_cost == 0) {
    curMap.apply_transformation(destx, desty, TRANS_OPEN, 1);
    return;
  }
  player.pos = point(destx, desty);
}

void game::add_msg(std::string message)
{
  messages.push_back(message);
  messages_to_print++;
}

void game::print_messages()
{
  
}
