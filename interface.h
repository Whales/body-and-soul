#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <string>
#include <vector>
#include <istream>
#include <map>
#include "window.h"
#include "geometry.h"

namespace cuss {

  enum element_type
  {
    ELE_NULL = 0, // Nothing
    ELE_DRAWING,  // Plain characters / text
    ELE_TEXTBOX,  // Scrollable text
    ELE_LIST,     // Scrollable list w/ selection
    ELE_TEXTENTRY,// Type to enter text
    ELE_NUMBER,   // Number to select
    ELE_MENU,     // Drop-down menu
    ELE_MAX
  };

  std::string element_type_name(element_type type);

  struct element
  {
    std::string name;
    int posx;
    int posy;
    int sizex;
    int sizey;
    bool selected;
    bool selectable;
    bool owns_data;
    nc_color fg;
    nc_color bg;

    element() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                selected = false; selectable = false;
                fg = c_white; bg = c_black; owns_data = true; }

    virtual element_type type() { return ELE_NULL; };
    virtual void draw(Window *win) {};

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool self_reference() { return false; };

    virtual bool set_data(std::string  data) { return false; };
    virtual bool add_data(std::string  data) { return false; };
    virtual bool ref_data(std::string *data) { return false; };

    virtual bool set_data(std::vector<std::string>  data) { return false; };
    virtual bool add_data(std::vector<std::string>  data) { return false; };
    virtual bool ref_data(std::vector<std::string> *data) { return false; };

    virtual bool set_data(int  data) { return false; };
    virtual bool add_data(int  data) { return false; };
    virtual bool ref_data(int *data) { return false; };

    virtual bool set_data(glyph gl, int posx, int posy) { return false; };

// This is used to set fg & bg, and hence is defined for element!
    virtual bool set_data(nc_color FG, nc_color BG = c_null);

    virtual void clear_data() {};

    virtual std::string get_str() { std::string ret; return ret; };
    virtual int get_int() { return 0; };
    virtual std::vector<std::string> get_str_list()
            { std::vector<std::string> ret; return ret; };
  };

  struct ele_drawing : public element
  {
    std::map<point, glyph, pointcomp> drawing;

    ele_drawing() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                    selected = false; selectable = false;
                    fg = c_white; bg = c_black; owns_data = true; }

    virtual element_type type() { return ELE_DRAWING; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool set_data(glyph gl, int posx, int posy);

    virtual bool set_data(nc_color FG, nc_color BG = c_null);

    virtual void clear_data() { drawing.clear();};

/* Translate is breaking a rule here; it's a function that isn't inherited from
 * element.  I'm not sure I'm okay with this, but for now I think that translate
 * is really drawing-specific; it looks for all instances of "from", and moves
 * them to "to".  While things like textbox could probably use this, I'm holding
 * off for now.
 */
    bool translate(long from, long to);
  };

  struct ele_textbox : public element
  {
    std::vector<std::string> *text;
    int offset;

    ele_textbox() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                    selected = false; selectable = false; offset = 0;
                    fg = c_white; bg = c_black; owns_data = false;
                    self_reference(); }
    ~ele_textbox() { if (owns_data) delete text; };

    virtual element_type type() { return ELE_TEXTBOX; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

/* We store this as a vector because the text needs to be split into seperate
 * lines.  It's more efficient to do this once, when the text is stored, than
 * every time we print.
 */
    virtual bool self_reference();

    virtual bool set_data(std::string data);
    virtual bool add_data(std::string data);

    virtual bool set_data(std::vector<std::string> data);
    virtual bool add_data(std::vector<std::string> data);
    virtual bool ref_data(std::vector<std::string> *data);

// These adjust the offset
    virtual bool set_data(int data);
    virtual bool add_data(int data);

    virtual void clear_data() { text->clear(); offset = 0;};

    virtual std::string get_str();
    virtual std::vector<std::string> get_str_list() { return (*text); };
  };

  struct ele_list : public element
  {
    std::vector<std::string> *list;
    int offset;
    int selection;

    ele_list() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                 selected = false; selectable = false; offset = 0;
                 selection = 0; fg = c_white; bg = c_black; owns_data = false;
                 self_reference(); }

    virtual element_type type() { return ELE_LIST; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool self_reference();

    virtual bool add_data(std::string data);

    virtual bool set_data(std::vector<std::string> data);
    virtual bool add_data(std::vector<std::string> data);
    virtual bool ref_data(std::vector<std::string> *data);

// These are used to set the selection
    virtual bool set_data(int data);
    virtual bool add_data(int data);

    virtual void clear_data() { list->clear(); offset = 0; selection = 0;};

    virtual int get_int();
    virtual std::string get_str();
    virtual std::vector<std::string> get_str_list() { return (*list); };
  };

  struct ele_textentry : public element
  {
    std::string *text;

    ele_textentry() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                      selected = false; selectable = false;
                      fg = c_white; bg = c_black; owns_data = false;
                      self_reference(); }

    virtual element_type type() { return ELE_TEXTENTRY; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool self_reference();

    virtual bool set_data(std::string data);
    virtual bool add_data(std::string data);
    virtual bool ref_data(std::string *data);

    virtual void clear_data() { text->clear(); };

    virtual std::string get_str() { return (*text); };
  };

  struct ele_number : public element
  {
    int *value;

    ele_number() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                   selected = false; selectable = false; value = 0;
                   fg = c_white; bg = c_black; owns_data = false;
                   self_reference(); }
    virtual element_type type() { return ELE_NUMBER; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool self_reference();

    virtual bool set_data(int data);
    virtual bool add_data(int data);
    virtual bool ref_data(int *data);

    virtual void clear_data() { value = 0; };

    virtual int get_int() { return (*value); };
  };

  struct ele_menu : public element
  {
    std::string title;
    std::vector<std::string> *list;
    int selection, offset;

    ele_menu() { name = ""; posx = 0; posy = 0; sizex = 0; sizey = 0;
                 selected = false; selectable = false; selection = 0;
                 offset = 0; fg = c_white; bg = c_black; title = ""; 
                 owns_data = false; self_reference(); }

    virtual element_type type() { return ELE_MENU; };
    virtual void draw(Window *win);

    virtual std::string save_data();
    virtual void load_data(std::istream &datastream);

    virtual bool self_reference();

// set_data sets the title--the only string unique identified w/o an index
    virtual bool set_data(std::string data);
// add_data adds an option to the menu--the only place where adding makes sense
    virtual bool add_data(std::string data);

    virtual bool set_data(std::vector<std::string> data);
    virtual bool add_data(std::vector<std::string> data);
    virtual bool ref_data(std::vector<std::string> *data);

// Change the selection
    virtual bool set_data(int data);
    virtual bool add_data(int data);

    virtual void clear_data() { title.clear(); list->clear(); offset = 0;
                                selection = 0; };

    virtual std::string get_str();
    virtual int get_int();
    virtual std::vector<std::string> get_str_list() { return (*list); };
  };


  enum action_id
  {
    ACT_NULL = 0,    // Do nothing

    ACT_SELECT_NEXT, // Select next
    ACT_SELECT_LAST, // Select last
    ACT_SELECT_NONE, // Select nothing
    ACT_SELECT_STR,  // Select something specific

    ACT_SCROLL,      // Scroll or adjust int field

    ACT_SET_COLORS,  // Set foreground color
    ACT_TRANSLATE,   // Change char A to char B

    ACT_MAX
  };

  bool action_needs_element(action_id act);
  std::string action_name(action_id act);

  struct binding
  {
    action_id act;
    std::string target;
    int a, b;
    binding(action_id ACT = ACT_NULL, std::string T = "", int A = 0, int B = 0):
           act (ACT), target (T), a (A), b (B) { };

    std::string save_data();
    void load_data(std::istream &datastream);
  };

      
  class interface
  {
   public:
    interface(std::string N = "", int X = 80, int Y = 24);
    ~interface();

    void add_element(element_type type, std::string name, int posx, int posy,
                     int sizex, int sizey, bool selectable = true);
    bool erase_element(std::string name);

    void draw(Window *win);
    void draw_prototype(Window *win); // For the editor

    std::string save_data();
    void load_data(std::istream &datastream);

    bool save_to_file(std::string filename);
    bool load_from_file(std::string filename);

    std::vector<std::string> element_names();

    element* selected();
    element* find_by_name(std::string name);

    element* select_next(bool force = false);
    element* select_last(bool force = false);
    element* select(std::string name);
    void select_none();

    bool set_selectable(std::string name, bool setting);
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
    bool set_data(std::string name, nc_color fg, nc_color bg = c_null);

    bool self_reference(std::string name);
    bool ref_data(std::string name, std::string *data);
    bool ref_data(std::string name, std::vector<std::string> *data);
    bool ref_data(std::string name, int *data);

    bool clear_data(std::string name);

    std::string get_str(std::string name);
    int get_int(std::string name);
    std::vector<std::string> get_str_list(std::string name);

    std::vector<std::string> binding_list();
    bool add_binding(long ch, action_id act, std::string target = "");
    bool add_binding(long ch, action_id act, std::string target,
                     int a, int b = 0);
    binding* bound_to(long ch);
    bool rem_binding(long ch);
    bool rem_all_bindings(action_id act = ACT_NULL);
    bool rem_all_bindings(std::string target);
    bool toggle_bindings();

    bool handle_action(long ch);

    std::string name;
    int sizex, sizey;

   private:
    int active_element;
    std::vector<element*> elements;
    std::map<long, binding> bindings;
    bool use_bindings;
  };

}; // namespace cuss

#endif
