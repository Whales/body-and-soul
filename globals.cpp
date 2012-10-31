#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "globals.h"

std::string datadir;

void init_environment()
{
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  datadir = std::string(getenv("APPDATA")) + "\\bas";
 #else
  datadir = std::string(getenv("HOME")) + "/.bas";
 #endif

 DIR *dir = opendir(datadir.c_str());
 if (!dir) {
 #if (defined _WIN32 || defined WINDOWS || defined __WIN32__)
  mkdir(datadir.c_str());
 #else
  mkdir(datadir.c_str(), 0777);
 #endif
 }
}
