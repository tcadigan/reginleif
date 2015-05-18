#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "gl-vector3.hpp"
#include "gl-quat.hpp"
#include "gl-rgba.hpp"

GLrgba WorldAmbientColor(void);
float WorldFade(void);
GLrgba WorldFogColor(void);
GLvector3 WorldLightVector(void);
GLquat WorldLightQuat(void);
GLrgba WorldLightColor(void);
void WorldInit(void);
void WorldTerm(void);
void WorldUpdate(void);

#endif
