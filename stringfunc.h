#ifndef _STRINGFUNC_H_
#define _STRINGFUNC_H_

#include <string>
#include <vector>

#define STD_DELIM "</>"

std::string load_to_delim(std::istream &datastream, std::string delim);
std::vector<std::string> break_into_lines(std::string text, int linesize);

#endif
