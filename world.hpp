#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "gl-vector-3d.hpp"
#include "gl-quat.hpp"
#include "gl-rgba.hpp"

gl_rgba world_ambient_color(void);
float world_fade(void);
gl_rgba world_fog_color(void);
gl_vector_3d world_light_vector(void);
gl_quat world_light_quat(void);
gl_rgba world_light_color(void);
void world_init(void);
void world_term(void);
void world_update(void);

#endif
