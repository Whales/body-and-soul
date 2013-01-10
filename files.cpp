#include <sys/types.h>
#include <dirent.h>
#include "files.h"

std::vector<std::string> files_in(std::string dir, std::string suffix)
{
 std::vector<std::string> ret;
 DIR *dp;
 dirent *dirp;

 if ( (dp = opendir(dir.c_str())) == NULL )
  return ret;

 while ( (dirp = readdir(dp)) != NULL ) {
  std::string filename(dirp->d_name);
  if (suffix == "" || filename.find(suffix) != std::string::npos)
   ret.push_back( std::string(dirp->d_name) );
 }

 closedir(dp);
 return ret;
}

void 
