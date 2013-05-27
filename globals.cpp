#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include "globals.h"
#include "window.h"
#include "stringfunc.h"
#include "rng.h"
#include "itemtype.h"

std::string DATADIR;
std::string CUSSDIR;

std::vector<body_part> BODY_PARTS_POOL;
std::vector<body> BODIES_POOL;
std::vector<terrain_type*> TERRAIN_POOL;
std::vector<element> ELEMENTS_POOL;
std::vector<item_type*> ITEMS_POOL;
std::vector< std::vector<mapgen_spec*> > MAPGEN_POOL;

void init_environment()
{
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  DATADIR = std::string(getenv("APPDATA")) + "\\bas";
  CUSSDIR = std::string(getenv("APPDATA")) + "\\cuss";
 #else
/*
  DATADIR = std::string(getenv("HOME")) + "/.bas";
  CUSSDIR = std::string(getenv("HOME")) + "/.cuss";
*/
  DATADIR = "bas";
  CUSSDIR = "cuss";
 #endif

 DIR *dir = opendir(DATADIR.c_str());
 if (!dir) {
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  mkdir(DATADIR.c_str());
 #else
  mkdir(DATADIR.c_str(), 0777);
 #endif
 } else
  closedir(dir);

 dir = opendir(CUSSDIR.c_str());
 if (!dir) {
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  mkdir(CUSSDIR.c_str());
 #else
  mkdir(CUSSDIR.c_str(), 0777);
 #endif
 } else
  closedir(dir);

 srand(time(NULL));
 rand();  // For some reason a call to rand() seems to be necessary to avoid
          // repetion.
}

void init_data()
{
 if (DATADIR.empty()) {
  debugmsg("DATADIR not initialized - can't load data!");
  return;
 }

 std::ifstream fin;
 std::string filename;

 filename = DATADIR + "/parts.txt";
 fin.open( filename.c_str() );
 if (fin.is_open()) {
  int numparts;
  fin >> numparts;
  for (int i = 0; i < numparts; i++) {
   body_part tmppart;
   tmppart.load_data(fin);
   BODY_PARTS_POOL.push_back(tmppart);
  }
  fin.close();
 }

 filename = DATADIR + "/bodies.txt";
 fin.open( filename.c_str() );
 if (fin.is_open()) {
  int numbodies;
  fin >> numbodies;
  for (int i = 0; i < numbodies; i++) {
   body tmpbody;
   tmpbody.load_data(fin);
   BODIES_POOL.push_back(tmpbody);
  }
  fin.close();
 }

  filename = DATADIR + "/terrain.txt";
  fin.open( filename.c_str() );
  if (fin.is_open()) {
    int numter;
    fin >> numter;
    for (int i = 0; i < numter && !fin.eof(); i++) {
      terrain_type* tmp = new terrain_type;
      tmp->load_data(fin);
      if (!tmp->name.empty())
        TERRAIN_POOL.push_back(tmp);
    }
    fin.close();
    for (int i = 0; i < TERRAIN_POOL.size(); i++) {
      TERRAIN_POOL[i]->init_transformations();
    }
  }

  filename = DATADIR + "/elements.txt";
  fin.open( filename.c_str() );
  if (fin.is_open()) {
    int numele;
    fin >> numele;
    for (int i = 0; i < numele && !fin.eof(); i++) {
      element tmp;
      tmp.load_data(fin);
      if (!tmp.name.empty()) {
        ELEMENTS_POOL.push_back(tmp);
      }
    }
    fin.close();
  }

  filename = DATADIR + "/items.txt";
  fin.open( filename.c_str() );
  if (fin.is_open()) {
    int numitem;
    fin >> numitem;
    for (int i = 0; i < numitem && !fin.eof(); i++) {
      item_type* tmp = load_item_type(fin);
      ITEMS_POOL.push_back(tmp);
    }
    fin.close();
  }

/* Loading mapgen specs is special because:
 * A) They may be spread across multiple files
 * B) A file may contain multiple specs
 * C) There's some metadata which indicates where the data should go
 * Thus, there's a special function for this.  In order to keep all the real
 * "meat" of mapgen spec loading in the same place, this function is in
 * mapgen.cpp.
 */
  load_mapgen_specs();
}

void save_data()
{
  if (DATADIR.empty()) {
    debugmsg("DATADIR not initialized - can't save data!");
    return;
  }

  std::ofstream fout;
  std::string filename;

  filename = DATADIR + "/parts.txt";
  fout.open( filename.c_str() );
  if (fout.is_open()) {
    fout << BODY_PARTS_POOL.size() << " ";
    for (std::vector<body_part>::iterator it = BODY_PARTS_POOL.begin();
         it != BODY_PARTS_POOL.end(); it++) {
      fout << it->save_data() << " ";
    }
    fout.close();
  }

  filename = DATADIR + "/bodies.txt";
  fout.open( filename.c_str() );
  if (fout.is_open()) {
    fout << BODIES_POOL.size() << " ";
    for (std::vector<body>::iterator it = BODIES_POOL.begin();
         it != BODIES_POOL.end(); it++) {
      fout << it->save_data() << " ";
    }
    fout.close();
  }

  filename = DATADIR + "/terrain.txt";
  fout.open( filename.c_str() );
  if (fout.is_open()) {
    fout << TERRAIN_POOL.size() << std::endl;
    for (std::vector<terrain_type*>::iterator it = TERRAIN_POOL.begin();
         it != TERRAIN_POOL.end(); it++)
      fout << (*it)->save_data() << std::endl;
    fout.close();
  }

  filename = DATADIR + "/elements.txt";
  fout.open( filename.c_str() );
  if (fout.is_open()) {
    fout << ELEMENTS_POOL.size() << std::endl;
    for (std::vector<element>::iterator it = ELEMENTS_POOL.begin();
         it != ELEMENTS_POOL.end(); it++) {
      fout << it->save_data() << std::endl;
    }
    fout.close();
  }

  filename = DATADIR + "/items.txt";
  fout.open( filename.c_str() );
  if (fout.is_open()) {
    fout << ITEMS_POOL.size() << std::endl;
    for (int i = 0; i < ITEMS_POOL.size(); i++) {
      item_type* it = ITEMS_POOL[i];
      fout << get_item_category_name( it->type() ) << ": " <<
              it->save_data() << std::endl;
    }
    fout.close();
  }

}

int lookup_terrain_id(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < TERRAIN_POOL.size(); i++) {
    if (no_caps(TERRAIN_POOL[i]->name) == name)
      return i;
  }
  return 0;
}

int lookup_body_id(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < BODIES_POOL.size(); i++) {
    if (no_caps(BODIES_POOL[i].name) == name)
      return i;
  }
  return 0;
}

void examine_terrain()
{
  std::stringstream out;
  for (int i = 0; i < TERRAIN_POOL.size(); i++) {
    out << i << ": " << TERRAIN_POOL[i]->name << ": " << TERRAIN_POOL[i];
    out << std::endl;
  }
  debugmsg(out.str().c_str());
}
