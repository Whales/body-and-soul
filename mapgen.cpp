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

/*
void set_connectors(submap* neighbor, int ter_id,
                    int xs, int ys, int &pos, int &width);
*/
std::vector<map_connect> get_connectors(submap* neighbor, int ter_id,
                                        int xs, int ys);
std::vector<map_connect> randomize_roads(mapgen_spec_buildings *bg);

void plan_blocks(submap* sm, mapgen_spec_buildings* bg);
void build_block(submap *sm, mapgen_spec_buildings *bg,
                 int x0, int y0, int x1, int y1, int depth = 0);
void build_house(submap* sm, mapgen_spec_buildings* bg,
                 int x0, int y0, int x1, int y1);
void draw_road(submap* sm, bool vertical, int ter_id, 
               int x0, int y0, int width0,
               int x1, int y1, int width1);

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
  while ((dp = readdir(dir))) {
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
        //debugmsg("Invalid maptype %s.", new_type.c_str());
      }
    } else if (id == "gen") {
      std::string gen_id, gen_type_name;
      datastream >> gen_type_name;
      mapgen_type gen_type = lookup_mapgen_type(gen_type_name);

      switch (gen_type) {

        case MAPGEN_NULL:
          //debugmsg("Invalid mapgen type: %s", gen_type_name.c_str());
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
    } else if (id.find("density") == 0) {
      datastream >> density;
    } else if (id.find("numroads") == 0) {
      datastream >> numroads;
    }
  } while (!datastream.eof() && id != "done");
}

void mapgen_spec_labyrinth::load_data(std::istream &datastream)
{
  name = load_to_character(datastream, ":;\n", true);
  std::string id;
// Set all terrain ids to 0; i.e. "clear" or "no change"
  wall_id  = 0;
  floor_id = 0;
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
  std::string tername = load_to_character(datastream, ";,\n", true);
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
          tiles[x][y].set_type(bspec->outside_id);
        }
      }
    }
/* Next, determine where to plunk down roads.  If there's nothing adjacent, we
 * can randomize it; otherwise, we'll need to get connectors.
 */
    int sm_end = SUBMAP_SIZE - 1;
    std::vector<map_connect> roads_north, roads_east, roads_south, roads_west;
    roads_north = (north ? get_connectors(north, bspec->road_id, -1, sm_end) :
                           randomize_roads(bspec));
    roads_east  = (east  ? get_connectors(east,  bspec->road_id, 0, -1) :
                           randomize_roads(bspec));
    roads_south = (south ? get_connectors(south, bspec->road_id, -1, 0) :
                           randomize_roads(bspec));
    roads_west  = (west  ? get_connectors(west,  bspec->road_id, sm_end, -1) :
                           randomize_roads(bspec));

// Connect roads randomly!
    for (int i = 0; i < roads_north.size() && !roads_south.empty(); i++) {
      //int n = rng(0, roads_south.size() - 1);
      draw_road(this, true, bspec->road_id,
                roads_north[i].pos,      0, roads_north[i].width,
                roads_south[i].pos, sm_end, roads_south[i].width);
      //roads_south.erase( roads_south.begin() + n );
    }
      
    for (int i = 0; i < roads_west.size() && !roads_east.empty(); i++) {
      //int n = rng(0, roads_east.size() - 1);
      draw_road(this, false, bspec->road_id,
                     0, roads_west[i].pos, roads_west[i].width,
                sm_end, roads_east[i].pos, roads_east[i].width);
      //roads_east.erase( roads_east.begin() + n );
    }
    plan_blocks(this, bspec);
/*
// Finally, build "blocks" (i.e. where the houses are)
    plan_blocks(this, bspec);
*/
  } break;

  case MAPGEN_LABYRINTH: {
    //mapgen_spec_labyrinth* lspec = static_cast<mapgen_spec_labyrinth*>(spec);
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

std::vector<map_connect> get_connectors(submap* neighbor, int ter_id,
                                        int xs, int ys)
{
  std::vector<map_connect> ret;
  if (!neighbor) {
    return ret;
  }
  int tmppos = -1, tmpwidth = -1;
  for (int i = 0; i < SUBMAP_SIZE; i++) {
    int x = (xs == -1 ? i : xs);
    int y = (ys == -1 ? i : ys);
    if (tmppos == -1) {
      if (neighbor->tiles[x][y].type->uid == ter_id) {
        tmppos = i;
      }
    } else if (neighbor->tiles[x][y].type->uid != ter_id) {
      tmpwidth = i - 1 - tmppos;
      ret.push_back( map_connect(tmppos, tmpwidth) );
      tmppos = -1;
      tmpwidth = -1;
    }
  }
  return ret;
}

std::vector<map_connect> randomize_roads(mapgen_spec_buildings* bg)
{
  std::vector<map_connect> ret;
  int numroads = bg->numroads;
  if (numroads == 0) {
    numroads = 1;
  } else if (10 + numroads * bg->maxroad >= SUBMAP_SIZE) {
    numroads = (SUBMAP_SIZE - 10) / bg->maxroad;
  }
// 10 on either side
  int space_per_road = (SUBMAP_SIZE - 10) / numroads;
  for (int i = 0; i < bg->numroads; i++) {
    int width = rng(bg->minroad, bg->maxroad);
    int pos = rng(5 + space_per_road * i, 5 + space_per_road * (i + 1) - width);
    ret.push_back( map_connect(pos, width) );
  }
  return ret;
}

void draw_road(submap* sm, bool vertical, int ter_id, 
               int x0, int y0, int width0, int x1, int y1, int width1)
{
/* All comments below will, for the sake of brevity and clarity, refer to a
 * vertically-oriented road.  They apply to horizontal ones as well (just switch x
 * and y).
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
  int widthspace = (dWidth <= 1 ? 0 : (diff - 1) / (dWidth - 1));
/* Of course, we don't want to always make the first width change at the very
 * start of the sloping section.  Taking the remainer of (diff-1) / (dWidth-1)
 * gives us the meximum position we can start from.  Randomize!
 */
  int widthoffset = (dWidth <= 1 ? rng(0, diff) :
                                   rng(0, (diff - 1) % (dWidth - 1)));
  int x_step = (x1 > x0 ? 1 : -1);
  int y_step = (y1 > y0 ? 1 : -1);
  int width_step = (width1 > width0 ? 1 : -1);
  int x = x0, y = y0, width = width0;
  if (vertical) {
    for (y = y0; y != y1 + y_step; y += y_step) {
      for (int xn = x; xn <= x + width; xn++) {
        if (xn >= 0 && xn < SUBMAP_SIZE && y >= 0 && y < SUBMAP_SIZE) {
          sm->tiles[xn][y].set_type(ter_id);
        }
      }
      if (y >= slopestart && x != x1) {
        x += x_step;
      }
      if (width != width1 && y >= slopestart + widthoffset &&
        (widthspace == 0 || (y - slopestart - widthoffset) % widthspace == 0)) {
        width += width_step;
      }
    }
  } else {
    for (x = x0; x != x1 + x_step; x += x_step) {
      for (int yn = y; yn <= y + width; yn++) {
        if (yn >= 0 && yn < SUBMAP_SIZE && x >= 0 && x < SUBMAP_SIZE ) {
          sm->tiles[x][yn].set_type(ter_id);
        }
      }
      if (x >= slopestart && y != y1) {
        y += y_step;
      }
      if (width != width1 && x >= slopestart + widthoffset &&
        (widthspace == 0 || (x - slopestart - widthoffset) % widthspace == 0)) {
        width += width_step;
      }
    }
  } 
}

void plan_blocks(submap* sm, mapgen_spec_buildings* bg)
{
  if (bg->wall_id <= 0) {
    return;
  }
  if (bg->road_id <= 0) {
// TODO: Write free-standing blocks
    return;
  }
  int new_ter[SUBMAP_SIZE][SUBMAP_SIZE];
  for (int x = 0; x < SUBMAP_SIZE; x++) {
    for (int y = 0; y < SUBMAP_SIZE; y++) {
      if (sm->ter(x, y).type->uid == bg->outside_id) {
        int num_roads = 0;
        for (int rx = x - 1; rx <= x + 1; rx++) {
          for (int ry = y - 1; ry <= y + 1; ry++) {
            if (sm->ter(rx, ry).type->uid == bg->road_id) {
              num_roads++;
            }
          }
        }
        if (num_roads >= 1 && num_roads <= 5) {
          new_ter[x][y] = bg->wall_id;
        } else {
          new_ter[x][y] = sm->ter(x, y).type->uid;
        }
      } else {
        new_ter[x][y] = sm->ter(x, y).type->uid;
      }
    }
  }
  for (int x = 0; x < SUBMAP_SIZE; x++) {
    for (int y = 0; y < SUBMAP_SIZE; y++) {
      sm->ter(x, y).set_type( new_ter[x][y] );
    }
  }
// Second pass: put floors in!
// Third pass: remove extraneous walls
}


/* The algorithm works like this:
 * 1. Start with a block:
 *    ............
 *    ............
 *    ............
 *    ............
 *    ............
 * 2. If it's too small to fit a single house, we're done.
 * 3. Otherwise, if one dimension is just big enough for a house, build as many
 *    as we can:
 *    ............
 *    .####..####.
 *    .####..####.
 *    .####..####.
 *    ............
 * 4. If both dimensions are big enough for more than one house, then carve off
 *    space on one of the shorter sides.  The carved off space should have a
 *    depth that's just big enough for one house:
 *    lllrrrrrrrrr
 *    lllrrrrrrrrr
 *    lllrrrrrrrrr
 *    lllrrrrrrrrr
 *    lllrrrrrrrrr
 * 5. Recurse with l and r.
 */

void build_block(submap *sm, mapgen_spec_buildings *bg,
                 int x0, int y0, int x1, int y1, int depth)
{
  
// TODO: if very big, split and draw a road in the middle
std::ofstream debugout;
debugout.open("debugblock.txt", std::ios::app);
  if (x1 < x0) {
    int tmp = x0;
    x0 = x1;
    x1 = tmp;
  }
  if (y1 < y0) {
    int tmp = y0;
    y0 = y1;
    y1 = tmp;
  }
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Block: " << x0 << ":" << y0 << " => " << x1 << ":" << y1 << std::endl;
  int dx = abs(x1 - x0), dy = abs(y1 - y0);
  int space_required = bg->minspacing * 2 + bg->minsize;
  int max_space = bg->maxspacing * 2 + bg->maxsize;
  if (dx < space_required || dy < space_required) {
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Too small.\n";
debugout.close();
    return; // Not big enough to fit a building!
  }
  bool x_bigger = (dx > dy || (dx == dy && one_in(2)));
  int dlong  = (x_bigger ? dx : dy), dshort = (x_bigger ? dy : dx);
  if (dshort >= space_required * 2) {
// Even the shorter side is long enough to fit more than one big house.
    if (max_space > dlong - 1) {
      max_space = dlong - 1; 
    }
    int split_dist = rng(space_required, max_space);
    if (x_bigger) {
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "X is bigger.\n";
      if (one_in(2)) { // Carve off the left side
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "carve off left\n";
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 1:\n";
debugout.close();
        build_block(sm, bg, x0, y0, x0 + split_dist - 1, y1, depth + 1);
debugout.open("debugblock.txt", std::ios::app);
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 2:\n";
debugout.close();
        build_block(sm, bg, x0 + split_dist, y0, x1, y1, depth + 1);
      } else { // Carve off the right side
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Carve off right.\n";
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 1:\n";
debugout.close();
        build_block(sm, bg, x0, y0, x1 - split_dist, y1, depth + 1);
debugout.open("debugblock.txt", std::ios::app);
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 2:\n";
debugout.close();
        build_block(sm, bg, x1 - split_dist + 1, y0, x1, y1, depth + 1);
      }
    } else { // y is bigger
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Y is bigger.\n";
      if (one_in(2)) { // Carve off the north side
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Carve off north.\n";
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 1:\n";
debugout.close();
        build_block(sm, bg, x0, y0, x1, y0 + split_dist - 1, depth + 1);
debugout.open("debugblock.txt", std::ios::app);
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 2:\n";
debugout.close();
        build_block(sm, bg, x0, y0 + split_dist, x1, y1, depth + 1);
      } else { // Carve off the south side
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Carve off south.\n";
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 1:\n";
debugout.close();
        build_block(sm, bg, x0, y0, x1, y1 - split_dist, depth + 1);
debugout.open("debugblock.txt", std::ios::app);
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "Split 2:\n";
debugout.close();
        build_block(sm, bg, x0, y1 - split_dist + 1, x1, y1, depth + 1);
      }
    }
  } else { // The shorter side isn't big enough to fit more than one house!
/* Okay, we aren't going to split, since the dy is only big enough for a single
 * house (albeit the largest possible house).  We don't really care about how we
 * fill that y-space, since we know it's big enough for one house, no more, no
 * less.  BUT!  We *do* care about the x-space, since we want to cram as many
 * houses in as possible.
 * To do that, we track "wiggle room."  We get the size of the smallest house,
 * including spacing on one side (don't want to double up on spacing).  Then we
 * divide dx by that size.  The remainder is our "wiggle room," extra space
 * which we can distribute into between-building spaces or wall sizes.
 */
for (int i = 0; i < depth; i++) { debugout << " "; }
debugout << "!!BUILD!!\n";
    int smallest_size = bg->minspacing + bg->minsize;
    int long_available = dlong - bg->minspacing; // Extra space at the end
    int num_houses = long_available / smallest_size;
    int wiggle = long_available % smallest_size;
    std::vector<int> spacing, walls;
    std::vector<int> available; // Which values AREN'T at their max
    for (int i = 0; i < num_houses; i++) {
      spacing.push_back(bg->minspacing);
      walls.push_back(bg->minsize);
      available.push_back(i);
      available.push_back(i + num_houses);
    }
    for (int i = 0; i < wiggle && !available.empty(); i++) {
      int sel = rng(0, available.size() - 1);
      int boost = available[sel];
      //debugmsg("[%d]/%d = %d", sel, available.size(), boost);
      if (boost >= num_houses) {
        boost -= num_houses;
        walls[boost]++;
        if (walls[boost] >= bg->maxsize) {
          available.erase( available.begin() + sel );
        }
      } else {
        spacing[boost]++;
        if (spacing[boost] >= bg->maxspacing) {
          available.erase( available.begin() + sel );
        }
      }
    }
// Now, actually build the houses!
    int longpos = (x_bigger ? x0 : y0);
    for (int i = 0; i < num_houses; i++) {
      longpos += spacing[i];
// Randomize the y-dimensions
      int max_shortsize = bg->maxsize;
      if (max_shortsize > dy - bg->minspacing * 2) {
        max_shortsize = dy - bg->minspacing * 2;
      }
      int shortsize = rng(bg->minsize, max_shortsize);
      int max_shortspace = bg->maxspacing; // We only care about the top
      if (dshort - max_shortspace - shortsize < bg->minspacing) {
        max_shortspace = dshort - bg->minspacing - shortsize;
      }
      int shortspace = rng(bg->minspacing, max_shortspace);

      int hx0, hy0, hx1, hy1;
      if (x_bigger) {
        hx0 = longpos;
        hx1 = longpos + walls[i];
        hy0 = y0 + shortspace;
        hy1 = y0 + shortspace + shortsize;
      } else {
        hx0 = x0 + shortspace;
        hx1 = x0 + shortspace + shortsize;
        hy0 = longpos;
        hy1 = longpos + walls[i];
      }
for (int ii= 0; ii< depth; ii++) { debugout << " "; }
debugout << "House: " << hx0 << ":" << hy0 << "=>" << hx1 << ":" << hy1 << "\n";
      build_house(sm, bg, hx0, hy0, hx1, hy1);
      longpos += walls[i] + 1;
    }
  }
debugout.close();
}

void build_house(submap* sm, mapgen_spec_buildings* bg,
                 int x0, int y0, int x1, int y1)
{
// TODO: vaults in houses, decorations in houses
// TODO: doors :v
  for (int x = x0; x <= x1 && x < SUBMAP_SIZE; x++) {
    for (int y = y0; y <= y1 && y < SUBMAP_SIZE; y++) {
      if (x == x0 || x == x1 || y == y0 || y == y1) {
        if (bg->wall_id > 0) {
          sm->tiles[x][y].set_type(bg->wall_id);
        }
      } else if (bg->floor_id > 0) {
        sm->tiles[x][y].set_type(bg->floor_id);
      }
    }
  }
  //Window w_tmp(0, 0, 80, 24);
/*
  for (int x = 0; x < SUBMAP_SIZE; x++) {
    for (int y = 0; y < SUBMAP_SIZE; y++) {
      glyph print = sm->tiles[x][y].type->symbol;
      w_tmp.putglyph(x, y, print);
    }
  }
*/
  //w_tmp.refresh();
  //getch();
}
