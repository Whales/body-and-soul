#include <dirent.h>
#include <fstream>
#include "map.h"
#include "mapgen.h"
#include "stringfunc.h"
#include "window.h"
#include "rng.h"

void load_mapgen_file(std::istream &datastream);
int get_mapgen_ter_id(std::istream &datastream, std::string name);
std::vector< std::vector<int> > noise_map(int mapsize,
                                         std::vector< std::vector<int> > &orig);
int interpolate(int a, int b, float alpha);
int rev_heightmap(int ter_id, int dist,
                  std::list< std::vector<height_point> >::iterator &it);

void load_mapgen_specs()
{
// Init MAPGEN_POOL with empty vectors; one for each map type
  for (int i = 0; i < MAP_MAX; i++) {
    std::vector<mapgen_spec*> tmp;
    MAPGEN_POOL.push_back(tmp);
  }
  std::string dirname = DATADIR + "/mapgen";
  dirent *dp;
  DIR *dir = opendir(dirname.c_str());
  if (!dir) {
    debugmsg("Couldn't open %s.", dirname.c_str());
    return;
  }
  std::string filename;
  while (dp = readdir(dir)) {
    filename = dp->d_name;
    if (filename != "." && filename != "..") {
      filename = DATADIR + "/mapgen/" + filename;
      std::ifstream fin;
      fin.open(filename.c_str());
      if (fin.is_open()) {
        load_mapgen_file(fin);
      } else {
        debugmsg("Not opened.");
      }
    }
  }
}

void load_mapgen_file(std::istream &datastream)
{
  std::string id;
  map_type current_type = MAP_NULL;
  do {
    datastream >> id;
    id = no_caps(id);

    if (id == "maptype") {
      std::string new_type = load_to_character(datastream, ",;\n", true);
      current_type = lookup_map_type(new_type);
      if (current_type == MAP_NULL) {
        debugmsg("Invalid maptype %s.", new_type.c_str());
      }
    } else if (id == "gen") {
      std::string gen_id, gen_type_name;
      datastream >> gen_type_name;
      mapgen_type gen_type = lookup_mapgen_type(gen_type_name);

      switch (gen_type) {

        case MAPGEN_NULL:
          debugmsg("Invalid mapgen type: %s", gen_type_name.c_str());
          break;

        case MAPGEN_BUILDINGS: {
          mapgen_spec_buildings* tmp = new mapgen_spec_buildings;
          tmp->load_data(datastream);
          MAPGEN_POOL[current_type].push_back(tmp);
        } break;

        case MAPGEN_LABYRINTH: {
          mapgen_spec_labyrinth* tmp = new mapgen_spec_labyrinth;
          tmp->load_data(datastream);
          MAPGEN_POOL[current_type].push_back(tmp);
        } break;

        case MAPGEN_HEIGHTMAP: {
          mapgen_spec_heightmap* tmp = new mapgen_spec_heightmap;
          tmp->load_data(datastream);
          MAPGEN_POOL[current_type].push_back(tmp);
        } break;

        default:
          debugmsg("Forgot to code for mapgen type #%d (%s)", gen_type,
                   gen_type_name.c_str());
      }
    }
  } while (!datastream.eof() && id != "done");
}

void mapgen_spec::load_data(std::istream &datastream)
{
  debugmsg("ran load_data on flavorless mapgen_spec");
  return;
}

void mapgen_spec_buildings::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":;\n", true);
  std::string id;
  do {
    datastream >> id;
    id = no_caps(id);
    if (id.find("wall") == 0) {
      wall_id = get_mapgen_ter_id(datastream, "wall");
    } else if (id.find("floor") == 0) {
      floor_id = get_mapgen_ter_id(datastream, "floor");
    } else if (id.find("door") == 0) {
      door_id = get_mapgen_ter_id(datastream, "door");
    } else if (id.find("outside") == 0) {
      outside_id = get_mapgen_ter_id(datastream, "outside");
    } else if (id.find("minsize") == 0) {
      datastream >> size_low;
    } else if (id.find("maxsize") == 0) {
      datastream >> size_high;
    } else if (id.find("minspacing") == 0) {
      datastream >> spacing_low;
    } else if (id.find("maxspacing") == 0) {
      datastream >> spacing_high;
    }
  } while (!datastream.eof() && id != "done");
}

void mapgen_spec_labyrinth::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":;\n", true);
  std::string id;
  do {
    datastream >> id;
    id = no_caps(id);

    if (id.find("wall") == 0) {
      wall_id = get_mapgen_ter_id(datastream, "wall");
    } else if (id.find("floor") == 0) {
      floor_id = get_mapgen_ter_id(datastream, "floor");
    } else if (id.find("minwidth") == 0) {
      datastream >> width_low;
    } else if (id.find("maxwidth") == 0) {
      datastream >> width_high;
    }
  } while (!datastream.eof() && id != "done");
}

void mapgen_spec_heightmap::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":;\n", true);
  std::string id;
  do {
    std::vector<height_point> tmpmap;
    datastream >> id;
    id = no_caps(id);

    if (id.find("map") == 0) {
      std::string terid;
      do {
        terid = no_caps(load_to_character(datastream, ";:,\n", true));
        int val;
        if (terid != "done") {
          datastream >> val;
          height_point tmppoint;
          tmppoint.ter_id = lookup_terrain_id(terid);
          tmppoint.percentile = val;
          if (tmpmap.empty() ||
              tmppoint.percentile <= tmpmap.back().percentile) {
            tmpmap.push_back(tmppoint);
          } else { // Search for proper insertion point
            bool done = false;
            for (std::vector<height_point>::iterator it = tmpmap.begin();
                 it != tmpmap.end() && !done;
                 it++) {
              if (it->percentile < tmppoint.percentile) {
                tmpmap.insert(it, tmppoint);
                done = true;
              }
            }
          }
        } 
      } while (terid != "done"); // Heightmap loop
      maps.push_back(tmpmap);
      tmpmap.clear();
    }
  } while (!datastream.eof() && id != "done");
}

mapgen_type lookup_mapgen_type(std::string name)
{
  name = no_caps(name);
  for (int i = 0; i < MAPGEN_MAX; i++) {
    mapgen_type ret = mapgen_type(i);
    if (name == no_caps(mapgen_type_name(ret)))
      return ret;
  }
  return MAPGEN_NULL;
}

std::string mapgen_type_name(mapgen_type type)
{
  switch (type) {
    case MAPGEN_MAX:
    case MAPGEN_NULL:       return "Null";
    case MAPGEN_BUILDINGS:  return "Buildings";
    case MAPGEN_LABYRINTH:  return "Labyrinth";
    case MAPGEN_HEIGHTMAP:  return "Heightmap";
    default:                return "Forgotten";
  }
  return "EscapedSwitch";
}

int get_mapgen_ter_id(std::istream &datastream, std::string name)
{
  std::string tername = load_to_character(datastream, ";,\b", true);
  int terid = lookup_terrain_id(tername);
  if (terid == 0) {
    debugmsg("Invalid %s: %s", name.c_str(), tername.c_str());
  }
  return terid;
}
  

// The actual meat of generation.

void submap::generate(mapgen_spec* spec, submap *north, submap *east,
                                         submap *south, submap *west)
{
  switch (spec->gentype()) {

    case MAPGEN_BUILDINGS: {
      mapgen_spec_buildings* bspec = static_cast<mapgen_spec_buildings*>(spec);
    } break;

    case MAPGEN_LABYRINTH: {
      mapgen_spec_labyrinth* lspec = static_cast<mapgen_spec_labyrinth*>(spec);
    } break;

    case MAPGEN_HEIGHTMAP: {
      mapgen_spec_heightmap* hspec = static_cast<mapgen_spec_heightmap*>(spec);
// Run through all the height maps
      for (std::list< std::vector<height_point> >::iterator it =
           hspec->maps.begin(); it != hspec->maps.end(); it++) {
// Initialize the noisemap with 0, or with appropriate values taken from
// adjacent submaps
        std::vector< std::vector<int> > noise;
        for (int x = 0; x < SUBMAP_SIZE; x++) {
          std::vector<int> tmprow;
          for (int y = 0; y < SUBMAP_SIZE; y++) {
            if (x <= 10 && west) {
              int ter_id = west->tiles[SUBMAP_SIZE - 1][y].type->uid;
              tmprow.push_back(rev_heightmap(ter_id, x, it));
            } else if (y <= 10 && north) {
              int ter_id = north->tiles[x][SUBMAP_SIZE - 1].type->uid;
              tmprow.push_back(rev_heightmap(ter_id, y, it));
            } else if (x >= SUBMAP_SIZE - 11 && east) {
              int ter_id = east->tiles[0][y].type->uid;
              tmprow.push_back(rev_heightmap(ter_id, SUBMAP_SIZE - 1 - x, it));
            } else if (y >= SUBMAP_SIZE - 11 && south) {
              int ter_id = south->tiles[x][0].type->uid;
              tmprow.push_back(rev_heightmap(ter_id, SUBMAP_SIZE - 1 - y, it));
            } else {
              tmprow.push_back(0);
            }
          }
          noise.push_back(tmprow);
        }
        noise = noise_map(SUBMAP_SIZE, noise);
        for (int x = 0; x < SUBMAP_SIZE; x++) {
          for (int y = 0; y < SUBMAP_SIZE; y++) {
            bool done = false;
            for (int i = 0; !done && i < it->size(); i++) {
              if ((*it)[i].percentile < noise[x][y]) {
                done = true;
                if ((*it)[i].ter_id > 0) {
                  tiles[x][y].set_type((*it)[i].ter_id);
                }
              }
            }
/*
            if (!done && !it->empty() && it->back().ter_id > 0) {
              tiles[x][y].set_type(it->back().ter_id);
            }
*/
          }
        }
      }
    } break;

  }
}

std::vector< std::vector<int> > noise_map(int mapsize,
                                          std::vector< std::vector<int> > &orig)
{
  Window w_test(0, 0, 80, 24);
  std::vector< std::vector<int> > noise;
  std::vector< std::vector<int> > ret;
  for (int x = 0; x < mapsize; x++) {
    std::vector<int> tmprow, tmpretrow;;
    for (int y = 0; y < mapsize; y++) {
      if (orig[x][y] == 0) {
        tmprow.push_back( rng(0, 99) );
      } else {
        tmprow.push_back( orig[x][y] );
      }
      tmpretrow.push_back( 0 );
    }
    noise.push_back(tmprow);
    ret.push_back(tmpretrow);
  }

  for (int size = 1; size <= mapsize / 2; size *= 2) {
    float freq = 1.0 / float(size);
    for (int x = 0; x < mapsize; x++) {
      int x0 = (x / size) * size, x1 = (x0 + size) % mapsize;
      float xalpha = (x - x0) * freq;
      for (int y = 0; y < mapsize; y++) {
        int y0 = (y / size) * size, y1 = (y0 + size) % mapsize;
        int yalpha = (y - y0) * freq;
        int top    = interpolate(noise[x0][y0], noise[x1][y0], xalpha),
            bottom = interpolate(noise[x0][y1], noise[x1][y1], xalpha);

        int result = interpolate(top, bottom, yalpha);
        //double layer_alpha = .1 + .9 * (double(size) / double(mapsize * 2));
        double layer_alpha = .1 + freq * 1.6;
        if (size == 1)
          ret[x][y] = result;
        else
          ret[x][y] = interpolate(ret[x][y], result, layer_alpha);
       // ret[x][y] = interpolate(result, ret[x][y], freq);
      }
    }
  }
  return ret;
}

int interpolate(int a, int b, float alpha)
{
  return a * (1.0 - alpha) + b * alpha;
}

int rev_heightmap(int ter_id, int dist,
                  std::list< std::vector<height_point> >::iterator &it)
{
  double alpha = double(dist) / 20;
  for (int i = 0; i < it->size(); i++) {
    if (ter_id == (*it)[i].ter_id) {
      int low = (*it)[i].percentile;
      int high = 99;
      if (i > 0) {
        high = (*it)[i - 1].percentile - 1;
      }
      return interpolate((high + low) / 2, rng(0, 99), alpha);
    }
  }
  return 0;
}

