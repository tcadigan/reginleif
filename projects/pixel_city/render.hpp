#ifndef __RENDER_HPP__
#define __RENDER_HPP__

#include "gl-rgba.hpp"

bool render_bloom();
void render_effect_cycle();
bool render_flat();
void render_flat_toggle();
float render_fog_distance();
bool render_fog();
void render_fog_toggle();
void render_fog_fx(float scalar);
void render_fps_toggle();
void render_init();
void render_letterbox_toggle();
int render_max_texture_size();
void render_resize();
void render_term();
void render_update();
bool render_wireframe();
void render_wireframe_toggle();
void render_help_toggle();
void render_print(int x, int y, int font, gl_rgba color, const char *fmt, ...);
void render_print(int line, const char *fmt, ...);

#endif /* __RENDER_HPP__ */
