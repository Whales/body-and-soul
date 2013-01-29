#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>
#include <list>
#include "body.h"
#include "terrain.h"
#include "window.h"

extern std::string DATADIR;
extern std::vector<body_part> BODY_PARTS_POOL;
extern std::vector<body> BODIES_POOL;
extern std::list<Window*> WINDOWLIST;
extern std::vector<terrain_type*> TERRAIN_POOL;

void init_environment();
void init_data();
void save_data();

#endif
