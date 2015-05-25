#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "world-fwd.hpp"

#include <SDL_opengl.h>

#include "camera-fwd.hpp"
#include "gl-quat.hpp"
#include "gl-rgba.hpp"
#include "gl-vector3.hpp"
#include "ini-manager.hpp"
#include "mouse-pointer.hpp"
#include "sky.hpp"
#include "terrain.hpp"
#include "terrain-map-fwd.hpp"
#include "terrain-texture.hpp"
#include "texture.hpp"

class world {
public:
    world();
    virtual ~world();

    void init();
    void update();
    void term();
    
    gl_vector3 get_light_vector() const;
    GLfloat get_fade() const;
    gl_quat get_light_quat() const;
    gl_rgba get_light_color() const;
    gl_rgba get_fog_color() const;
    gl_rgba get_ambient_color() const;

private:
    ini_manager *ini_mgr_;
    texture *texture_;
    camera *camera_;
    sky *sky_entity_;
    terrain_map *map_;
    mouse_pointer *pointer_;
    terrain_texture *terrain_texture_;
    terrain *terrain_;

    gl_vector3 light_vector_;
    gl_rgba light_color_;
    gl_rgba ambient_color_;
    gl_rgba fog_color_;
    GLfloat fade_;
    GLint last_update_;
};

#endif
