#include <string>
#include <sstream>
#include "globals.h"
#include "itemtype.h"
#include "interface.h"
#include "window.h"

int main()
{
  init_display();
  init_environment();
  init_data();

  save_data();
  end_display();
  return 0;
}
