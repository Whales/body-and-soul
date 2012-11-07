#include "stringfunc.h"

std::vector<std::string> break_into_lines(std::string text, int linesize)
{
 std::vector<std::string> ret;

 while (text.length() > linesize) {

  size_t linebreak = text.find_last_of(" ", linesize);
  std::string tmp;
   if (linebreak == std::string::npos) {
    linebreak = linesize - 1;
    tmp = text.substr(0, linebreak) + "-";
   } else
    tmp = text.substr(0, linebreak);

  ret.push_back(tmp);
  text = text.substr(linebreak + 1);
 }

 return ret;
}

std::string load_to_delim(std::istream &datastream, std::string delim)
{
 std::string ret, tmp;
 do {
  datastream >> tmp;
  if (tmp != delim)
   ret += tmp + " ";
 } while (tmp != delim);

 if (!ret.empty() && ret[ret.size() - 1] == ' ')
  ret = ret.substr(0, ret.size() - 1);

 return ret;
}
