#include <string>
#include <sstream>
#include "globals.h"
#include "terrain.h"
#include "interface.h"
#include "window.h"
#include "game.h"

int main()
{
  init_display();
  init_environment();
  init_data();

  game g;
  g.init();
  while (g.do_turn());

  end_display();
  return 0;
}
