#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <string>
#include <list>
#include "body.h"
#include "window.h"

extern std::string DATADIR;
extern std::list<body_stat> BODY_STATS_POOL;
extern std::list<body_part> BODY_PARTS_POOL;
extern std::list<body> BODIES_POOL;
extern std::list<Window*> WINDOWLIST;

void init_environment();
void init_data();
void save_data();

#endif
