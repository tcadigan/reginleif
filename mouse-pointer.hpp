#ifndef MOUSE_POINTER_HPP_
#define MOUSE_POINTER_HPP_

#include "camera.hpp"
#include "entity.hpp"
#include "gl-vector3.hpp"
#include "ini-manager.hpp"
#include "point.hpp"
#include "terrain-map.hpp"
#include "texture.hpp"
#include "world-fwd.hpp"

class mouse_pointer : public entity {
public:
    mouse_pointer(world const &world_object,
                  texture &texture);
    virtual ~mouse_pointer();

    void init(terrain_map const &terrain_map_entity,
              camera const &camera_object,
              ini_manager const &ini_mgr);
    void update();
    void term();
    
    void render();
    point get_selected();

private:
    point draw_grid();

    world const &world_;
    GLint texture_id_;
    
    terrain_map const *map_;
    camera const *camera_;
    ini_manager const *ini_mgr_;
    

    gl_vector3 position_;
    point last_mouse_;
    point last_cell_;
    GLfloat pulse_;
    GLint pt_size_;
};

#endif
