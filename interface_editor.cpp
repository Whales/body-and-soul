#include <fstream>
#include "interface.h"

int main()
{ 
 init_display();
 nci::interface edited;

 std::ifstream fin;
 fin.open("editor.nci");
 if (fin.is_open())
  edited.load_data(fin);
 else
  edited.name = "nci_editor";

 bool done = false;

 do {
  long ch = input();
  switch (ch) {

   case 'N': case 'n': {
    element_type type = ELE_NULL;
    switch (menu("Element type:", "Drawing", "Text", "List", "Text Entry",
                                  "Number")) {
     case 1: type = ELE_DRAWING;   break;
     case 2: type = ELE_TEXTBOX;   break;
     case 3: type = ELE_LIST;      break;
     case 4: type = ELE_TEXTENTRY; break;
     case 5: type = ELE_NUMBER;    break;
    }
    int posx, posy, sizex, sizey;
    debugmsg("Pick starting point");
    pick_point(posx, posy);
    debugmsg("Pick end point");
    pick_point(sizex, sizey);
    sizex -= (posx - 1);
    sizey -= (posy - 1);
    
 } while (!done);
