#ifndef MAP_HPP_
#define MAP_HPP_

#include "gl-vector3.hpp"
#include "gl-rgba.hpp"

// This defines how many zones the terrain is broken into
// There are lots of important tradeoffs going on here
// This affects the density of zones, zone textures, the time
// spent compiling terrain, and many other minor details
#define ZONE_GRID 8

// This defines how big the map is. This MUST be a power of 2
#define MAP_AREA 1024

// This is the name of the bitmap to use when generating data
#define MAP_IMAGE "terrain_map6.bmp"

// This is the name of the raw data file where we dump the terrain data
// so that we don't have to generate it every time we run the program
#define MAP_FILE "map.dat"

// We smooth out incoming elevation data. This is the radius of the circle
// used in blending. Larger falues will make the hills smooter. A value of 0
// will disable the smoothing, making the terrain more jagged and higher poly.
#define BLEND_RANGE 3

// How many milliseconds to spend updating the lighting and shadows
// Keep in mind we don't need much
#define MAP_UPDATE_TIME 1

// This controls the radius of the spherical area around the camera where
// terrain data is concentrated. That is, stuff right next to the camera is
// the highest detail, and will diminish until it reaches FAR_VIEW, where
// detail is lowest.
#define FAR_VIEW 200

// How high to make the hills. You'll need to tune this if you use different
// terrain data
#define TERRAIN_SCALE 90

// Set this to 1 to make it re-generate terrain data instead of loading the
// data file each time.
#define FORCE_REBUILD 0

enum {
    LAYER_GRASS,
    LAYER_LOWGRASS,
    LAYER_DIRT,
    LAYER_ROCK,
    LAYER_SAND,
    LAYER_COUNT,
    LAYER_LIGHTING,
    LAYER_SPECIAL,
};

GLrgba MapLight(int x, int y);
float MapDistance(int x, int y);
float MapDot(int x, int y);
float MapElevation(float x, float y);
float MapLayer(int x, int y, int layer);
GLvector3 MapNormal(int x, int y);
GLvector3 MapPosition(int x, int y);
float MapElevation(int x, int y);
void MapInit(void);
void MapTerm(void);
void MapUpdate(void);
int MapSize(void);

#endif
