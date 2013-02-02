#include <string>
#include <sstream>
#include "globals.h"
#include "terrain.h"
#include "interface.h"
#include "window.h"
#include "map.h"
#include "rng.h"

int main()
{
  init_display();
  init_environment();
  init_data();

  map demo;
  demo.generate(MAP_CITY);

  Window w_map(0, 0, 80, 24);
  int posx = 0, posy = 0;
  bool quit = false;
  do {
    demo.draw(&w_map,  posx, posy, 0);
    w_map.refresh();
    char ch = getch();
    switch (ch) {
    case 'k': posy--;         break;
    case 'j': posy++;         break;
    case 'h': posx--;         break;
    case 'l': posx++;         break;
    case 'y': posy--; posx--; break;
    case 'u': posy--; posx++; break;
    case 'b': posy++; posx--; break;
    case 'n': posy++; posx++; break;
    case 'q': quit = true;    break;
    case '.': demo.apply_transformation(posx, posy, TRANS_HEAT, 50);
              demo.process_transformations(); break;
    }
  } while (!quit);

  end_display();
  return 0;
}
