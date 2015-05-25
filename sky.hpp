#ifndef SKY_HPP_
#define SKY_HPP_

#include "sky-fwd.hpp"

#include <SDL_opengl.h>

#include "camera.hpp"
#include "entity.hpp"
#include "ini-manager.hpp"
#include "sky-point.hpp"
#include "world.hpp"

class sky : public entity {
public:
    sky(camera const &camera,
        ini_manager const &ini_mgr);

    virtual ~sky();

    void init(world const &world_object);
    void update();
    
    void render(world const &world_object);

private:
    camera const &camera_;
    ini_manager const &ini_mgr_;

    GLint sky_grid_;
    sky_point **grid_;
};

#endif
