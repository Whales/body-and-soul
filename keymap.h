#ifndef _KEYMAP_H_
#define _KEYMAP_H_

#include <map>

struct action_datum
{
  std::string name;
  action_id id;
  std::string identifier;
};

struct keymap
{
public:
  action_id get_action(long key);
  long get_key(action_id key);

  void rebind_key(long key, action_id act);
private:
  std::map<long, action_id> mapping;
  std::map<action_id, long> reverse;
};
#endif
