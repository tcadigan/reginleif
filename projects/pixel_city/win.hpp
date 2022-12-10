#ifndef WIN_HPP_
#define WIN_HPP_

#include <SDL2/SDL.h>

#include <string>
#include <memory>

// Versioning info
static std::string const APP_TITLE("PixelCity");
static int constexpr VERSION_MAJOR = 1;
static int constexpr VERSION_MINOR = 0;
static int constexpr VERSION_REVISION = 10;

// Do we hide scene building behind a loading screen or show it?
static bool constexpr LOADING_SCREEN = true;

// Controls the density of cars
static int constexpr CARS = 500;

// The "dead zone" along the edge of the world, with super-low detail
static int constexpr WORLD_EDGE = 200;

// How often to rebuild the city
static int constexpr RESET_INTERVAL = 999999; // Milliseconds

// How long the screen fade takes when transitioning to a new city
static int constexpr FADE_TIME = 1; // Milliseconds

// Debug ground texture that shows traffic lanes
static bool constexpr SHOW_DEBUG_GROUND = false;

// Controls the amount of space available for buildings.
// Other code is written for assuming this will be a power of two
static int constexpr WORLD_SIZE = 1024;
static int constexpr WORLD_HALF = WORLD_SIZE / 2;

// Bitflags use to track how world space is being used.
enum class usage_t {
    none = 0,
    claim_road = 1,
    claim_walk = 2,
    claim_building = 4,
    map_road_north = 8,
    map_road_south = 16,
    map_road_east = 32,
    map_road_west = 64
};

usage_t operator&(usage_t left, usage_t right);
usage_t operator|(usage_t left, usage_t right);

// Used in orienting roads and cars
enum class direction_t { north, east, south, west };

void win_term();

extern SDL_Window *window;

#endif /* WIN_HPP_ */
