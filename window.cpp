#include <vector>
#include <string>
#include <list>
#include "window.h"
#include "globals.h"

Window::Window()
{
 w = newwin(0, 0, 0, 0);
 outlined = false;
 xdim = 0;
 ydim = 0;
 WINDOWLIST.push_back(this);
}

Window::Window(int posx, int posy, int sizex, int sizey)
{
 w = newwin(sizey, sizex, posy, posx);
 outlined = false;
 xdim = sizex;
 ydim = sizey;
 WINDOWLIST.push_back(this);
}

Window::~Window()
{
 delwin(w);
 WINDOWLIST.remove(this);
}

void Window::init(int posx, int posy, int sizex, int sizey)
{
 delwin(w);
 w = newwin(sizey, sizex, posy, posx);
 xdim = sizex;
 ydim = sizey;
}

void Window::close()
{
 delwin(w);
 WINDOWLIST.remove(this);
 refresh_all(true);
}

void Window::outline()
{
 outlined = true;
 long col = get_color_pair(c_white, c_black);
 wattron(w, col);
 wborder(w, LINE_XOXO, LINE_XOXO, LINE_OXOX, LINE_OXOX,
            LINE_OXXO, LINE_OOXX, LINE_XXOO, LINE_XOOX );
 wattroff(w, col);
}
 
void Window::putch(int x, int y, nc_color fg, nc_color bg, long sym)
{
/*
 if (outlined) {
  x++;
  y++;
 }
*/
 long col = get_color_pair(fg, bg);
 wattron(w, col);
 mvwaddch(w, y, x, sym);
 wattroff(w, col);
}

void Window::putglyph(int x, int y, glyph gl)
{
 putch(x, y, gl.fg, gl.bg, gl.symbol);
}

void Window::putstr(int x, int y, nc_color fg, nc_color bg, std::string str,
                    ...)
{
/*
 if (outlined) {
  x++;
  y++;
 }
*/
 va_list ap;
 va_start(ap, str);
 char buff[8192];
 vsprintf(buff, str.c_str(), ap);
 va_end(ap);

 std::string prepped = buff;
 long col = get_color_pair(fg, bg);

 if (prepped.find_first_of("<c=") == std::string::npos) {
// No need to do color segments, so just print!
  wattron(w, col);
  mvwprintw(w, y, x, buff);
  wattroff(w, col);
 } else { // We need to do color segments!
  wmove(w, y, x);
  size_t tag;
  std::vector<std::string> segments;
  std::vector<long> color_pairs;
  nc_color cur_fg = fg, cur_bg = bg;

  while ( (tag = prepped.find_first_of("<c=")) != std::string::npos ) {
// Everything before the tag is a segment, with the current colors
   segments.push_back( prepped.substr(0, tag) );
   color_pairs.push_back( get_color_pair(cur_fg, cur_bg) );
// Strip off everything up to and including "<c="
   prepped = prepped.substr(tag + 3);
// Find the end of the tag
   size_t tagend = prepped.find_first_of(">");
   if (tagend == std::string::npos) {
    debugmsg("Unterminated color tag!");
    return;
   }
   std::string tag = prepped.substr(0, tagend);
// Strip out the tag
   prepped = prepped.substr(tagend + 1);

   if (tag == "reset" || tag == "/") { // Reset the colors
    cur_fg = fg;
    cur_bg = bg;
   } else { // We're looking for the color!
    size_t comma = tag.find(",");
    if (comma == std::string::npos) { // No comma - just setting fg
     cur_fg = color_string(tag);
     if (cur_fg == c_null) {
      debugmsg("Malformed color tag: %s", tag.c_str());
      return;
     }
    } else {
     nc_color new_fg = color_string( tag.substr(0, comma) ),
              new_bg = color_string( tag.substr(comma + 1) );
     if (new_fg == c_null && new_bg == c_null) {
      debugmsg("Malformed color tag: %s", tag.c_str());
      return;
     }
     if (new_fg != c_null)
      cur_fg = new_fg;
     if (new_bg != c_null)
      cur_bg = new_bg;
    } // if comma was found
   } // color needed to be found
  } // while (tag != std::string::npos)
// There's a little string left over; push it into our vectors!
  segments.push_back(prepped);
  color_pairs.push_back( get_color_pair(cur_fg, cur_bg) );

  if (segments.size() != color_pairs.size()) {
   debugmsg("Segments.size() = %d, color_pairs.size() = %d",
            segments.size(), color_pairs.size());
   return;
  }

  for (int i = 0; i < segments.size(); i++) {
   wattron( w, color_pairs[i] );
   wprintw(w, segments[i].c_str());
   wattroff( w, color_pairs[i] );
  }
 } // We need to do color segments!

}

void Window::clear_area(int x1, int y1, int x2, int y2)
{
 for (int x = x1; x <= x2; x++) {
  for (int y = y1; y <= y2; y++)
   putch(x, y, c_black, c_black, 'x');
 }
}

void Window::line_v(int x, nc_color fg, nc_color bg)
{
 for (int y = (outlined ? 1 : 0); y < (outlined ? ydim - 1 : ydim); y++)
  putch(x, y, fg, bg, LINE_XOXO);

 if (outlined) { // Alter the outline so it attaches to our line
  putch(x, 0, fg, bg, LINE_OXXX);
  putch(x, ydim - 1, fg, bg, LINE_XXOX);
 }
}

void Window::line_h(int y, nc_color fg, nc_color bg)
{
 for (int x = (outlined ? 1 : 0); x < (outlined ? xdim - 1 : xdim); x++)
  putch(x, y, fg, bg, LINE_OXOX);

 if (outlined) { // Alter the outline so it attaches to our line
  putch(0, y, fg, bg, LINE_XXXO);
  putch(xdim - 1, y, fg, bg, LINE_XOXX);
 }
}

void Window::refresh()
{
 wrefresh(w);
}

void init_display()
{
 initscr();
 noecho();
 cbreak();
 keypad(stdscr, true);
 init_colors();
 curs_set(0);
 timeout(1);
 getch();
 timeout(-1);
}

long input()
{
 return getch();
}

void debugmsg(const char *mes, ...)
{
 va_list ap;
 va_start(ap, mes);
 char buff[1024];
 vsprintf(buff, mes, ap);
 va_end(ap);
 attron(c_red);
 mvprintw(0, 0, "DEBUG: %s      \n  Press spacebar...", buff);
 while(getch() != ' ');
 attroff(c_red);
}

void refresh_all(bool erase) // erase defaults to false
{
 if (erase)
  clear();

 for (std::list<Window*>::iterator it = WINDOWLIST.begin();
      it != WINDOWLIST.end(); it++)
  (*it)->refresh();
}
