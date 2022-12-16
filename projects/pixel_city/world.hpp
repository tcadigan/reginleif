#ifndef __WORLD_HPP__
#define __WORLD_HPP__

#include "gl-bbox.hpp"
#include "gl-rgba.hpp"

#include <chrono>

// Do we hide scene building behind a loading screen or show it?
static bool constexpr LOADING_SCREEN = true;

// Controls the density of cars
static int constexpr CARS = 500;

// The "dead zone" along the edge of the world, with super-low detail
static int constexpr WORLD_EDGE = 200;

// How often to rebuild the city
static std::chrono::milliseconds RESET_INTERVAL(999999);

// How long the screen fade takes when transitioning to a new city
static std::chrono::milliseconds FADE_TIME(1);

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

gl_rgba world_bloom_color();
usage_t world_cell(int x, int y);
gl_rgba world_light_color(unsigned index);
int world_logo_index();
gl_bbox world_hot_zone();
void world_init();
float world_fade();
void world_render();
void world_reset();
int world_scene_begin();
int world_scene_elapsed();
void world_term();
void world_update();

#endif /* __WORLD_HPP__ */
