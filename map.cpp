#include "map.h"
#include "globals.h"

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
  if (id < 0 || id >= TERRAIN_POOL.size())
    return;

  set_type(TERRAIN_POOL[id]);
}

void tile::set_type(terrain_type* _type)
{
  type = _type;
  transforms.clear();
  for (int i = 0; i < TRANS_MAX; i++) {
    transforms.push_back( type->transformations.resistance );
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

submap::~submap();
{
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
    tile* cur = tiles[x][y];
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
}

map::~map()
{
}
