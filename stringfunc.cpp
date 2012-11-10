#include "stringfunc.h"

std::vector<std::string> break_into_lines(std::string text, int linesize)
{
 std::vector<std::string> ret;

 size_t chars = 0; // Number of actually-printed characters at...
 size_t pos = 0; // ... this point in the string
 size_t linebreak = std::string::npos; // The last acceptable breakpoint
 while (text.length() > linesize && pos < text.size()) {
  if (text.substr(pos, 3) == "<c=") {
   pos = text.find('>', pos);
   if (pos == std::string::npos) {
    //debugmsg("Bad colortag!");
    return ret;
   }
   linebreak = pos;
  } else if (text[pos] == ' ')
   linebreak = pos;
  pos++;
  chars++;
  if (chars > linesize) {
   std::string tmp;
   if (linebreak == std::string::npos) {
    linebreak = linesize - 1;
    tmp = text.substr(0, linebreak) + "-";
    text = text.substr(linebreak);
   } else if (text[linebreak] == ' ') {
    tmp = text.substr(0, linebreak);
    text = text.substr(linebreak + 1);
   } else if (text[linebreak] == '>') {
    linebreak++;
    tmp = text.substr(0, linebreak);
    text = text.substr(linebreak);
   }
   ret.push_back(tmp);
   pos = 0;
   chars = 0;
   linebreak = std::string::npos;
  }
 }
 ret.push_back(text);
 return ret;
/*
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

 ret.push_back(text);
*/

 return ret;
}

std::string load_to_delim(std::istream &datastream, std::string delim)
{
 std::string ret, tmp;
 do {
  datastream >> tmp;
  if (tmp != delim)
   ret += tmp + " ";
 } while (tmp != delim && !(datastream.eof()));

 if (!ret.empty() && ret[ret.size() - 1] == ' ')
  ret = ret.substr(0, ret.size() - 1);

 return ret;
}
