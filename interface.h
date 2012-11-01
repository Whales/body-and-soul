#ifndef _NCI_H_
#define _NCI_H_

#include <string>
#include <vector>
#include "window.h"

namespace nci {

  enum element_type
  {
    ELE_NULL = 0, // Nothing
    ELE_DRAWING,  // Plain characters / text
    ELE_TEXTBOX,  // Scrollable text
    ELE_LIST,     // Scrollable list w/ selection
    ELE_TEXTENTRY,// Type to enter text
    ELE_NUMBER,   // Number to select
    ELE_MAX
  };

  struct element
  {
    virtual element_type type() { return ELE_NULL; };
    virtual void draw(Window *win) {};

    std::string name;
    int posx;
    int posy;
    int sizex;
    int sizey;
    bool selected;
    bool selectable;
  };

  struct ele_drawing : public element
  {
    virtual element_type type() { return ELE_DRAWING; };
    virtual void draw(Window *win);

    std::vector<glyph> drawing;
  };

  struct ele_textbox : public element
  {
    virtual element_type type() { return ELE_TEXTBOX; };
    virtual void draw(Window *win);

/* We store this as a vector because the text needs to be split into seperate
 * lines.  It's more efficient to do this once, when the text is stored, than
 * every time we print.
 */
    std::vector<std::string> text;
    int offset;
  };

  struct ele_list : public element
  {
    virtual element_type type() { return ELE_LIST; };
    virtual void draw(Window *win);

    std::vector<std::string> list;
    int offset;
    int selection;
  };

  struct ele_textentry : public element
  {
    virtual element_type type() { return ELE_TEXTENTRY; };
    virtual void draw(Window *win);

    std::string text;
  };

  struct ele_number : public element
  {
    virtual element_type type() { return ELE_NUMBER; };
    virtual void draw(Window *win);

    int value;
  };
      
  class interface
  {
   public:
// set_data replaces the element's data with whatever is passed
// add_data appends whatever is passed to the element's data
// For text entry and text boxes
    bool set_data(std::string name, std::string data);
    bool add_data(std::string name, std::string data);
// For list selections
    bool set_data(std::string name, std::vector<std::string> data);
    bool add_data(std::string name, std::vector<std::string> data);
// For sliders and number pickers
// In this case, add_data literally adds it (i.e. + operator).
    bool set_data(std::string name, int data);
    bool add_data(std::string name, int data);

    void add_element(element_type type, std::string name, int posx, int posy,
                     int sizex, int sizey, bool selectable = true);
    element* selected();
    void select_next();
    void select(std::string name);
   private:
    int active_element;
    std::vector<element> elements;
  };

}; // namespace nci

#endif
