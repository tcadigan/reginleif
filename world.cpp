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

world::world()
    : ini_mgr_(new ini_manager())
    , texture_(new texture())
    , camera_(new camera(*ini_mgr_))
    , sky_entity_(new sky(*camera_, *ini_mgr_))
    , map_(new terrain_map(*camera_, *ini_mgr_))
    , pointer_(new mouse_pointer(*texture_, *map_, *camera_, *ini_mgr_))
    , terrain_texture_(new terrain_texture(*texture_, *map_, *camera_, *ini_mgr_))
    , terrain_(new terrain(map_->get_size(), *terrain_texture_, *map_, *camera_, *ini_mgr_))
    , light_vector_(gl_vector3(-0.75f, 0.25f, 0.0f))
    , light_color_(gl_rgba(2.1f, 2.1f, 0.1f, 1.0f))
    , ambient_color_(gl_rgba(0.6f, 0.6f, 0.6f, 1.0f))
    , fog_color_(gl_rgba(0.7f, 0.7f, 0.7f, 1.0f))
{
}

world::~world()
{
    delete ini_mgr_;
    delete texture_;
    delete camera_;
    delete sky_entity_;
    delete map_;
    delete pointer_;
    delete terrain_texture_;
    delete terrain_;
}

void world::init()
{
    last_update_ = SDL_GetTicks();
}

void world::update()
{
    long now;
    long delta;
    
    now = SDL_GetTicks();
    delta = now - last_update_;
    last_update_ = now;

    fade_ += ((float)delta / 500.0f);

    if(fade_ > 1.0f) {
        entity_fade_start();
        fade_ = 0.0f;
    }
}

void world::term()
{
}

gl_vector3 world::get_light_vector() const
{
    return light_vector_;
}

GLfloat world::get_fade() const
{
    return fade_;
}

gl_quat world::get_light_quat() const
{
    return gl_quat(light_vector_.get_x(),
                   light_vector_.get_y(),
                   light_vector_.get_z(),
                   0.0f);
}

gl_rgba world::get_light_color() const
{
    return light_color_;
}

gl_rgba world::get_fog_color() const
{
    return fog_color_;
}

gl_rgba world::get_ambient_color() const
{
    return ambient_color_;
}


