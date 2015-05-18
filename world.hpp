#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "glTypes.hpp"

GLrgba WorldAmbientColor(void);
float WorldFade(void);
GLrgba WorldFogColor(void);
GLvector WorldLightVector(void);
GLquat WorldLightQuat(void);
GLrgba WorldLightColor(void);
void WorldInit(void);
void WorldTerm(void);
void WorldUpdate(void);

#endif
