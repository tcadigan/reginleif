#ifndef RENDER_HPP_
#define RENDER_HPP_

#include <SDL_opengl.h>

#include "camera.hpp"
#include "ini-manager.hpp"
#include "world-fwd.hpp"

class render {
public:
    render(camera const &cmaera,
           ini_manager const &ini_mgr);
    virtual ~render();

    void init();
    void update(world const &world_object);
    void term();

private:
    void resize();

    camera const &camera_;
    ini_manager const &ini_mgr_;

    // HDC hDc;
    // HGLRC hRC;
    GLint render_width_;
    GLint render_height_;
    GLint render_distance_;
    GLfloat render_aspect_;
    GLushort *buffer_;

};

#endif
