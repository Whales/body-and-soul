#include <sstream>
#include "body.h"
#include "rng.h"

int body::challenge_roll(challenge_type type)
{
 return rng( 0, skill_level(type) );
}

int body::skill_level(challenge_type type)
{
 int ret = 0, num = 0, biggest = 0;
 std::list<body_part>::iterator bp_it;
 std::list<body_stat>::iterator bs_it;
 std::list<challenge_type>::iterator ch_it;
 for (bp_it = body_parts.begin(); bp_it != body_parts.end(); bp_it++) {
  for (bs_it = bp_it->stats.begin(); bs_it != bp_it->stats.end(); bs_it++) {
   for (ch_it = bs_it->chals.begin(); ch_it != bs_it->chals.end(); ch_it++) {
    if (*ch_it == type) {
     num++;
     ret += bs_it->value;
     if (bs_it->value > biggest)
      biggest = bs_it->value;
    }
   }
  }
 }

// Subtract 10% for each limb past the first, preserving the biggest
 if (num > 1) {
  ret -= biggest;
  ret -= (ret * (num - 1)) / 10;
  ret += biggest;
 }

 return ret;
}

std::string body::save_data()
{
 std::stringstream ret;
 ret << symbol.save_data() << name << " </> " << body_parts.size() <<
        " ";
 for (std::list<body_part>::iterator it = body_parts.begin();
      it != body_parts.end(); it++)
  ret << it->save_data() << " ";

 return ret.str();
}

void body::load_data(std::istream &datastream)
{
 symbol.load_data(datastream);
 std::string tmpstr;
 do {
  datastream >> tmpstr;
  if (tmpstr != "</>")
   name += tmpstr + " ";
 } while (tmpstr != "</>");

 if (!name.empty())
  name = name.substr(0, name.size() - 1); // Clear out the extra " "

 int num_parts;
 datastream >> num_parts;

 for (int i = 0; i < num_parts; i++) {
  body_part tmppart;
  tmppart.load_data(datastream);
  body_parts.push_back(tmppart);
 }
}

std::string body_part::save_data()
{
 std::stringstream ret;
 ret << name << " </> " << stats.size() << " ";
 for (std::list<body_stat>::iterator it = stats.begin();
      it != stats.end(); it++)
  ret << it->save_data() << " ";

 return ret.str();
}

void body_part::load_data(std::istream &datastream)
{
 std::string tmpstr;
 do {
  datastream >> tmpstr;
  if (tmpstr != "</>")
   name += tmpstr + " ";
 } while (tmpstr != "</>");

 int statsize;
 datastream >> statsize;
 for (int i = 0; i < statsize; i++) {
  body_stat stattmp;
  stattmp.load_data(datastream);
  stats.push_back(stattmp);
 }
}

std::string body_stat::save_data()
{
 std::stringstream ret;
 ret << name << " </> " << value << " " << chals.size();
 for (std::list<challenge_type>::iterator it = chals.begin();
      it != chals.end(); it++)
  ret << int(*it) << " ";

 return ret.str();
}

void body_stat::load_data(std::istream &datastream)
{
 std::string tmpstr;
 do {
  datastream >> tmpstr;
  if (tmpstr != "</>")
   name += tmpstr + " ";
 } while (tmpstr != "</>");

 datastream >> value;

 int numchals;
 datastream >> numchals;
 for (int i = 0; i < numchals; i++) {
  int tmp;
  datastream >> tmp;
  chals.push_back( challenge_type(tmp) );
 }
}
