#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "gl-bbox.hpp"
#include "gl-rgba.hpp"
#include "win.hpp"

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

#endif /* WORLD_HPP_ */
