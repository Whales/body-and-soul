#include <fstream>
#include "interface.h"

using namespace nci;

bool pick_point(Window &w, int &x, int &y);
void init_interface(interface &edited);
void draw_line(interface &ncint, int x1, int y1, int x2, int y2);
void draw_box (interface &ncint, int x1, int y1, int x2, int y2);

enum draw_mode {
DM_NULL = 0,
DM_DRAW,
DM_LINE,
DM_BOX,
DM_OBJECT,
DM_MAX
};

int main()
{ 
 init_display();
 timeout(500);
 int sizex = 80, sizey = 25;
 nci::interface edited;
 Window w(0, 0, sizex, sizey);

 init_interface(edited);
 edited.draw(&w);

 draw_mode dm = DM_NULL;

 bool done = false, blink = true;
 int posx = 0, posy = 0, bufx = -1, bufy = -1;
 glyph pen('x', c_white, c_black);

 do {

  edited.draw(&w);
  if (blink) {
   glyph gl_orig = w.glyphat(posx, posy);
   if (gl_orig == pen) {
    glyph tmp(pen.symbol, pen.fg, hilight(pen.bg));
    w.putglyph(posx, posy, tmp);
   } else
    w.putglyph(posx, posy, pen);
  }

  long ch = getch();
  if (ch == ERR)
   blink = !blink;

  else {
   blink = true;
   if (dm == DM_DRAW) {
    if (ch == '\n') {
     if (bufx != -1 && bufy != -1 && posy < sizey - 1) {
      posx = bufx;
      posy++;
     }
    } else if (ch == KEY_ESC)
     dm = DM_NULL;
    else if (ch == KEY_LEFT) {
     if (posx > 0) posx--;
    } else if (ch == KEY_RIGHT) {
     if (posx < sizex - 1) posx++;
    } else if (ch == KEY_UP) {
     if (posy > 0) posy--;
    } else if (ch == KEY_DOWN) {
     if (posy < sizey - 1) posy++;
    } else {
     pen.symbol = ch;
     edited.set_data("BG", pen, posx, posy);
    }
   } else { // Not DM_DRAW
    int movex = 0, movey = 0;
    if (ch == 'y' || ch == 'h' || ch == 'b' || ch == '7' || ch == '4' ||
        ch == '1' || ch == KEY_LEFT)
     movex = -1;
    if (ch == 'u' || ch == 'l' || ch == 'n' || ch == '9' || ch == '6' ||
        ch == '3' || ch == KEY_RIGHT)
     movex = 1;
    if (ch == 'y' || ch == 'k' || ch == 'u' || ch == '7' || ch == '8' ||
        ch == '9' || ch == KEY_UP)
     movey = -1;
    if (ch == 'b' || ch == 'j' || ch == 'n' || ch == '1' || ch == '2' ||
        ch == '3' || ch == KEY_DOWN)
     movey = 1;

    if (movex != 0 || movey != 0) {
     posx += movex;
     if (posx < 0) posx = 0;
     if (posx >= sizex) posx = sizex - 1;
     posy += movey;
     if (posy < 0) posy = 0;
     if (posy >= sizey) posy = sizey - 1;
    } else if (ch == 'S' || ch == 's') {
     char quitconf = popup_getkey("Quit & Save?");
     if (quitconf == 'y' || quitconf == 'Y' || quitconf == 's' ||
         quitconf == 'S')
      done = true;
    } else if (ch == '\n') {
     switch (dm) {
      case DM_NULL:
       dm = DM_OBJECT;
       bufx = posx;
       bufy = posy;
       break;

      case DM_LINE:
       draw_line(edited, bufx, bufy, posx, posy);
       dm = DM_NULL;
       bufx = -1;
       bufy = -1;
       break;

      case DM_BOX:
       draw_box(edited, bufx, bufy, posx, posy);
       dm = DM_NULL;
       bufx = -1;
       bufy = -1;
       break;

      case DM_OBJECT: {
       element_type type = ELE_NULL;
       switch (menu("Element type:", "Drawing", "Text", "List", "Text Entry",
                                     "Number", 0)) {
        case 1: type = ELE_DRAWING;   break;
        case 2: type = ELE_TEXTBOX;   break;
        case 3: type = ELE_LIST;      break;
        case 4: type = ELE_TEXTENTRY; break;
        case 5: type = ELE_NUMBER;    break;
       }
       int sizex = posx - bufx + 1;
       int sizey = posy - bufy + 1;
       std::string name = string_input_popup("Name element:");
       char sel = popup_getkey("Selectable?");
       bool selectable = (sel == 'y' || sel == 'Y');
       edited.add_element(type, name, bufx, bufy, sizex, sizey, selectable);
       edited.draw(&w);
       bufx = -1;
       bufy = -1;
      } break;
     } // switch (dm)
    } else if (ch == KEY_ESC) {
     if (dm != DM_NULL) {
      dm = DM_NULL;
      if (bufx != -1 && bufy != -1) {
       posx = bufx;
       posy = bufy;
      }
     }
    }
   } // !DM_DRAW
  } // ch != ERR

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

void init_interface(interface &edited)
{
 std::ifstream fin;
 fin.open("editor.nci");
 if (fin.is_open()) {
  edited.load_data(fin);
  fin.close();
 } else {
  edited.name = "nci_editor";
  edited.add_element(ELE_DRAWING, "BG", 0, 0, 80, 25, false);
 }
}

void draw_line(interface &ncint, int x1, int y1, int x2, int y2)
{
 int xdir = 0, ydir = 0;
 char sym = '*';
 if (x1 == x2 && y1 == y2)
  return;

 if (x1 == x2) {
  sym = LINE_XOXO;
  ydir = (y2 > y1 ? 1 : -1);
 } else if (y1 == y2) {
  sym = LINE_OXOX;
  xdir = (x2 > x1 ? 1 : -1);
 } else if (y2 - y1 == x2 - x1) {
  sym = '\\';
  xdir = (x2 > x1 ? 1 : -1);
  ydir = (y2 > y1 ? 1 : -1);
 } else if (y2 - y1 == x1 - x2) {
  sym = '/';
  xdir = (x2 > x1 ? 1 : -1);
  ydir = (y2 > y1 ? 1 : -1);
 } else {
  debugmsg("No Bresenham here!");
  return;
 }

 int x = x1, y = y1;
 glyph gl(sym, 
 do {
  ncint.add_data("BG", 
void draw_box (interface &ncint, int x1, int y1, int x2, int y2);
