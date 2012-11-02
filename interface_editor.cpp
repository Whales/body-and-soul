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

 bool done = false;
 do {
  long ch = input();
  switch (ch) {
   case 'N': case 'n':
    
 } while (!done);
