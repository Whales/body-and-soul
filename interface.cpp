#ifndef _ORZ_H_
#define _ORZ_H_

#include <string>
#include <vector>
#include "window.h"

namespace orz {

  enum element_type
  {
    ELE_NULL = 0, // Nothing
    ELE_DRAWING,  // Plain characters / text
    ELE_TEXTBOX,  // Scrollable text
    ELE_LIST,     // Scrollable list w/ selection
    ELE_TEXTENTRY,// Type to enter text
    ELE_MAX
  };

  struct element
  {
    virtual element_type type() { return ELE_NULL; };
    virtual void draw(Window *win);

    std::string name;
    int posx;
    int posy;
    int sizex;
    int sizey;
  };

  struct ele_drawing : public element
  {
    virtual element_type type() { return ELE_DRAWING; };
    virtual void draw(Window *win);

    std::vector< std::vector<long> > drawing;
  };

  struct ele_textbox : public element
  {
    virtual element_type type() { return ELE_TEXTBOX; };
    virtual void draw(Window *win);

    std::string text;
    int offset;
  };

  struct ele_list : public element
  {
    virtual element_type type() { return ELE_LIST; };
    virtual void draw(Window *win);

    std::vector<std::string> list;
    int offset;
  };

  struct ele_textentry : public element
  {
    virtual element_type type() { return ELE_TEXTENTRY; };
    virtual void draw(Window *win);

    std::string text;
  };
      
  class interface
  {
   public:
    bool send_data(std::string, std::string); // For textboxes & textentry
    bool send_data(std::string, std::vector<std::string>);
    bool send_data(std::string, int); // For sliders, toggles, etc
   private:
    int current_element;
    std::vector<element> elements;
  };

}; // namespace orz

#endif
