#include <fstream>
#include <sstream>
#include "interface.h"

using namespace cuss;

bool pick_point(Window &w, int &x, int &y);
void init_interface(interface &edited);
void draw_line(interface &ncint, int x1, int y1, int x2, int y2);
void temp_line(Window &w, int x1, int y1, int x2, int y2);
void draw_box (interface &ncint, int x1, int y1, int x2, int y2);
void temp_box (Window &w, int x1, int y1, int x2, int y2);
void paint(interface &ncint, int x, int y);
void fix_lines(interface &ncint);
bool is_line(long ch);

void help();
void set_pen_symbol();
void set_pen_fg();
void set_pen_bg();
nc_color pick_color();

enum draw_mode {
DM_NULL = 0,
DM_DRAW,
DM_TYPE,
DM_LINE,
DM_BOX,
DM_OBJECT,
DM_MAX
};

glyph pen;

int main()
{ 
 init_display();
 int sizex = 80, sizey = 25;
 cuss::interface edited;
 Window w(0, 0, sizex, sizey);

 init_interface(edited);

 draw_mode dm = DM_NULL;

 bool done = false, blink = true;
 int posx = 0, posy = 0, bufx = -1, bufy = -1;
 pen = glyph('x', c_white, c_black);

 do {
  if (dm == DM_DRAW)
   paint(edited, posx, posy);

  edited.draw_prototype(&w);
  glyph gl_orig = w.glyphat(posx, posy);
  if (blink) {
   if (gl_orig == pen) {
    glyph tmp(pen.symbol, pen.fg, hilight(pen.bg));
    w.putglyph(posx, posy, tmp);
   } else
    w.putglyph(posx, posy, pen);
  }

  if (dm == DM_LINE)
   temp_line(w, bufx, bufy, posx, posy);
  if (dm == DM_BOX || dm == DM_OBJECT)
   temp_box(w, bufx, bufy, posx, posy);

  w.refresh();
  timeout(200);
  long ch = getch();
  timeout(-1);

  if (ch == ERR)
   blink = !blink;
  else {
   blink = true;
   if (dm == DM_TYPE) {
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
    } else if (ch == KEY_BACKSPACE || ch == 127) {
     edited.set_data("BG", glyph(-1, c_black, c_black), posx, posy);
     if (posx > 0) posx--;
    } else {
     pen.symbol = ch;
     edited.set_data("BG", pen, posx, posy);
     pen.symbol = '_';
     if (posx < sizex - 1) posx++;
    }
   } else { // Not typing.
 
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
    } else if (ch == '?') {
     help();
    } else if (ch == 'c' || ch == 'C') {
     pen = gl_orig;
    } else if (ch == '\'') {
     set_pen_symbol();
    } else if (ch == '"') {
     pen.symbol = LINE_XXXX;
    } else if (ch == '[') {
     set_pen_fg();
    } else if (ch == ']') {
     set_pen_bg();
    } else if (ch == 'i' || ch == 'I') {
     dm = DM_TYPE;
     pen.symbol = '_';
    } else if (ch == ';') {
     bufx = posx; bufy = posy;
     dm = DM_LINE;
    } else if (ch == ':') {
     bufx = posx; bufy = posy;
     dm = DM_BOX;
    } else if (ch == '>') {
     dm = DM_DRAW;
    } else if (ch == '.') {
     paint(edited, posx, posy);
    } else if (ch == 'x') {
     edited.set_data("BG", glyph(-1, c_black, c_black), posx, posy);
    } else if (ch == '/') {
     fix_lines(edited);
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
 
      case DM_DRAW:
       if (bufx != -1 && bufy != -1 && posy < sizey - 1) {
        posx = bufx;
        posy++;
       }
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
       dm = DM_NULL;
       switch (menu("Element type:", "Drawing", "Text", "List", "Text Entry",
                                     "Number", NULL)) {
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
   } // Not typing
  } // ch != ERR

 } while (!done);

 std::ofstream fout;
 fout.open("editor.cuss");
 if (fout.is_open()) {
  fout << edited.save_data();
  fout.close();
 }

 endwin();
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
 fin.open("editor.cuss");
 if (fin.is_open()) {
  edited.load_data(fin);
  fin.close();
 } else {
  edited.name = "cuss_editor";
  edited.add_element(ELE_DRAWING, "BG", 0, 0, 80, 25, false);
 }
}

void draw_line(interface &ncint, int x1, int y1, int x2, int y2)
{
 int xdir = 0, ydir = 0;
 long sym = '*';
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
 glyph gl(sym, pen.fg, pen.bg);
 do {
  ncint.set_data("BG", gl, x, y);
  x += xdir;
  y += ydir;
 } while (x != x2 || y != y2);
 ncint.set_data("BG", gl, x, y);
}

void temp_line(Window &w, int x1, int y1, int x2, int y2)
{
 int xdir = 0, ydir = 0;
 long sym = '*';
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
  return;
 }

 int x = x1, y = y1;
 glyph gl(sym, c_magenta, c_blue);
 do {
  w.putglyph(x, y, gl);
  x += xdir;
  y += ydir;
 } while (x != x2 || y != y2);
 w.putglyph(x, y, gl);
}

void draw_box (interface &ncint, int x1, int y1, int x2, int y2)
{
 if (x1 == x2 && y1 == y2)
  return;

 if (x1 > x2) {
  int buf = x2;
  x2 = x1;
  x1 = buf;
 }
 if (y1 > y2) {
  int buf = y2;
  y2 = y1;
  y1 = buf;
 }

 if (x1 + 1 < x2) {
  draw_line(ncint, x1, y1, x2, y1);
  draw_line(ncint, x1, y2, x2, y2);
 }
 if (y1 + 1 < y2) {
  draw_line(ncint, x1, y1, x1, y2);
  draw_line(ncint, x2, y1, x2, y2);
 }

 ncint.set_data("BG", glyph(LINE_OXXO, pen.fg, pen.bg), x1, y1);
 ncint.set_data("BG", glyph(LINE_OOXX, pen.fg, pen.bg), x2, y1);
 ncint.set_data("BG", glyph(LINE_XXOO, pen.fg, pen.bg), x1, y2);
 ncint.set_data("BG", glyph(LINE_XOOX, pen.fg, pen.bg), x2, y2);
}

void temp_box (Window &w, int x1, int y1, int x2, int y2)
{
 if (x1 == x2 && y1 == y2)
  return;

 if (x1 > x2) {
  int buf = x2;
  x2 = x1;
  x1 = buf;
 }
 if (y1 > y2) {
  int buf = y2;
  y2 = y1;
  y1 = buf;
 }

 w.putglyph(x1, y1, glyph(LINE_OXXO, c_magenta, c_blue));
 w.putglyph(x2, y1, glyph(LINE_OOXX, c_magenta, c_blue));
 w.putglyph(x1, y2, glyph(LINE_XXOO, c_magenta, c_blue));
 w.putglyph(x2, y2, glyph(LINE_XOOX, c_magenta, c_blue));

 if (x1 + 1 < x2) {
  temp_line(w, x1 + 1, y1, x2 - 1, y1);
  temp_line(w, x1 + 1, y2, x2 - 1, y2);
 }
 if (y1 + 1 < y2) {
  temp_line(w, x1, y1 + 1, x1, y2 - 1);
  temp_line(w, x2, y1 + 1, x2, y2 - 1);
 }
}

void paint(interface &ncint, int x, int y)
{
 ncint.set_data("BG", pen, x, y);
 if (pen.symbol == LINE_XXXX)
  fix_lines(ncint);
}

void fix_lines(interface &ncint)
{
 element* ele = ncint.find_by_name("BG");
 ele_drawing* bg = static_cast<ele_drawing*>(ele);
 if (!bg)
  return;
 std::map<point, glyph>::iterator it;
 for (it = bg->drawing.begin(); it != bg->drawing.end(); it++) {
  point north(it->first.x    , it->first.y - 1);
  point  east(it->first.x + 1, it->first.y    );
  point south(it->first.x    , it->first.y + 1);
  point  west(it->first.x - 1, it->first.y    );
  if (is_line(it->second.symbol)) {
   if (is_line( bg->drawing[north].symbol)) {
    if (is_line( bg->drawing[east].symbol)) {
     if (is_line( bg->drawing[south].symbol)) {
      if (is_line( bg->drawing[west].symbol))
       it->second.symbol =LINE_XXXX;
      else
       it->second.symbol =LINE_XXXO;
     } else {
      if (is_line( bg->drawing[west].symbol))
       it->second.symbol =LINE_XXOX;
      else
       it->second.symbol =LINE_XXOO;
     }
    } else {
     if (is_line( bg->drawing[south].symbol)) {
      if (is_line( bg->drawing[west].symbol))
       it->second.symbol =LINE_XOXX;
      else
       it->second.symbol =LINE_XOXO;
     } else {
      if (is_line( bg->drawing[west].symbol))
       it->second.symbol =LINE_XOOX;
     }
    }
   } else {
    if (is_line( bg->drawing[east].symbol)) {
     if (is_line( bg->drawing[south].symbol)) {
      if (is_line( bg->drawing[west].symbol))
       it->second.symbol =LINE_OXXX;
      else
       it->second.symbol =LINE_OXXO;
     } else {
      it->second.symbol =LINE_OXOX;
     }
    } else {
     if (is_line( bg->drawing[south].symbol)) {
      if (is_line( bg->drawing[west].symbol))
       it->second.symbol =LINE_OOXX;
     } else {
      it->second.symbol =LINE_OXOX;
     }
    }
   }
  }
 }
}

bool is_line(long ch)
{
 return (ch == LINE_OOXX || ch == LINE_OXOX || ch == LINE_OXXO ||
         ch == LINE_OXXX || ch == LINE_XOOX || ch == LINE_XOXO ||
         ch == LINE_XOXX || ch == LINE_XXOO || ch == LINE_XXOX ||
         ch == LINE_XXXO || ch == LINE_XXXX);
}

void help()
{
 popup("\
S     Save & quit\n\
-     Open element browser\n\
Enter Create element\n\
\"     Start freeform line drawing\n\
i     enter type mode\n\
>     enter draw mode\n\
;     Draw line\n\
:     Draw box\n\
Esc   Exit type/draw mode or cancel object, box or line\n\
.     place current symbol\n\
x     delete drawing under cursor\n\
/     Fix lines\n\
c     Copy symbol under pen\n\
'     Set pen symbol\n\
[]    Set foreground / background color\n\
");
}

void set_pen_symbol()
{
 long ch = getch();
 if (ch != ' ')
  pen.symbol = ch;
}

void set_pen_fg()
{
 nc_color tmp = pick_color();
 if (tmp != c_null)
  pen.fg = pick_color();
}

void set_pen_bg()
{
 nc_color tmp = pick_color();
 if (tmp != c_null)
  pen.bg = pick_color();
}

nc_color pick_color()
{
 std::stringstream text;
 Window w_col(1, 1, 20, 6);
 w_col.outline();
 for (int i = 0; i < c_dkgray; i++) {
  w_col.putch(i + 1, 1, nc_color(i), c_black, '#');
  w_col.putch(i + 1, 3, nc_color(i + 8), c_black, '#');
 }
 w_col.putstr(1, 2, c_white, c_black, "12345678");
 w_col.putstr(1, 4, c_white, c_black, "abcdefgh");

 w_col.refresh();
 long ch = getch();

 if (ch >= '1' && ch <= '8')
  return nc_color(ch - '1');
 if (ch >= 'a' && ch <= 'h')
  return nc_color(ch - 'a' + c_dkgray);

 return c_null;
}
