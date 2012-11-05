#ifndef _NCI_H_
#define _NCI_H_

#include <string>
#include <vector>
#include <istream>
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
    std::string name;
    int posx;
    int posy;
    int sizex;
    int sizey;
    bool selected;
    bool selectable;

    element() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                selected = false; selected = false; };

    virtual element_type type() { return ELE_NULL; };
    virtual void draw(Window *win) {};

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool set_data(std::string data) { return false; };
    virtual bool add_data(std::string data) { return false; };

    virtual bool set_data(std::vector<std::string> data) { return false; };
    virtual bool add_data(std::vector<std::string> data) { return false; };

    virtual bool set_data(int data) { return false; };
    virtual bool add_data(int data) { return false; };

    virtual bool set_data(glyph gl, int posx, int posy) { return false; };

  };

  struct ele_drawing : public element
  {
    std::vector<glyph> drawing;

    ele_drawing() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                    selected = false; selected = false; };

    virtual element_type type() { return ELE_DRAWING; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool set_data(glyph gl, int posx, int posy);
  };

  struct ele_textbox : public element
  {
    std::vector<std::string> text;
    int offset;

    ele_textbox() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                    selected = false; selected = false; offset = 0; };

    virtual element_type type() { return ELE_TEXTBOX; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

/* We store this as a vector because the text needs to be split into seperate
 * lines.  It's more efficient to do this once, when the text is stored, than
 * every time we print.
 */
    virtual bool set_data(std::string data);
    virtual bool add_data(std::string data);

    virtual bool set_data(std::vector<std::string> data);
    virtual bool add_data(std::vector<std::string> data);
  };

  struct ele_list : public element
  {
    std::vector<std::string> list;
    int offset;
    int selection;

    ele_list() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                 selected = false; selected = false; offset = 0;
                 selection = 0; };

    virtual element_type type() { return ELE_LIST; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool add_data(std::string data);

    virtual bool set_data(std::vector<std::string> data);
    virtual bool add_data(std::vector<std::string> data);
  };

  struct ele_textentry : public element
  {
    std::string text;

    ele_textentry() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                      selected = false; selected = false; text = ""; };

    virtual element_type type() { return ELE_TEXTENTRY; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool set_data(std::string data);
    virtual bool add_data(std::string data);
  };

  struct ele_number : public element
  {
    int value;

    ele_number() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                   selected = false; selected = false; value = 0; };
    virtual element_type type() { return ELE_NUMBER; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool set_data(int data);
    virtual bool add_data(int data);
  };
      
  class interface
  {
   public:
    interface();
    void add_element(element_type type, std::string name, int posx, int posy,
                     int sizex, int sizey, bool selectable = true);
    void draw(Window *win);
    std::string save_data();
    void load_data(std::istream &datastream);

    element* selected();
    element* find_by_name(std::string name);

    void select_next();
    void select_last();
    bool select(std::string name);
// set_data replaces the element's data with whatever is passed
// add_data appends whatever is passed to the element's data
// These are all defined for each element type; if they're invalid, the type
//  just returns false.
    bool set_data(std::string name, std::string data);
    bool add_data(std::string name, std::string data);

    bool set_data(std::string name, std::vector<std::string> data);
    bool add_data(std::string name, std::vector<std::string> data);

    bool set_data(std::string name, int data);
    bool add_data(std::string name, int data);

    bool set_data(std::string name, glyph gl, int posx, int posy);

    std::string name;

   private:
    int active_element;
    std::vector<element> elements;
  };

}; // namespace nci

#endif
