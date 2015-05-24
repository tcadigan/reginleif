#ifndef MAP_HPP_
#define MAP_HPP_

#include "gl-vector-3d.hpp"
#include "gl-rgba.hpp"

enum map_layers {
    LAYER_GRASS,
    LAYER_LOWGRASS,
    LAYER_DIRT,
    LAYER_ROCK,
    LAYER_SAND,
    LAYER_COUNT,
    LAYER_LIGHTING,
    LAYER_SPECIAL,
};

gl_rgba map_light(int x, int y);
float map_distance(int x, int y);
float map_dot(int x, int y);
float map_elevation(float x, float y);
float map_layer(int x, int y, int layer);
gl_vector_3d map_normal(int x, int y);
gl_vector_3d map_position(int x, int y);
float map_elevation(int x, int y);
void map_init(void);
void map_term(void);
void map_update(void);
int map_size(void);

#endif
