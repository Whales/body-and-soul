#include "terrain.h"
#include "window.h"
#include "globals.h"
//#include "interface.h"

int main()
{
 init_display();
 init_environment();
 init_data();

 Window w_header(80, 1, 0, 0);
 w_header.putstr(1, 0, c_white, c_black, "<c=,blue>N<c=/>ew <c=,blue>L<c=/>oad <c=,blue>S<c=/>ave");

 Window w_main(80, 23, 0, 1);
 w_main.outline();
 w_main.putstr( 1, 0, c_black, c_white, "<c=,ltblue>S<c=/>tats");
 w_main.line_v(26);
 int line = 1;
 body_stat test;
 test.name = "Test stat";
 test.value = 10;
 BODY_STATS_POOL.push_back(test);
 for (std::list<body_stat>::iterator it = BODY_STATS_POOL.begin();
      it != BODY_STATS_POOL.end() && line < 23; it++) {
  w_main.putstr(1, line, c_white, c_black, "%s - %d", it->name.c_str(), it->value);
  line++;
 }
 w_main.putstr(27, 0, c_black, c_white, "<c=,ltblue>P<c=/>arts");
 w_main.line_v(52);
 w_main.putstr(53, 0, c_black, c_white, "<c=,ltblue>B<c=/>odies");

 refresh_all();

 getch();

 save_data();

 endwin();

 return 0;
}
