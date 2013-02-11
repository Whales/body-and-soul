#include <fstream>
#include "map.h"
#include "globals.h"
#include "rng.h"
#include "stringfunc.h"
#include "geometry.h"

#define SGN(a) (((a)<0) ? -1 : 1)
#define INBOUNDS(x,y) \
  ((x) >= 0 && (y) >= 0 && (x) < SUBMAP_SIZE * 3 && (y) < SUBMAP_SIZE * 3)

tile::tile()
{
  type = NULL;
  for (int i = 0; i < TRANS_MAX; i++) {
    transforms.push_back(0);
  }
}

tile::~tile()
{
}

void tile::set_type(int id)
{
  if (id < 0 || id >= TERRAIN_POOL.size()) {
    debugmsg("Bad set_type");
    return;
  }

  set_type(TERRAIN_POOL[id]);
}

void tile::set_type(terrain_type* _type)
{
  type = _type;
  transforms.clear();
  for (int i = 0; i < TRANS_MAX && i < type->transformations.size(); i++) {
    transforms.push_back( type->transformations[i].resistance );
  }
}

void tile::apply_transformation(transform_type type, int amount)
{
  transforms[type] -= amount;
}

submap::submap()
{
  for (int i = 0; i < SUBMAP_SIZE; i++) {
    for (int j = 0; j < SUBMAP_SIZE; j++)
      tiles[i][j].set_type(TERRAIN_POOL[0]);
  }
}

submap::~submap()
{
}

tile& submap::ter(int x, int y)
{
  if (x < 0 || y < 0 || x >= SUBMAP_SIZE || y >= SUBMAP_SIZE) {
    nulltile = tile();
    nulltile.set_type(0);
    return nulltile;
  }
  return tiles[x][y];
}

void submap::apply_transformation(int x, int y, transform_type type, int amount)
{
  if (x < 0 || x >= SUBMAP_SIZE || y < 0 || y >= SUBMAP_SIZE)
    return;

  tiles[x][y].apply_transformation(type, amount);
  unprocessed_transformations.push_back(x + y * SUBMAP_SIZE);
}

void submap::process_transformations()
{
  if (unprocessed_transformations.empty())
    return;

  for (int i = 0; i < unprocessed_transformations.size(); i++) {
    int x = unprocessed_transformations[i] % SUBMAP_SIZE;
    int y = unprocessed_transformations[i] / SUBMAP_SIZE;
    tile* cur = &(tiles[x][y]);
// Since we might be affected by 2 or more transformations concurrently, we
// store a list of the possible results, and a "lowest" variable which must be
// tied to be added to the list, or beaten to clear out the list
    std::vector<int> results;
    int lowest = -1;
    for (int j = 0; j < TRANS_MAX; j++) {
// Resulting terrain - 0 if no transformation takes place
      int result = cur->type->transformations[j].result;
      int amount = cur->transforms[j]; // If < 0, we transform
      if (amount < 0) { // We're affected by the transform
        if (result == 0) { // No actual transformation; just clear it out
          cur->transforms[j] = 0;
        } else { // The transform changes our type
          if (amount < lowest) { // A new strongest transformation!
            results.clear();
            results.push_back(result);
            lowest = amount;
          } else if (amount == lowest) { // Tied for best!
            results.push_back(result);
          }
        }
      }
    }
    if (!results.empty()) { // We've got to change!
      cur->set_type( results[rng(0, results.size() - 1)] );
    }
  }

  unprocessed_transformations.clear();
}

map::map()
{
  nulltile.set_type(0);
}

map::~map()
{
}

tile& map::ter(int x, int y)
{
  if (x < 0 || y < 0 || x >= SUBMAP_SIZE * 3 || y >= SUBMAP_SIZE * 3) {
    nulltile = tile();
    nulltile.set_type(0);
    return nulltile;
  }

  int smx = x / SUBMAP_SIZE, smy = y / SUBMAP_SIZE;
  x %= SUBMAP_SIZE;
  y %= SUBMAP_SIZE;
  if (smx < 0 || smy < 0 || smx >= 3 || smy >= 3 ||
      x < 0 || y < 0 || x >= SUBMAP_SIZE * 3 || y >= SUBMAP_SIZE * 3) {
    debugmsg("Nope!");
  }

  return submaps[smx][smy].tiles[x][y];
}

void map::draw(Window *w, int origx, int origy, int sight_dist,
               glyph orig_glyph)
{
  int xdim = w->sizex() / 2, ydim = w->sizey() / 2;
  for (int x = origx - xdim; x <= origx + xdim; x++) {
    for (int y = origy - ydim; y <= origy + ydim; y++) {
      glyph print;
      if (!ter(x, y).type) {
        debugmsg("badtype");
      }
      //if (x != 191 || y != 187)
      if (x == origx && y == origy) {
        w->putglyph(x + xdim - origx, y + ydim - origy, orig_glyph);
      } else if (sees(origx, origy, x, y, sight_dist)) {
        print = ter(x, y).type->symbol;
        w->putglyph(x + xdim - origx, y + ydim - origy, print);
      }
    }
  }
}

/*
bool map::sees(int origx, int origy, int destx, int desty, int sight_dist)
{
  std::vector<point> line = line_to(origx, origy, destx, desty);
  if (line.size() > sight_dist) { // Don't even bother iterating over it.
    return false;
  }
  if (line.size() == 1) { // Trivial (sight_dist = 0 already returned false)
    return true;
  }
  for (int i = 0; i < line.size() - 1; i++) {
    terrain_type* tertype = ter( line[i].x, line[i].y ).type;
    if (tertype->sight_cost < 0) {
      return false;
    } else if (tertype->sight_cost == 0) {
      sight_dist--;
    } else {
      sight_dist -= tertype->sight_cost;
    }
    if (sight_dist <= 0) {
      return false;
    }
  }
  return true;
}
*/
bool map::sees(int Fx, int Fy, int Tx, int Ty, int range, int *tc)
{
  bool deltc = false;
  if (!tc) {
    tc = new int;
    deltc = true;
  }
  int dx = Tx - Fx;
  int dy = Ty - Fy;
  int ax = abs(dx) << 1;
  int ay = abs(dy) << 1;
  int sx = SGN(dx);
  int sy = SGN(dy);
  int x = Fx;
  int y = Fy;
  int t = 0;
  int st;
 
  if (range >= 0 && (abs(dx) > range || abs(dy) > range)) {
    if (deltc) {
      delete tc;
    }
    return false;	// Out of range!
  }
  if (ax > ay) { // Mostly-horizontal line
    st = SGN(ay - (ax >> 1));
// Doing it "backwards" prioritizes straight lines before diagonal.
// This will help avoid creating a string of zombies behind you and will
// promote "mobbing" behavior (zombies surround you to beat on you)
    for ((*tc) = abs(ax); (*tc) >= -1; (*tc)--) {
      t = (*tc) * st;
      x = Fx;
      y = Fy;
      int sight_left = range;
      do {
        if (t > 0) {
          y += sy;
          t -= ax;
        }
        x += sx;
        t += ay;
        if (x == Tx && y == Ty) {
          (*tc) *= st;
          if (deltc) {
            delete tc;
          }
          return true;
        } else {
          int cost = ter(x, y).type->sight_cost;
          if (cost < 0) {
            sight_left = 0;
          } else {
            sight_left -= (cost == 0 ? 1 : cost);
          }
        }
      } while (sight_left > 0 && (INBOUNDS(x,y)));
    }
    if (deltc) {
      delete tc;
    }
    return false;
  } else { // Same as above, for mostly-vertical lines
    st = SGN(ax - (ay >> 1));
    for ((*tc) = abs(ay); (*tc) >= -1; (*tc)--) {
      t = (*tc) * st;
      x = Fx;
      y = Fy;
      int sight_left = range;
      do {
        if (t > 0) {
          x += sx;
          t -= ay;
        }
        y += sy;
        t += ax;
        if (x == Tx && y == Ty) {
          (*tc) *= st;
          if (deltc) {
            delete tc;
          }
          return true;
        } else {
          int cost = ter(x, y).type->sight_cost;
          if (cost < 0) {
            sight_left = 0;
          } else {
            sight_left -= (cost == 0 ? 1 : cost);
          }
        }
      } while (sight_left > 0 && (INBOUNDS(x,y)));
    }
    if (deltc) {
      delete tc;
    }
    return false;
  }
  if (deltc) {
    delete tc;
  }
  return false; // Shouldn't ever be reached, but there it is.
}

void map::generate(map_type type)
{
  if (type == MAP_TEST) {
    for (int x = 0; x < SUBMAP_SIZE * 3; x++) {
      for (int y = 0; y < SUBMAP_SIZE * 3; y++) {
        if (x % 10 == 0 || x % 10 == 1) {
          ter(x, y).set_type(TER_FLOOR);
        } else {
          ter(x, y).set_type( y % (TERRAIN_POOL.size() - 1) + 1);
        }
      }
    }
  } else {
    std::vector<mapgen_spec*> choices = MAPGEN_POOL[type];
    if (choices.empty()) {
      debugmsg("No mapgen specifications for %s found.",
               map_type_name(type).c_str());
      generate(MAP_TEST);
      return;
    }
    mapgen_spec* spec = choices[rng(0, choices.size() - 1)];
    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 3; y++) {
        submap* north = NULL;
        submap* west  = NULL;
        if (x > 0) {
          west  = &(submaps[x - 1][y]);
        }
        if (y > 0) {
          north = &(submaps[x][y - 1]);
        }
        submaps[x][y].generate(spec, north, NULL, NULL, west);
      }
    }
  }
}

void map::apply_transformation(int x, int y, transform_type type, int amount)
{
  int smx = x / SUBMAP_SIZE, smy = y / SUBMAP_SIZE;
  x %= SUBMAP_SIZE;
  y %= SUBMAP_SIZE;
  submaps[smx][smy].apply_transformation(x, y, type, amount);
}

void map::process_transformations()
{
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      submaps[x][y].process_transformations();
    }
  }
}

map_type lookup_map_type(std::string name)
{
  for (int i = 0; i < MAP_MAX; i++) {
    if (no_caps(name) == no_caps(map_type_name( map_type(i) ))) {
      return map_type(i);
    }
  }
  map_type tmp = map_type();
  return tmp;
}

std::string map_type_name(map_type type)
{
  switch (type) {
    case MAP_MAX:
    case MAP_NULL:      return "Null";
    case MAP_TEST:      return "Test";
    case MAP_CITY:      return "City";
    case MAP_LABYRINTH: return "Labyrinth";
    case MAP_CITADEL:   return "Citadel";
    case MAP_PARK:      return "Park";
    case MAP_SEWERS:    return "Sewers";
    case MAP_WOODS:     return "Woods";
    case MAP_WASTES:    return "Wastes";
    case MAP_MISTS:     return "Mists";
    case MAP_SEA:       return "Sea";
    case MAP_CAVERNS:   return "Caverns";
    default:            return "ForgotIt";
  }
  return "Escaped switch";
}

