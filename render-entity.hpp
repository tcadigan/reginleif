#ifndef RENDER_ENTITY_HPP_
#define RENDER_ENTITY_HPP_

#include <SDL_opengl.h>

#include "camera.hpp"
#include "ini-manager.hpp"
#include "sun.hpp"
#include "world-fwd.hpp"

class render_entity : public entity {
public:
    render_entity(world const &world_object);
    virtual ~render_entity();

    void init(camera const &camera_object,
              sun const &sub_object,
              ini_manager const &ini_mgr);
    void term();

    virtual void update(); 
    virtual void render();

private:
    void resize();

    world const &world_;
    camera const *camera_;
    sun const *sun_;
    ini_manager const *ini_mgr_;

    // HDC hDc;
    // HGLRC hRC;
    GLint render_width_;
    GLint render_height_;
    GLint render_distance_;
    GLfloat render_aspect_;
    GLushort *buffer_;
};

#endif
