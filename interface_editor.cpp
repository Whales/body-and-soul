#include <fstream>
#include <sstream>
#include "interface.h"
#include "files.h"

using namespace cuss;

void starting_window(interface &edited);
void elements_window(interface &edited);

void init_interface(interface &edited, std::string name);
void draw_line(interface &edited, int x1, int y1, int x2, int y2);
void temp_line(Window &w, int x1, int y1, int x2, int y2);
void draw_box (interface &edited, int x1, int y1, int x2, int y2);
void temp_box (Window &w, int x1, int y1, int x2, int y2);
void paint(interface &edited, int x, int y);
void fix_lines(interface &edited, std::string name);
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
DM_ELEMENT,
DM_MOVE_ELE,
DM_RESIZE_ELE,
DM_MAX
};

glyph pen;

int main()
{ 
 init_display();
 cuss::interface edited;

 starting_window(edited);

 int sizex = 80, sizey = 24;
 edited.sizex = sizex;
 edited.sizey = sizey;
 Window w(0, 0, sizex, sizey);

 draw_mode dm = DM_NULL;

 bool done = false, blink = true;
 int posx = 0, posy = 0, bufx = -1, bufy = -1;
 pen = glyph('x', c_white, c_black);

 do {
  if (dm == DM_DRAW)
   paint(edited, posx, posy);

  edited.draw_prototype(&w);
  glyph gl_orig = w.glyphat(posx, posy);
  element* sel = edited.selected();

  if (blink) {
   if (dm == DM_MOVE_ELE)
     w.putglyph(sel->posx, sel->posy, glyph(LINE_OXXO, c_pink, c_black));
   else if (dm == DM_RESIZE_ELE)
     w.putglyph(sel->posx + sel->sizex - 1, sel->posy + sel->sizey - 1,
                glyph(LINE_XOOX, c_pink, c_black));
   else {
     if (gl_orig == pen) {
       glyph tmp(pen.symbol, pen.fg, hilight(pen.bg));
       w.putglyph(posx, posy, tmp);
     } else
       w.putglyph(posx, posy, pen);
   }
  }

  if (dm == DM_LINE)
   temp_line(w, bufx, bufy, posx, posy);
  if (dm == DM_BOX || dm == DM_ELEMENT)
   temp_box(w, bufx, bufy, posx, posy);

  w.refresh();
  timeout((blink ? 300 : 150));
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

     if (dm == DM_MOVE_ELE && sel) {
      sel->posx += movex;
      if (sel->posx < 0)
       sel->posx = 0;
      if (sel->posx + sel->sizex - 1 >= sizex)
       sel->posx = sizex - sel->sizex;
      sel->posy += movey;
      if (sel->posy < 0)
       sel->posy = 0;
      if (sel->posy + sel->sizey - 1 >= sizey)
       sel->posy = sizey - sel->sizey;

     } else if (dm == DM_RESIZE_ELE && sel) {
      sel->sizex += movex;
      if (sel->sizex < 1)
       sel->sizex = 1;
      if (sel->posx + sel->sizex - 1 >= sizex)
       sel->sizex = sizex - sel->posx;
      sel->sizey += movey;
      if (sel->sizey < 1)
       sel->sizey = 1;
      if (sel->posy + sel->sizey - 1 >= sizey)
       sel->sizey = sizey - sel->posy;

     } else { // Normal cursor movement
      posx += movex;
      if (posx < 0) posx = 0;
      if (posx >= sizex) posx = sizex - 1;
      posy += movey;
      if (posy < 0) posy = 0;
      if (posy >= sizey) posy = sizey - 1;
     }

    } else if (ch == '?') {
     help();

    } else if (ch == '<') {
     edited.select_last(true);
     sel = edited.selected();

    } else if (ch == '>') {
     edited.select_next(true);
     sel = edited.selected();

    } else if (ch == 'm' && sel) {
     dm = DM_MOVE_ELE;
     bufx = sel->posx; bufy = sel->posy;

    } else if (ch == 'r' && sel) {
     dm = DM_RESIZE_ELE;
     bufx = sel->sizex; bufy = sel->sizey;

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
     bufx = posx; bufy = posy;

    } else if (ch == ';') {
     bufx = posx; bufy = posy;
     dm = DM_LINE;

    } else if (ch == ':') {
     bufx = posx; bufy = posy;
     dm = DM_BOX;

    } else if (ch == ',') {
     dm = DM_DRAW;

    } else if (ch == '.') {
     paint(edited, posx, posy);

    } else if (ch == 'x') {
     edited.set_data("BG", glyph(-1, c_black, c_black), posx, posy);

    } else if (ch == '/') {
     fix_lines(edited, "BG");

    } else if (ch == 'S' || ch == 's') {
     char quitconf = popup_getkey("Quit & Save?");
     if (quitconf == 'y' || quitconf == 'Y' || quitconf == 's' ||
         quitconf == 'S')
      done = true;

    } else if (ch == '\n') {
     switch (dm) {
      case DM_NULL:
       dm = DM_ELEMENT;
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
 
      case DM_ELEMENT: {
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
       int x1, y1;
       if (bufx < posx)
        x1 = bufx;
       else
        x1 = posx;
       if (bufy < posy)
        y1 = bufy;
       else
        y1 = posy;
       std::string name = string_input_popup("Name element:");
       char sel = popup_getkey("Selectable?");
       bool selectable = (sel == 'y' || sel == 'Y');
       edited.add_element(type, name, x1, y1, sizex, sizey, selectable);
       edited.draw(&w);
       bufx = -1;
       bufy = -1;
      } break;

      case DM_MOVE_ELE:
      case DM_RESIZE_ELE:
       dm = DM_NULL;
       break;
     } // switch (dm)
    } else if (ch == KEY_ESC) {
     if (sel) {
      if (dm == DM_MOVE_ELE && bufx != -1 && bufy != -1) {
       sel->posx = bufx;
       sel->posy = bufy;
      }
      if (dm == DM_RESIZE_ELE && bufx != -1 && bufy != -1) {
       sel->sizex = bufx;
       sel->sizey = bufy;
      }
      bufx = -1;
      bufy = -1;
      edited.select_none();
      sel = edited.selected();
     } else {
      edited.select_none();
      sel = edited.selected();
     }

     if (dm != DM_NULL) {
      dm = DM_NULL;
      if (bufx != -1 && bufy != -1) {
       posx = bufx;
       posy = bufy;
       bufx = -1;
       bufy = -1;
      }
     }
    }
   } // Not typing
  } // ch != ERR

 } while (!done);

 std::ofstream fout;
 std::stringstream foutname;
 foutname << "cuss/" << edited.name << ".cuss";
 std::string fname = foutname.str();
 fout.open(fname.c_str());
 if (fout.is_open()) {
  fout << edited.save_data();
  fout.close();
 } else
  popup("Couldn't open %s for saving", fname.c_str());

 endwin();
 return 0;
}

void starting_window(interface &edited)
{
 Window w_start(0, 0, 80, 24);
 cuss::interface i_start;
 cuss::interface selected;

 if (!i_start.load_from_file("cuss/i_start.cuss"))
  debugmsg("Couldn't load starting interface!");

 i_start.set_data("list_interfaces", files_in("cuss", "cuss"));
 std::string selname = i_start.get_str("list_interfaces");

 if (selname != "") {
  std::stringstream filename;
  filename << "cuss/" << selname;
  selected.load_from_file(filename.str());
  i_start.set_data("list_elements", selected.element_names());
 }
 i_start.select("list_interfaces");

 bool done = false;
 while (!done) {
  i_start.draw(&w_start);
  w_start.refresh();
  long ch = getch();
  if (ch == 'j' || ch == '2' || ch == KEY_DOWN) {
   i_start.add_data("list_interfaces",  1);
   selname = i_start.get_str("list_interfaces");
   if (selname != "") {
    std::stringstream filename;
    filename << "cuss/" << selname;
    selected.load_from_file(filename.str());
    i_start.set_data("list_elements", selected.element_names());
   }
  }
  if (ch == 'k' || ch == '8' || ch == KEY_UP) {
   i_start.add_data("list_interfaces", -1);
   selname = i_start.get_str("list_interfaces");
   if (selname != "") {
    std::stringstream filename;
    filename << "cuss/" << selname;
    selected.load_from_file(filename.str());
    i_start.set_data("list_elements", selected.element_names());
   }
  }
  if (ch == 'l' || ch == 'L' || ch == '\n') {
   done = true;
   std::stringstream filename;
   filename << "cuss/" << i_start.get_str("list_interfaces");
   edited.load_from_file(filename.str());
  }
  if (ch == 'n' || ch == 'N') {
   std::string name = string_input_popup("Name: ");
   if (name != "") {
    done = true;
    init_interface(edited, name);
   }
  }
 }
}

void elements_window(interface &edited)
{
 Window w_elements(0, 0, 80, 24);

 cuss::interface i_ele;
 if (!i_ele.load_from_file("cuss/i_elements.cuss")) }
  debugmsg("Couldn't load cuss/i_elements.cuss!");
  return;
 }

 i_ele.set_data("e_elelist", edited.element_names());
 edited.select_by_name("BG");
 element* selected = edited.selected();
 do {
  if (selected) {
   i_ele.set_data("e_elename", selected->name);
   i_ele.set_data("e_eletype", element_type_name(selected->type()) );
   i_ele.set_data("e_editable", (selected->editable ? "Yes" : "No"));
  }
 } while (true);
}

void init_interface(interface &edited, std::string name)
{
 std::stringstream filename;
 filename << "cuss/" << name << ".cuss";
 std::ifstream fin;
 fin.open(filename.str().c_str());
 if (fin.is_open()) {
  edited.load_data(fin);
  fin.close();
 } else {
  edited.name = name;
  edited.sizex = 80;
  edited.sizey = 25;
  edited.add_element(ELE_DRAWING, "BG", 0, 0, 80, 25, false);
 }
}

void draw_line(interface &edited, int x1, int y1, int x2, int y2)
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
  edited.set_data("BG", gl, x, y);
  x += xdir;
  y += ydir;
 } while (x != x2 || y != y2);
 edited.set_data("BG", gl, x, y);
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

void draw_box (interface &edited, int x1, int y1, int x2, int y2)
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

 if (is_line(pen.symbol)) {
  if (x1 + 1 < x2) {
   draw_line(edited, x1, y1, x2, y1);
   draw_line(edited, x1, y2, x2, y2);
  }
  if (y1 + 1 < y2) {
   draw_line(edited, x1, y1, x1, y2);
   draw_line(edited, x2, y1, x2, y2);
  }

  edited.set_data("BG", glyph(LINE_OXXO, pen.fg, pen.bg), x1, y1);
  edited.set_data("BG", glyph(LINE_OOXX, pen.fg, pen.bg), x2, y1);
  edited.set_data("BG", glyph(LINE_XXOO, pen.fg, pen.bg), x1, y2);
  edited.set_data("BG", glyph(LINE_XOOX, pen.fg, pen.bg), x2, y2);

 } else { // Not a line-drawing box; so draw & fill
  for (int x = x1; x <= x2; x++) {
   for (int y = y1; y <= y2; y++)
     paint(edited, x, y);
  }
 }
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

void paint(interface &edited, int x, int y)
{
 edited.set_data("BG", pen, x, y);
 if (pen.symbol == LINE_XXXX)
  fix_lines(edited, "BG");
}

void fix_lines(interface &edited, std::string name)
{
 element* ele = edited.find_by_name(name);
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
<>    Select last / next element\n\
m     Move element\n\
r     Resize element\n\
i     Enter typing mode\n\
,     Enter drawing mode\n\
;     Draw line\n\
:     Draw box\n\
Esc   Cancel drawing, unselect element, exist typing/drawing mode\n\
.     place current symbol\n\
x     delete drawing under cursor\n\
/     Fix lines\n\
'     Set pen symbol\n\
\"     Set pen symbol to line drawings\n\
c     Copy symbol & colors under pen\n\
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
  pen.fg = tmp;
}

void set_pen_bg()
{
 nc_color tmp = pick_color();
 if (tmp != c_null)
  pen.bg = tmp;
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
