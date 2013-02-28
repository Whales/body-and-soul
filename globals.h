#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>
#include <list>
#include "map.h"
#include "body.h"
#include "terrain.h"
#include "element.h"
#include "window.h"
#include "mapgen.h"
#include "itemtype.h"

extern std::string DATADIR;
extern std::vector<body_part> BODY_PARTS_POOL;
extern std::vector<body> BODIES_POOL;
extern std::vector<terrain_type*> TERRAIN_POOL;
extern std::vector<element> ELEMENTS_POOL;
extern std::vector<item_type*> ITEMS_POOL;
extern std::vector< std::vector<mapgen_spec*> > MAPGEN_POOL;

extern std::list<Window*> WINDOWLIST;

void init_environment();
void init_data();
void save_data();

void examine_terrain();

int lookup_terrain_id(std::string name);
int lookup_body_id(std::string name);

#endif
