#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "gl-bbox.hpp"
#include "gl-rgba.hpp"
#include "win.hpp"

gl_rgba WorldBloomColor();
usage_t WorldCell(int x, int y);
gl_rgba WorldLightColor(unsigned index);
int WorldLogoIndex();
gl_bbox WorldHotZone();
void WorldInit();
float WorldFade();
void WorldRender();
void WorldReset();
int WorldSceneBegin();
int WorldSceneElapsed();
void WorldTerm();
void WorldUpdate();

#endif /* WORLD_HPP_ */
