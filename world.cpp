/*
 * world.hpp
 * 2006 Shamus Young
 *
 * This just holds some values for the lightin colors, and the direction of the
 * "sun". REMEMBER that the light vector won't work if you move it off the
 * X-Y plane, so the Z value should always be zero.
 */

#include <SDL.h>
#include <math.h>

#include "glTypes.hpp"
#include "macro.hpp"
#include "map.hpp"
#include "math.hpp"
#include "entity.hpp"
#include "sky.hpp"
#include "pointer.hpp"
#include "terrain.hpp"

static GLvector light_vector = { -0.75f, 0.25f, 0.0f };
static GLrgba light_color = { 2.1f, 2.1f, 0.1f, 1.0f };
static GLrgba ambient_color = { 0.6f, 0.6f, 0.6f, 1.0f };
static GLrgba fog_color = { 0.7f, 0.7f, 0.7f, 1.0f };
static float fade;
static long last_update;
static CPointer *pointer;

GLvector WorldLightVector(void)
{
    return light_vector;
}

float WorldFade(void)
{
    return fade;
}

GLquat WorldLightQuat(void)
{
    GLquat q;

    q.x = light_vector.x;
    q.y = light_vector.y;
    q.z = light_vector.z;
    q.w = 0.0f;

    return q;
}

GLrgba WorldLightColor(void)
{
    return light_color;
}

GLrgba WorldFogColor(void)
{
    return fog_color;
}

GLrgba WorldAmbientColor(void)
{
    return ambient_color;
}

void WorldInit(void)
{
    pointer = new CPointer();
    new CTerrain(MapSize());
    new CSky();
    last_update = SDL_GetTicks();
}

void WorldTerm(void)
{
}

void WorldUpdate(void)
{
    long now;
    long delta;

    now = SDL_GetTicks();
    delta = now - last_update;
    last_update = now;

    fade += ((float)delta / 500.0f);

    if(fade > 1.0f) {
        EntityFadeStart();
        fade = 0.0f;
    }
}

