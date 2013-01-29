#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>
#include <list>
#include "map.h"
#include "body.h"
#include "terrain.h"
#include "window.h"
#include "mapgen.h"

extern std::string DATADIR;
extern std::vector<body_part> BODY_PARTS_POOL;
extern std::vector<body> BODIES_POOL;
extern std::list<Window*> WINDOWLIST;
extern std::vector<terrain_type*> TERRAIN_POOL;
extern std::vector< std::vector<mapgen_spec*> > MAPGEN_POOL;

void init_environment();
void init_data();
void save_data();

#endif
