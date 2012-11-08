#include "interface.h"
#include "stringfunc.h"
#include <sstream>
#include <fstream>

void print_scrollbar(Window *win, int posx, int posy, int length, int offset,
                     int size, bool selected);

using namespace cuss;

#define SELECTCOLOR c_blue

#define PREP_ELEMENT(ele) \
  (ele)->name = name;\
  (ele)->posx = posx;\
  (ele)->posy = posy;\
  (ele)->sizex = szx;\
  (ele)->sizey = szy;\
  (ele)->selected = false;\
  (ele)->selectable = selectable

// Base save/load functions.
std::string element::save_data()
{
 std::stringstream ret;
 ret << name << " " << STD_DELIM << " " << posx << " " << posy << " " <<
        sizex << " " << sizey << " " << selectable;
 return ret.str();
}

void element::load_data(std::istream &datastream)
{
 name = load_to_delim(datastream, STD_DELIM);

 datastream >> posx >> posy >> sizex >> sizey >> selectable;
}

bool element::set_data(nc_color FG, nc_color BG)
{
 fg = FG;
 if (BG != c_null) // bg defaults to c_null
  bg = BG;

 return true;
}

// *** DRAWING ELEMENT ***
void ele_drawing::draw(Window *win)
{
 std::map<point, glyph>::iterator it;
 for (it = drawing.begin(); it != drawing.end(); it++)
  win->putglyph( it->first.x, it->first.y, it->second);
}

std::string ele_drawing::save_data()
{
 std::stringstream ret;
 ret << element::save_data() << " " << drawing.size() << " ";
 std::map<point, glyph>::iterator it;
 for (it = drawing.begin(); it != drawing.end(); it++)
  ret << it->first.x << " " << it->first.y << " " << it->second.save_data() <<
         " ";
 return ret.str();
}

void ele_drawing::load_data(std::istream &datastream)
{
 element::load_data(datastream);
 int tmpsize;
 datastream >> tmpsize;
 for (int i = 0; i < tmpsize; i++) {
  point tmppoint;
  glyph tmpglyph;
  datastream >> tmppoint.x >> tmppoint.y;
  tmpglyph.load_data(datastream);
  drawing[tmppoint] = tmpglyph;
 }
}
 
bool ele_drawing::set_data(glyph gl, int posx, int posy)
{
 if (posx < 0 || posx >= sizex || posy < 0 || posy >= sizey)
  return false;

 if (gl.symbol == -1)
  drawing.erase( point(posx, posy) );
 else
  drawing[ point(posx, posy) ] = gl;

 return true;
}

bool ele_drawing::set_data(nc_color FG, nc_color BG)
{
 std::map<point, glyph>::iterator it;
 for (it = drawing.begin(); it != drawing.end(); it++) {
  it->second.fg = FG;
  if (BG != c_null) // bg defaults to c_null
   it->second.bg = BG;
 }

 return true;
}

// *** TEXTBOX ELEMENT ***
void ele_textbox::draw(Window *win)
{
 win->clear_area(posx, posy, posx + sizex - 1, posy + sizey - 1);

 for (int i = 0; i + offset < text.size() && i < sizey; i++)
  win->putstr(posx, posy + i, fg, bg, text[i + offset]);

 if (selectable)
  print_scrollbar(win, posx + sizex - 1, posy, sizey, offset, text.size(),
                  selected);
}

std::string ele_textbox::save_data()
{
 std::stringstream ret;
 ret << element::save_data() << " " << text.size();
 for (int i = 0; i < text.size(); i++)
  ret << text[i] << " " << STD_DELIM << " ";

 return ret.str();
}

void ele_textbox::load_data(std::istream &datastream)
{
 element::load_data(datastream);
 int tmpsize;
 datastream >> tmpsize;
 for (int i = 0; i < tmpsize; i++) {
  std::string tmp = load_to_delim(datastream, STD_DELIM);
  text.push_back(tmp);
 }
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

// *** LIST ELEMENT ***
void ele_list::draw(Window *win)
{
 win->clear_area(posx, posy, posx + sizex - 1, posy + sizey - 1);

 for (int i = 0; i + offset < list.size() && i < sizey; i++) {
  nc_color hilite = (selection == i + offset ? SELECTCOLOR : bg);
  if (!selected)
   hilite = bg;
  win->putstr(posx, posy + i, fg, hilite, list[i + offset]);
 }

 if (selectable)
  print_scrollbar(win, posx + sizex - 1, posy, sizey, offset, list.size(),
                  selected);
}

std::string ele_list::save_data()
{
 std::stringstream ret;
 ret << element::save_data() << " " << list.size();
 for (int i = 0; i < list.size(); i++)
  ret << list[i] << " " << STD_DELIM << " ";

 return ret.str();
}

void ele_list::load_data(std::istream &datastream)
{
 element::load_data(datastream);
 int tmpsize;
 datastream >> tmpsize;
 for (int i = 0; i < tmpsize; i++) {
  std::string tmp = load_to_delim(datastream, STD_DELIM);
  list.push_back(tmp);
 }
}

bool ele_list::add_data(std::string data)
{
 list.push_back(data);
 return true;
}

bool ele_list::set_data(std::vector<std::string> data)
{
 list = data;
 selection = 0;
 offset = 0;
 return true;
}

bool ele_list::add_data(std::vector<std::string> data)
{
 for (int i = 0; i < data.size(); i++)
  list.push_back(data[i]);
 return true;
}

bool ele_list::set_data(int data)
{
 selection = data;
 if (selection < sizey)
  offset = 0;
 else if (selection >= list.size() - sizey)
  offset = list.size() - sizey;
 else
  offset = selection;

 return true;
}

bool ele_list::add_data(int data)
{
 selection += data;
 if (selection < 0)
  selection = 0;
 if (selection >= list.size())
  selection = list.size() - 1;

 if (selection < offset)
  selection--;
 if (offset + sizey <= selection)
  offset = selection - sizey + 1;

 return true;
}

std::string ele_list::get_str()
{
 if (selection < 0 || selection >= list.size()) {
  std::string ret;
  return ret;
 }

 return list[selection];
}

// *** TEXT ENTRY ELEMENT ***
void ele_textentry::draw(Window *win)
{
 nc_color hilite = (selected ? SELECTCOLOR : bg);
// Ensure we see the end of the word--and a blank space
 int start = (selected ? text.size() + 1 - sizex : 0);
 if (start < 0)
  start = 0;
 int length = (selected ? sizex - 1 : sizex);

 std::string print = text.substr(start, length);

 win->putstr(posx, posy, fg, hilite, print);
 for (int x = posx + print.length(); x < posx + sizex; x++)
  win->putch(x, posy, bg, hilite, '_');
}

std::string ele_textentry::save_data()
{
 std::stringstream ret;
 ret << element::save_data() << " " << text << " " << STD_DELIM;
 return ret.str();
}

void ele_textentry::load_data(std::istream &datastream)
{
 element::load_data(datastream);
 text = load_to_delim(datastream, STD_DELIM);
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


// *** NUMBER ELEMENT ***
// This function assumes that the number won't ever be too big for the field.
void ele_number::draw(Window *win)
{
 nc_color hilite = (selected ? SELECTCOLOR : bg);
 win->putstr(posx, posy, fg, hilite, "%d", value);
}

std::string ele_number::save_data()
{
 std::stringstream ret;
 ret << element::save_data() << " " << value;
 return ret.str();
}

void ele_number::load_data(std::istream &datastream)
{
 element::load_data(datastream);
 datastream >> value;
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

interface::interface()
{
 active_element = -1;
}

void interface::add_element(element_type type, std::string name, int posx,
                            int posy, int szx, int szy, bool selectable)
{
 if (posx < 0 || posx >= sizex || posy < 0 || posy >= sizey)
  return;

 if (posx + szx >= sizex)
  szx = sizex - posx;
 if (posy + szy >= sizey)
  szy = sizey - posy;

 switch (type) {
 
  case ELE_NULL:
   return; // We don't have any reason to actually add these, right?
 
  case ELE_DRAWING: {
   ele_drawing *ele = new ele_drawing;
   PREP_ELEMENT(ele);
   elements.push_back(ele);
  } break;
 
  case ELE_TEXTBOX: {
   ele_textbox *ele = new ele_textbox;
   PREP_ELEMENT(ele);
   elements.push_back(ele);
  } break;
 
  case ELE_LIST: {
   ele_list *ele = new ele_list;
   PREP_ELEMENT(ele);
   elements.push_back(ele);
  } break;
 
  case ELE_TEXTENTRY: {
   ele_textentry *ele = new ele_textentry;
   PREP_ELEMENT(ele);
   elements.push_back(ele);
  } break;
 
  case ELE_NUMBER: {
   ele_number *ele = new ele_number;
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
 win->clear();
 for (int i = 0; i < elements.size(); i++)
  elements[i]->draw(win);
 win->refresh();
}

void interface::draw_prototype(Window *win)
{
 win->clear();
 for (int i = 0; i < elements.size(); i++) {
  if (elements[i]->name != "BG") {
   int x1 = elements[i]->posx, y1 = elements[i]->posy;
   int x2 = x1 + elements[i]->sizex - 1, y2 = y1 + elements[i]->sizey - 1;
   for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
     element_type type = elements[i]->type();
     win->putch(x, y, c_black,
                type == ELE_DRAWING ? c_dkgray : nc_color(2 + type), ' ');
    }
   }
   if (y1 == y2) {
    win->putch(x1, y1, c_white, c_black, LINE_XXXO);
    win->putch(x2, y2, c_white, c_black, LINE_XOXX);
   } else if (x1 == x2) {
    win->putch(x1, y1, c_white, c_black, LINE_OXXX);
    win->putch(x2, y2, c_white, c_black, LINE_XXOX);
   } else {
    win->putch(x1, y1, c_white, c_black, LINE_OXXO);
    win->putch(x2, y2, c_white, c_black, LINE_XOOX);
   }
   win->putstr(x1 + 1, y1, (elements[i]->selected ? c_magenta : c_yellow),
               c_black, elements[i]->name.substr(0, elements[i]->sizex - 2));
  }
  if (elements[i]->type() == ELE_DRAWING)
   elements[i]->draw(win);
 }
}

std::string interface::save_data()
{
 std::stringstream ret;
 ret << name << " " << STD_DELIM << " " << elements.size() << " ";
 for (int i = 0; i < elements.size(); i++)
  ret << elements[i]->type() << " " << elements[i]->save_data() << " ";

 return ret.str();
}

void interface::load_data(std::istream &datastream)
{
 name = load_to_delim(datastream, STD_DELIM);
 elements.clear();
 int tmpcount;
 datastream >> tmpcount;
 for (int i = 0; i < tmpcount; i++) {
  int tmptype;
  datastream >> tmptype;
  switch ( element_type(tmptype) ) {

   case ELE_NULL:
    debugmsg("Loaded NULL element!");
    break;

   case ELE_DRAWING: {
    ele_drawing *tmp = new ele_drawing;
    tmp->load_data(datastream);
    elements.push_back(tmp);
   } break;

   case ELE_TEXTBOX: {
    ele_textbox *tmp = new ele_textbox;
    tmp->load_data(datastream);
    elements.push_back(tmp);
   } break;

   case ELE_LIST: {
    ele_list *tmp = new ele_list;
    tmp->load_data(datastream);
    elements.push_back(tmp);
   } break;

   case ELE_TEXTENTRY: {
    ele_textentry *tmp = new ele_textentry;
    tmp->load_data(datastream);
    elements.push_back(tmp);
   } break;

   case ELE_NUMBER: {
    ele_number *tmp = new ele_number;
    tmp->load_data(datastream);
    elements.push_back(tmp);
   } break;
  }
 } // for (int i = 0; i < tmpcount; i++)
}

bool interface::save_to_file(std::string filename)
{
 std::ofstream fout;
 fout.open(filename.c_str());
 if (!fout.is_open())
  return false;

 fout << save_data();
 fout.close();
 return true;
}

bool interface::load_from_file(std::string filename)
{
 std::ifstream fin;
 fin.open(filename.c_str());
 if (!fin.is_open())
  return false;

 load_data(fin);
 fin.close();
 return true;
}

std::vector<std::string> interface::element_names()
{
 std::vector<std::string> ret;
 for (int i = 0; i < elements.size(); i++)
  ret.push_back( elements[i]->name );

 return ret;
}

element* interface::selected()
{
 if (active_element < 0 || active_element >= elements.size())
  return NULL;

 return elements[active_element];
}

element* interface::find_by_name(std::string name)
{
 for (int i = 0; i < elements.size(); i++) {
  if (elements[i]->name == name)
   return elements[i];
 }
 return NULL;
}

void interface::select_next(bool force)
{
 if (elements.empty()) {
  active_element = -1;
  return;
 }

 if (active_element >= 0 && active_element < elements.size())
  elements[active_element]->selected = false;

 int tried = 0;
 do {
  tried++;
  if (active_element == elements.size() - 1)
   active_element = 0;
  else
   active_element++;
 } while ((!force && tried < elements.size() &&
           !elements[active_element]->selectable) ||
          elements[active_element]->name == "BG");

 if (tried == elements.size() && !elements[active_element]->selectable)
  active_element = -1;
 else
  elements[active_element]->selected = true;
}

void interface::select_last(bool force)
{
 if (elements.empty()) {
  active_element = -1;
  return;
 }

 if (active_element >= 0 && active_element < elements.size())
  elements[active_element]->selected = false;

 int tried = 0;
 do {
  tried++;
  if (active_element == 0)
   active_element = elements.size() - 1;
  else
   active_element--;
 } while ((!force && tried < elements.size() &&
           !elements[active_element]->selectable) ||
          elements[active_element]->name == "BG");

 if (tried == elements.size() && !elements[active_element]->selectable)
  active_element = -1;
 else
  elements[active_element]->selected = true;
}

void interface::select_none()
{
 if (active_element >= 0 && active_element < elements.size())
  elements[active_element]->selected = false;

 active_element = -1;
}

bool interface::select(std::string name)
{
 for (int i = 0; i < elements.size(); i++) {
  if (elements[i]->name == name) {
   if (active_element >= 0 && active_element < elements.size())
    elements[active_element]->selected = false;
   active_element = i;
   elements[active_element]->selected = true;
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

bool interface::set_data(std::string name, glyph gl, int x, int y)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->set_data(gl, x, y);
}

// bg defaults to c_null
bool interface::set_data(std::string name, nc_color fg, nc_color bg)
{
 element* ele = find_by_name(name);
 if (!ele)
  return false;

 return ele->set_data(fg, bg);
}

std::string interface::get_str(std::string name)
{
 element* ele = find_by_name(name);
 if (!ele) {
  std::string ret;
  return ret;
 }

 return ele->get_str();
}

int interface::get_int(std::string name)
{
 element* ele = find_by_name(name);
 if (!ele)
  return 0;

 return ele->get_int();
}

