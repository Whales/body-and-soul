#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include "globals.h"
#include "window.h"

std::string DATADIR;
std::list<body_stat> BODY_STATS_POOL;
std::list<body_part> BODY_PARTS_POOL;
std::list<body> BODIES_POOL;
std::list<Window*> WINDOWLIST;

void init_environment()
{
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  DATADIR = std::string(getenv("APPDATA")) + "\\bas";
 #else
  DATADIR = std::string(getenv("HOME")) + "/.bas";
 #endif

 DIR *dir = opendir(DATADIR.c_str());
 if (!dir) {
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  mkdir(DATADIR.c_str());
 #else
  mkdir(DATADIR.c_str(), 0777);
 #endif
 }
}

void init_data()
{
 if (DATADIR.empty()) {
  debugmsg("DATADIR not initialized!");
  return;
 }

 std::ifstream fin;
 std::string filename;

 filename = DATADIR + "/stats.txt";
 fin.open( filename.c_str() );
 if (fin.is_open()) {
  int numstats;
  fin >> numstats;
  for (int i = 0; i < numstats; i++) {
   body_stat tmpstat;
   tmpstat.load_data(fin);
   BODY_STATS_POOL.push_back(tmpstat);
  }
  fin.close();
 }

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

}
