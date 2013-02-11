#include "game.h"
#include "globals.h"

void game::init()
{
  w_body.init( 0,  0, 27, 25);
  //w_terrain = Window(27,  0, 25, 24);
  w_soul.init(52,  0, 28, 25);
  //w_terrain = Window(0, 0, 80, 24);
  w_terrain.init(0, 0, 80, 25);

  curMap.generate(MAP_WOODS);
  player.pos = point(80, 80);
  player.my_body = BODIES_POOL[ lookup_body_id("human") ];
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
  curMap.draw(&w_terrain, player.pos.x, player.pos.y, 10,
              player.my_body.symbol);

  w_terrain.refresh();
}

void game::pl_move(int dx, int dy)
{
  int destx = player.pos.x + dx, desty = player.pos.y + dy;
  tile* dest = &(curMap.ter(destx, desty));
  if (dest->type->move_cost == 0) {
    curMap.apply_transformation(destx, desty, TRANS_OPEN, 100);
    return;
  }
  player.pos = point(destx, desty);
}
