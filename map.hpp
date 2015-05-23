#ifndef MAP_HPP_
#define MAP_HPP_

#include "gl-vector3.hpp"
#include "gl-rgba.hpp"

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
