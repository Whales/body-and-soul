#ifndef _GAME_H_
#define _GAME_H_

#include "entity.h"
#include "item.h"
#include "map.h"
#include "interface.h"

class game
{
 public:
  game() {};
  ~game(){};

  void init();
  bool do_turn();
  void draw();
  void pl_move(int dx, int dy);

  void add_msg(std::string message);
  void print_messages();

  entity player;
  map curMap;

 private:
  Window w_terrain;
  Window w_body;
  Window w_soul;
  Window w_messages;

  cuss::interface int_body;
  cuss::interface int_soul;
  cuss::interface int_messages;

  std::vector<std::string> messages;
  int messages_to_print;
};

#endif
