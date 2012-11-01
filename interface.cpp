#include "interface.h"

void print_scrollbar(Window *win, int posx, int posy, int length, int offset,
                     int size, bool selected);
std::vector<std::string> break_into_lines(std::string text);

using namespace nci;

#define SELECTCOLOR c_blue

// First we have all the drawing functions for the different elements.
void ele_drawing::draw(Window *win)
{
 for (int i = 0; i < drawing.size() && i < sizex * sizey; i++)
  win->putglyph(posx + i % sizex, posy + i / sizey, drawing[i]);
}

void ele_textbox::draw(Window *win)
{
 win->clear_area(posx, posy, posx + sizex - 1, posy + sizey - 1);

 for (int i = 0; i + offset < text.size() && i < sizey; i++)
  win->putstr(posx, posy + i, c_white, c_black, text[i + offset]);

 if (selectable)
  print_scrollbar(win, posx + sizex - 1, posy, sizey, offset, text.size(),
                  selected);

}
     
void ele_list::draw(Window *win)
{
 win->clear_area(posx, posy, posx + sizex - 1, posy + sizey - 1);

 for (int i = 0; i + offset < text.size() && i < sizey; i++) {
  nc_color bg = (selection == i + offset ? SELECTCOLOR : c_black);
  win->putstr(posx, posy + i, c_white, bg, text[i + offset]);
 }

 if (selectable)
  print_scrollbar(win, posx + sizex - 1, posy, sizey, offset, text.size(),
                  selected);
}

void ele_textentry::draw(Window *win)
{
 nc_color bg = (selected ? SELECTCOLOR : c_black);
// Ensure we see the end of the word--and a blank space
 int start = (selected ? text.size() + 1 - sizex : 0);
 if (start < 0)
  start = 0;
 int length = (selected ? sizex - 1 : sizex);

 std::string print = text.substr(start, length);

 win->putstr(posx, posy, c_white, bg, print);
 for (int x = posx + print.length(); x < posx + sizex; x++)
  win->putch(x, posy, c_white, bg, '_');
}

// This function assumes that the number won't ever be too big for the field.
void ele_number::draw(Window *win)
{
 nc_color bg = (selected ? SELECTCOLOR : c_black);
 win->putstr(posx, posy, c_white, bg, "%d", value);
}

void print_scrollbar(Window *win, int posx, int posy, int length, int offset,
                     int size, bool selected)
{
 nc_color barcol = (selected ? SELECTCOLOR : c_ltgray);
 int barsize = (length >= size ? -1 : (length * length) / size);

 if (barsize == -1) {
// Don't print a scroll bar
  for (int y = posy; y < posy + length; y++)
   win->putch(posx, y, barcol, c_black, LINE_XOXO);
 } else {
  int barpos = (offset + length >= size ? length - barsize :
                (offset * length) / size);
  for (int y = posy; y < posy + length; y++) {
   long ch = ((y >= barpos && y < barpos + barsize) ? '#' : LINE_XOXO);
   win->putch(posx, y, barcol, c_black, ch);
  }
 }
}
 

std::vector<std::string> break_into_lines(std::string text, int linesize)
{
 std::vector<std::string> ret;

 while (text.length() > linesize) {

  size_t linebreak = text.find_last_of(" ", linesize);
  std::string tmp;
   if (linebreak == std::string::npos) {
    linebreak = linesize - 1;
    tmp = text.substr(0, linebreak) + "-";
   } else
    tmp = text.substr(0, linebreak);

  ret.push_back(tmp);
  text = text.substr(linebreak + 1);
 }

 return ret;
}
