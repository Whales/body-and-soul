#include <dirent.h>
#include <fstream>
#include <stdlib.h>
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

void set_connectors(submap* neighbor, int ter_id,
                    int xs, int ys, int &pos, int &width);

void draw_road(submap* sm, bool vertical, int ter_id, 
               int x0, int y0, int width0, int x1, int y1, int width1);

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
// Set all terrain to 0 - e.g. "clear" or "no change"
  wall_id = 0;
  floor_id = 0;
  door_id = 0;
  outside_id = 0;
  road_id = 0;
// Set the size bounds to some reasonble values
  minsize = 2;
  maxsize = 5;
  minspacing = 0;
  maxspacing = 4;
  minroad = 3;
  maxroad = 6;
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
    } else if (id.find("road") == 0) {
      road_id = get_mapgen_ter_id(datastream, "road");
    } else if (id.find("minsize") == 0) {
      datastream >> minsize;
    } else if (id.find("maxsize") == 0) {
      datastream >> maxsize;
    } else if (id.find("minspacing") == 0) {
      datastream >> minspacing;
    } else if (id.find("maxspacing") == 0) {
      datastream >> maxspacing;
    } else if (id.find("minroad") == 0) {
      datastream >> minroad;
    } else if (id.find("maxroad") == 0) {
      datastream >> maxroad;
    }
  } while (!datastream.eof() && id != "done");
}

void mapgen_spec_labyrinth::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":;\n", true);
  std::string id;
// Set all terrain ids to 0; i.e. "clear" or "no change"
  wall_id = 0;
  door_id = 0;
// Set width to some reasonable defaults
  minwidth = 1;
  maxwidth = 4;
  do {
    datastream >> id;
    id = no_caps(id);

    if (id.find("wall") == 0) {
      wall_id = get_mapgen_ter_id(datastream, "wall");
    } else if (id.find("floor") == 0) {
      floor_id = get_mapgen_ter_id(datastream, "floor");
    } else if (id.find("minwidth") == 0) {
      datastream >> minwidth;
    } else if (id.find("maxwidth") == 0) {
      datastream >> maxwidth;
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
// Start by setting everything to the "outdoor" terrain type
      if (bspec->outside_id > 0) {
        for (int x = 0; x < SUBMAP_SIZE; x++) {
          for (int y = 0; y < SUBMAP_SIZE; y++) {
            tiles[x][y].set_type(outside_id);
          }
        }
      }
// Next, determine where to start a vertical and horizontal road.
// This is generally random; if there's an adjacent submap, copy that.
      int vert_road_width_top = rng(bspec->minroad, bspec->maxroad),
          vert_road_width_bot = rng(bspec->minroad, bspec->maxroad),
          vert_road_pos_top = rng(5, SUBMAP_SIZE - 6 - vert_road_width),
          vert_road_pos_bot = rng(5, SUBMAP_SIZE - 6 - vert_road_width);
// Connect to neighbors if applicable
      set_connectors(north, -1, SUBMAP_SIZE - 1, bspec->road_id,
                       vert_road_width_top, vert_road_width_bot);
      set_connectors(south, -1, 0, bspec->road_id,
                       vert_road_width_top, vert_road_width_bot);
      draw_road(this, true, bspec->road_id, // "true" means this is vertical
                vert_road_pos_top, 0, vert_road_width_top,
                vert_road_pos_bottom, SUBMAP_SIZE - 1, vert_road_width_bottom);
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

void set_connectors(submap* neighbor, int ter_id,
                    int xs, int ys, int &pos, int &width)
{
  if (!neighbor) {
    return;
  }
  int tmppos = -1, tmpwidth = -1;
  for (int i = 0; (tmppos == -1 || tmpwidth == -1) && i < SUBMAP_width; i++) {
    int x = (xs == -1 ? i : xs);
    int y = (ys == -1 ? i : ys);
    if (tmppos == -1) {
      if (neighbor->tiles[x][y].type->uid == ter_id) {
        tmppos = i;
      } else {
        tmpwidth = i + 1 - tmppos;
      }
    }
  }
  if (tmppos >= 0) {
    pos = tmppos;
    if (tmpwidth >= 0) {
      width = tmpwidth;
    }
  }
}

void draw_road(submap* sm, bool vertical, int ter_id, 
               int x0, int y0, int width0, int x1, int y1, int width1)
{
/* All comments below will, for the sake of brevity and clarity, refer to a
 * vertically-oriented road.  They apply to horizontal ones as well.
 */

/* If x0 != x1, then at some point we'll need to start moving horizontally.
 * This will take abs(x1 - x0) steps, since we want to avoid slope > 1
 * Thus, we must start sloping at (SUBMAP_SIZE - 1 - dx) at the latest!
 */

  int diff = (vertical ? abs(x1 - x0) : abs(y1 - y0));
  int slopestart = rng(10, SUBMAP_SIZE - 10 - diff);

/* We want to change the width - if necessarily - during the sloped portion of
 * the road.  This makes it a little less jarring.  We want to space the width
 * change evenly across the $diff steps we're taking; if we assume that the
 * first change is at the very start, that leaves (diff - 1) steps to fit
 * (dWidth - 1) changes.
 */
  int dWidth = abs(width1 - width0);
  int widthspace = (dWidth == 1 ? 0 : (diff - 1) / (dWidth - 1));
/* Of course, we don't want to always make the first width change at the very
 * start of the sloping section.  Taking the remainer of (diff-1) / (dWidth-1)
 * gives us the meximum position we can start from.  Randomize!
 */
  int widthoffset = (dWidth == 1 ? rng(0, diff) :
                                   rng(0, (diff - 1) % (dWidth - 1)));
  int primary_step, secondary_step, width_step;
  if (vertical) {
    primary_step = (y1 > y0 ? 1 : -1);
    secondary_step = (x1 > x0 ? 1 : -1);
  } else {
    primary_step = (x1 > x0 ? 1 : -1);
    secondary_step = (y1 > y0 ? 1 : -1);
  }
  if (width1 > width0) {
    width_step = 1;
  } else {
    width_step = -1;
  }
  int x = x0, y = y0, width = width0;
  while (x != x1 && y != y1) {
    if (vertical) {
      y += primary_step;
      if (y >= slopestart && x != x1) {
        x += secondary_step;
      }
      if (width != width1 && y >= slopestart + widthoffset &&
          (y - slopestart - widthoffset) % widthspace == 0) {
        width += width_step;
      }
      for (int xn = x; xn <= x + width; xn++) {
        sm->tiles[xn][y].set_type(ter_id);
      }
    } else {
      x += primary_step;
      if (x >= slopestart && y != y1) {
        y += secondary_step;
      }
      if (width != width1 && x >= slopestart + widthoffset &&
          (x - slopestart - widthoffset) % widthspace == 0) {
        width += width_step;
      }
      for (int yn = y; yn <= y + width; yn++) {
        sm->tiles[x][yn].set_type(ter_id);
      }
    }
  }
}
