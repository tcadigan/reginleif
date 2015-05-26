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
    , camera_(new camera(*this))
    , sky_entity_(new sky(*this))
    , terrain_map_entity_(new terrain_map(*this))
    , mouse_pointer_entity_(new mouse_pointer(*this, *texture_))
    , terrain_texture_(new terrain_texture(*this))
    , terrain_entity_(new terrain(*this))
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
    delete terrain_map_entity_;
    delete mouse_pointer_entity_;
    delete terrain_texture_;
    delete terrain_entity_;
}

void world::init()
{
    // ini_manager needs:
    //     <nothing>
    ini_mgr_->init();

    // Camera needs:
    //     terrain_map
    //     ini_manager
    camera_->init(*terrain_map_entity_, *ini_mgr_);

    // Terrain Map needs:
    //     camera (Completed)
    //     ini_manager
    terrain_map_entity_->init(*camera_, *ini_mgr_);

    // Sky needs:
    //     camera
    //     ini_manager
    sky_entity_->init(*camera_, *ini_mgr_);

    // Mouse Pointer needs:
    //     terrain map
    //     camera
    //     ini_manager
    mouse_pointer_entity_->init(*terrain_map_entity_, *camera_, *ini_mgr_);

    // Terrain Texture needs:
    //     texture
    //     terrain map
    //     camera
    //     ini_manager
    terrain_texture_->init(*texture_, *terrain_map_entity_, *camera_, *ini_mgr_);

    // Terrain needs:
    //     terrain texture
    //     terrain map
    //     camera
    //     ini_manager
    terrain_entity_->init(*terrain_texture_,
                          *terrain_map_entity_,
                          *camera_, 
                          *ini_mgr_);

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


