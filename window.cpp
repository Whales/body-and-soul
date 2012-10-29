#include "window.h"

Window::Window()
{
 w = new WINDOW;
 outlined = false;
}

Window::Window(int sizex, int sizey, int posx, int posy)
{
 w = newwin(sizey, sizex, posy, posx);
 outlined = false;
}

Window::~Window()
{
 delete w;
}

void Window::outline()
{
 outlined = true;
 wborder(w, LINE_XOXO, LINE_XOXO, LINE_OXOX, LINE_OXOX,
            LINE_OXXO, LINE_OOXX, LINE_XXOO, LINE_XOOX );
}
 
void Window::putch(int x, int y, long sym, nc_color fg, nc_color bg)
{
 long col = get_color_pair(fg, bg);
 wattron(w, col);
 mvwaddch(w, y, x, sym);
 wattroff(w, col);
}

void Window::putglyph(int x, int y, glyph gl)
{
 putch(x, y, gl.symbol, gl.fg, gl.bg);
}

void Window::putstr(int x, int y, nc_color fg, nc_color bg, std::string str,
                    ...)
{
 va_list ap;
 va_start(ap, str);
 char buff[8192];
 vsprintf(buff, mes.c_str(), ap);
 va_end(ap);

 long col = get_color_pair(fg, bg);
 wattron(w, col);
 mvwprintw(y, x, buff);
 wattroff(w, col);
}
