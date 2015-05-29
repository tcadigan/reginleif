#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "world-fwd.hpp"

#include "entity-item.hpp"

#include <SDL_opengl.h>

#include "camera-fwd.hpp"
#include "entity-manager.hpp"
#include "gl-rgba.hpp"
#include "ini-manager.hpp"
#include "mouse-pointer.hpp"
#include "sky.hpp"
#include "sun.hpp"
#include "terrain-entity.hpp"
#include "terrain-map-fwd.hpp"
#include "terrain-texture.hpp"
#include "texture-manager.hpp"

class world : public entity_item {
public:
    world();
    virtual ~world();

    void init();
    void term();
    
    GLfloat get_fade() const;
    gl_rgba get_fog_color() const;
    gl_rgba get_ambient_color() const;

    virtual void update();
    virtual void render();

private:
    ini_manager *ini_mgr_;
    entity_manager *entity_mgr_;
    texture_manager *texture_mgr_;
    camera *camera_;
    sky *sky_entity_;
    terrain_map *terrain_map_entity_;
    mouse_pointer *mouse_pointer_entity_;
    terrain_texture *terrain_texture_;
    terrain_entity *terrain_entity_;
    sun *sun_;
    
    gl_rgba ambient_color_;
    gl_rgba fog_color_;
    GLfloat fade_;
    GLint last_update_;
};

#endif
