#include "interface.h"

void print_scrollbar(Window *win, int posx, int posy, int length, int offset,
                     int size, bool selected);
std::vector<std::string> break_into_lines(std::string text, int linesize);

using namespace nci;

#define SELECTCOLOR c_blue

#define PREP_ELEMENT(ele) \
  (ele).name = name;\
  (ele).posx = posx;\
  (ele).posy = posy;\
  (ele).sizex = sizex;\
  (ele).sizey = sizey;\
  (ele).selected = false;\
  (ele).selectable = selectable

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

bool ele_textbox::set_data(std::string data)
{
 std::vector<std::string> broken = break_into_lines(data, sizex);
 set_data(broken);
 return true;
}

bool ele_textbox::add_data(std::string data)
{
 std::vector<std::string> broken = break_into_lines(data, sizex);
 add_data(broken);
 return true;
}

bool ele_textbox::set_data(std::vector<std::string> data)
{
 text = data;
 return true;
}

bool ele_textbox::add_data(std::vector<std::string> data)
{
 for (int i = 0; i < data.size(); i++)
  text.push_back(data[i]);
 return true;
}

void ele_list::draw(Window *win)
{
 win->clear_area(posx, posy, posx + sizex - 1, posy + sizey - 1);

 for (int i = 0; i + offset < list.size() && i < sizey; i++) {
  nc_color bg = (selection == i + offset ? SELECTCOLOR : c_black);
  win->putstr(posx, posy + i, c_white, bg, list[i + offset]);
 }

 if (selectable)
  print_scrollbar(win, posx + sizex - 1, posy, sizey, offset, list.size(),
                  selected);
}

bool ele_list::add_data(std::string data)
{
 list.push_back(data);
 return true;
}

bool ele_list::set_data(std::vector<std::string> data)
{
 list = data;
 return true;
}

bool ele_list::add_data(std::vector<std::string> data)
{
 for (int i = 0; i < data.size(); i++)
  list.push_back(data[i]);
 return true;
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

bool ele_textentry::set_data(std::string data)
{
 text = data;
 return true;
}

bool ele_textentry::add_data(std::string data)
{
 text += data;
 return true;
}

// This function assumes that the number won't ever be too big for the field.
void ele_number::draw(Window *win)
{
 nc_color bg = (selected ? SELECTCOLOR : c_black);
 win->putstr(posx, posy, c_white, bg, "%d", value);
}

bool ele_number::set_data(int data)
{
 value = data;
 return true;
}

bool ele_number::add_data(int data)
{
 value += data;
 return true;
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

interface::interface()
{
 active_element = -1;
}

void interface::add_element(element_type type, std::string name, int posx,
                            int posy, int sizex, int sizey, bool selectable)
{
 switch (type) {

 case ELE_NULL:
  return; // We don't have any reason to actually add these, right?

 case ELE_DRAWING: {
  ele_drawing ele;
  PREP_ELEMENT(ele);
  elements.push_back(ele);
  } break;

 case ELE_TEXTBOX: {
  ele_textbox ele;
  PREP_ELEMENT(ele);
  elements.push_back(ele);
  } break;

 case ELE_LIST: {
  ele_list ele;
  PREP_ELEMENT(ele);
  elements.push_back(ele);
  } break;

 case ELE_TEXTENTRY: {
  ele_textentry ele;
  PREP_ELEMENT(ele);
  elements.push_back(ele);
  } break;

 case ELE_NUMBER: {
  ele_number ele;
  PREP_ELEMENT(ele);
  elements.push_back(ele);
  } break;

 default:
  debugmsg("Unknown element type %d", type);
  return;
 }

 if (active_element == -1)
  active_element = 0;
}

void interface::draw(Window *win)
{
 for (int i = 0; i < elements.size(); i++)
  elements[i].draw(win);
 win->refresh();
}

element* interface::selected()
{
 if (active_element < 0 || active_element >= elements.size())
  return NULL;

 return &(elements[active_element]);
}

element* interface::find_by_name(std::string name)
{
 for (int i = 0; i < elements.size(); i++) {
  if (elements[i].name == name)
   return &(elements[i]);
 }
 return NULL;
}

void interface::select_next()
{
 if (active_element == elements.size() - 1)
  active_element = 0;
 else
  active_element++;
}

void interface::select_last()
{
 if (active_element == 0)
  active_element = elements.size() - 1;
 else
  active_element--;
}

bool interface::select(std::string name)
{
 for (int i = 0; i < elements.size(); i++) {
  if (elements[i].name == name) {
   active_element = i;
   return true;
  }
 }
 return false;
}

bool interface::set_data(std::string name, std::string data)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->set_data(data);
}

bool interface::add_data(std::string name, std::string data)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->add_data(data);
}

bool interface::set_data(std::string name, std::vector<std::string> data)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->set_data(data);
}

bool interface::add_data(std::string name, std::vector<std::string> data)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->add_data(data);
}

bool interface::set_data(std::string name, int data)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->set_data(data);
}

bool interface::add_data(std::string name, int data)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->add_data(data);
}

