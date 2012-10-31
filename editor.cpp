#include "terrain.h"
#include "window.h"
#include "globals.h"

int main()
{
 init_display();
 init_environment();
 init_data();

 Window w_header(80, 1, 0, 0);
 w_header.putstr(1, 0, c_white, c_black, "<c=,blue>N<c=/>ew <c=,blue>L<c=/>oad <c=,blue>S<c=/>ave");

 Window w_main(80, 23, 0, 1);
 w_main.outline();
 w_main.putstr(0, 0, c_white, c_black, "<c=,blue>N<c=/>ew <c=,blue>L<c=/>oad");

 refresh_all();

 getch();

 endwin();

 return 0;
}