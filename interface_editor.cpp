#include <fstream>
#include "interface.h"

#define PREP_ELE(ele) \
  ele.name = name;\
  ele.posx = posx;\
  ele.posy = posy;\
  ele.sizex = sizex;\
  ele.sizey = sizey;\
  ele.selectable = selectable;\
  edited.push_back(ele)

using namespace nci;

bool pick_point(Window &w, int &x, int &y);

int main()
{ 
 init_display();
 nci::interface edited;
 Window w(0, 0, 80, 25);

 std::ifstream fin;
 fin.open("editor.nci");
 if (fin.is_open()) {
  edited.load_data(fin);
  fin.close();
 } else {
  edited.name = "nci_editor";
  edited.add_element(ELE_DRAWING, "BG", 0, 0, 80, 25, false);
 }

 bool done = false, drawmode = false;

 do {
  long ch = input();
  if (drawmode) {
   edited.
  switch (ch) {

   case 'I': case 'i':
    drawmode != drawmode;
    break;

   case 'N': case 'n': {
    element_type type = ELE_NULL;
    switch (menu("Element type:", "Drawing", "Text", "List", "Text Entry",
                                  "Number", 0)) {
     case 1: type = ELE_DRAWING;   break;
     case 2: type = ELE_TEXTBOX;   break;
     case 3: type = ELE_LIST;      break;
     case 4: type = ELE_TEXTENTRY; break;
     case 5: type = ELE_NUMBER;    break;
    }
    int posx, posy, sizex, sizey;
    w.putstr(0, 0, c_red, c_black, "Starting point:");
    bool didit = pick_point(w, posx, posy);
    edited.draw(&w);
    w.putstr(0, 0, c_red, c_black, "End point:     ");
    didit |= pick_point(w, sizex, sizey);
    edited.draw(&w);
    sizex -= (posx - 1);
    sizey -= (posy - 1);
    if (didit) {
     std::string name = string_input_popup("Name element:");
     char sel = popup_getkey("Selectable?");
     bool selectable = (sel == 'y' || sel == 'Y');
     edited.add_element(type, name, posx, posy, sizex, sizey, selectable);
     edited.draw(&w);
    }
   } break; // 'n' 'N'
  } // switch (ch)

  edited.draw(&w);

 } while (!done);

 std::ofstream fout;
 fout.open("editor.nci");
 if (fout.is_open()) {
  fout << edited.save_data();
  fout.close();
 }
 return 0;
}

bool pick_point(Window &w, int &x, int &y)
{
 x = 0;
 y = 0;
 char ch;
 do {
  glyph orig = w.glyphat(x, y);
  w.putch(x, y, c_white, c_black, 'x');
  do {
   ch = input();
  } while (ch != 'j' && ch != 'k' && ch != 'l' && ch != 'h' && ch != 'y' &&
           ch != 'u' && ch != 'b' && ch != 'n' && ch != KEY_ESC && ch != '\n');

  if (ch == KEY_ESC)
   return false;

  int movex = 0, movey = 0;
  if (ch == 'y' || ch == 'u' || ch == 'k')
   movey = -1;
  if (ch == 'j' || ch == 'b' || ch == 'n')
   movey = 1;
  if (ch == 'h' || ch == 'y' || ch == 'b')
   movex = -1;
  if (ch == 'l' || ch == 'u' || ch == 'n')
   movex = 1;

  if (movex != 0 || movey != 0) {
   w.putch(x, y, orig.fg, orig.bg, orig.symbol);
   x += movex;
   y += movey;
  }
 } while (ch != '\n');
 return true;
}
   
