#include <dirent.h>
#include <fstream>
#include "map.h"
#include "mapgen.h"
#include "stringfunc.h"
#include "window.h"
#include "rng.h"

void load_mapgen_file(std::istream &datastream);
int get_mapgen_ter_id(std::istream &datastream, std::string name);
std::vector< std::vector<int> > noise_map();

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
      debugmsg("File %s", filename.c_str());
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
  debugmsg("Loading...");
  do {
    datastream >> id;
    id = no_caps(id);

    if (id == "maptype") {
      std::string new_type = load_to_character(datastream, ",;\n", true);
      current_type = lookup_map_type(new_type);
      debugmsg("%s - %d", new_type.c_str(), current_type);
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
          debugmsg("Forgot to code for mapgen type #%d", gen_type);
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
          debugmsg("terrain '%s'", terid.c_str());
          if (tmppoint.ter_id == 0) {
            debugmsg("Error in height map; no terrain '%s'", terid.c_str());
          } else if (tmpmap.empty() ||
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

    } else if (id.find("endmap") == 0) {
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

void submap::generate(mapgen_spec* spec)
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
        std::vector< std::vector<int> > noise = noise_map();
        for (int x = 0; x < SUBMAP_SIZE; x++) {
          for (int y = 0; y < SUBMAP_SIZE; y++) {
            bool done = false;
            for (int i = 0; !done && i < it->size(); i++) {
              if ((*it)[i].percentile < noise[x][y]) {
                done = true;
                if ((*it)[i].ter_id > 0)
                  tiles[x][y].set_type((*it)[i].ter_id);
              }
            }
            if (!done && !it->empty() && it->back().ter_id > 0) {
              tiles[x][y].set_type(it->back().ter_id);
            }
          }
        }
      }
    } break;

  }
}

std::vector< std::vector<int> > noise_map()
{
  std::vector< std::vector<int> > noise;
  for (int x = 0; x < SUBMAP_SIZE; x++) {
    std::vector<int> tmprow;
    for (int y = 0; y < SUBMAP_SIZE; y++) {
      tmprow.push_back(0);
    }
    noise.push_back(tmprow);
  }
      
  for (int size = 1; size <= SUBMAP_SIZE / 2; size *= 2) {
    for (int x = 0; x < SUBMAP_SIZE; x += size) {
      for (int y = 0; y < SUBMAP_SIZE; y += size) {
        int val = rng(0, 99);
        if (size == 1) {
          noise[x][y] = val;
        } else {
          for (int px = x; px < x + size; px++) {
            for (int py = y; py < y + size; py++) {
              noise[px][py] += 1 + val;
              noise[px][py] /= 2;
            }
          }
        }
      }
    }
  }

  return noise;
}
