#include "terrain.h"
#include "globals.h"

#define SUBMAP_SIZE 50

enum map_type
{
 MAP_NULL,
// Test
 MAP_TEST,
// In the city
 MAP_CITY,
 MAP_LABYRINTH,
 MAP_CITADEL,
 MAP_PARK,
 MAP_SEWERS,
// Outside the city
 MAP_WOODS,
 MAP_WASTES,
 MAP_MISTS,
 MAP_SEA,

 MAP_MAX
};

struct tile
{
  tile();
  ~tile();

  void set_type(int id);
  void set_type(terrain_type *_type);

  void apply_transformation(transform_type type, int amount);

  terrain_type *type;
  std::vector<int> transforms;
  
};

struct submap
{
  submap();
  ~submap();

  void apply_transformation(int x, int y, transform_type type, int amount);
  void process_transformations();

  tile tiles[SUBMAP_SIZE][SUBMAP_SIZE];
  std::vector<int> unprocessed_transformations;
};

struct transformer
{
  transform_type type;
  int posx;
  int posy;
};

class map
{
 public:
// Constructors
  map();
  ~map();

// Accessors
  tile& ter(int x, int y);
  void draw(Window *w, int origx, int origy, int sight_dist);

// Mutators
  void generate(map_type type);
  void update();
  void resize(int x, int y);
  void apply_transformation(int x, int y, transform_type type, int amount);
  void process_transformations();

 private:
  submap submaps[3][3];
  
  bool bounded;
  tile nulltile;
};
