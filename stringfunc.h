#ifndef _STRINGFUNC_H_
#define _STRINGFUNC_H_

#include <string>
#include <vector>
#include <istream>

#define STD_DELIM "</>"

std::vector<std::string> break_into_lines(std::string text, int linesize);

std::string load_to_delim(std::istream &datastream, std::string delim);
std::string load_to_character(std::istream &datastream, char ch,
                              bool trim = false);
std::string load_to_character(std::istream &datastream, std::string chars,
                              bool trim = false);

std::string trim(const std::string &orig);

std::string all_caps(const std::string &orig);

#endif
