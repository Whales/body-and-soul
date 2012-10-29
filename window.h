#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <ncurses>
#include "glyph.h"

//      LINE_NESW  - X for on, O for off
#define LINE_XOXO 4194424
#define LINE_OXOX 4194417
#define LINE_XXOO 4194413
#define LINE_OXXO 4194412
#define LINE_OOXX 4194411
#define LINE_XOOX 4194410
#define LINE_XXXO 4194420
#define LINE_XXOX 4194422
#define LINE_XOXX 4194421
#define LINE_OXXX 4194423
#define LINE_XXXX 4194414


class Window
{
 public:
  Window();
  Window(int sizex, int sizey, int posx, int posy);
  ~Window();

  void outline();

  void putch(int x, int y, long sym,
             nc_color fg = c_ltgray, nc_color bg = c_black);

  void putglyph(int x, int y, glyph gl);
  void putstr(int x, int y, nc_color fg, nc_color bg, std::string str, ...);
 private:
  WINDOW* w;
  bool outlined;
};
