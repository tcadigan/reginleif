#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "gl-bbox.hpp"
#include "gl-rgba.hpp"

gl_rgba WorldBloomColor();
char WorldCell(int x, int y);
gl_rgba WorldLightColor(unsigned index);
int WorldLogoIndex();
gl_bbox WorldHotZone();
void WorldInit(void);
float WorldFade(void);
void WorldRender();
void WorldReset(void);
int WorldSceneBegin();
int WorldSceneElapsed();
void WorldTerm(void);
void WorldUpdate(void);

#endif /* WORLD_HPP_ */
