#ifndef RENDER_HPP_
#define RENDER_HPP_

#include "gl-rgba.hpp"

bool RenderBloom();
void RenderEffectCycle();
bool RenderFlat();
void RenderFlatToggle();
float RenderFogDistance();
bool RenderFog();
void RenderFogToggle();
void RenderFogFX(float scalar);
void RenderFPSToggle();
void RenderInit();
void RenderLetterboxToggle();
int RenderMaxTextureSize();
void RenderResize();
void RenderTerm();
void RenderUpdate();
bool RenderWireframe();
void RenderWireframeToggle();
void RenderHelpToggle();
void RenderPrint(int x, int y, int font, gl_rgba color, const char *fmt, ...);
void RenderPrint(int line, const char *fmt, ...);

#endif /* RENDER_HPP_ */
