#ifndef SKY_HPP_
#define SKY_HPP_

#include "sky-fwd.hpp"

#include "entity-item.hpp"

#include <SDL_opengl.h>
#include <vector>

#include "camera.hpp"
#include "ini-manager.hpp"
#include "sky-point.hpp"
#include "world-fwd.hpp"

class sky : public entity_item {
public:
    sky(world const &world_object);

    virtual ~sky();

    void init(camera const &camera_object,
              ini_manager const &ini_mgr);

    void term();

    virtual void update();
    virtual void render();

private:
    world const &world_;
    camera const *camera_;
    ini_manager const *ini_mgr_;

    GLint sky_grid_;
    std::vector<sky_point *> grid_;
};

#endif
