/*
 * world.hpp
 * 2006 Shamus Young
 *
 * This just holds some values for the lightin colors, and the direction of the
 * "sun". REMEMBER that the light vector won't work if you move it off the
 * X-Y plane, so the Z value should always be zero.
 */

#include "world.hpp"

#include <SDL.h>

#include "map.hpp"
#include "entity.hpp"
#include "sky.hpp"
#include "mouse-pointer.hpp"
#include "terrain.hpp"

static gl_vector_3d light_vector(-0.75f, 0.25f, 0.0f);
static gl_rgba light_color(2.1f, 2.1f, 0.1f, 1.0f);
static gl_rgba ambient_color(0.6f, 0.6f, 0.6f, 1.0f);
static gl_rgba fog_color(0.7f, 0.7f, 0.7f, 1.0f);
static float fade;
static long last_update;
static mouse_pointer *pointer;

gl_vector_3d world_light_vector(void)
{
    return light_vector;
}

float world_fade(void)
{
    return fade;
}

gl_quat world_light_quat(void)
{
    gl_quat q(light_vector.x_,
              light_vector.y_,
              light_vector.z_,
              0.0f);

    return q;
}

gl_rgba world_light_color(void)
{
    return light_color;
}

gl_rgba world_fog_color(void)
{
    return fog_color;
}

gl_rgba world_ambient_color(void)
{
    return ambient_color;
}

void world_init(void)
{
    pointer = new mouse_pointer();
    new terrain(map_size());
    new sky();
    last_update = SDL_GetTicks();
}

void world_term(void)
{
}

void world_update(void)
{
    long now;
    long delta;

    now = SDL_GetTicks();
    delta = now - last_update;
    last_update = now;

    fade += ((float)delta / 500.0f);

    if(fade > 1.0f) {
        entity_fade_start();
        fade = 0.0f;
    }
}

