#ifndef VISIBLE_HPP_
#define VISIBLE_HPP_

#include "gl-vector3.hpp"
#include "world.hpp"

static int constexpr GRID_RESOLUTION = 32;
static int constexpr GRID_CELL = GRID_RESOLUTION / 2;
static int constexpr GRID_SIZE = WORLD_SIZE / GRID_RESOLUTION;

void visible_update();
bool visible(gl_vector3 pos);
bool visible(int x, int z);

#endif /* VISIBLE_HPP_ */
