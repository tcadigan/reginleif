#ifndef MOUSE_POINTER_HPP_
#define MOUSE_POINTER_HPP_

#include "camera.hpp"
#include "entity.hpp"
#include "gl-vector3.hpp"
#include "ini-manager.hpp"
#include "point.hpp"
#include "terrain-map.hpp"
#include "texture.hpp"

class mouse_pointer : public entity {
public:
    mouse_pointer(texture &texture,
                  terrain_map const &map,
                  camera const &camera,
                  ini_manager const &ini_mgr);
    virtual ~mouse_pointer();

    void update();
    
    void render();
    point get_selected();

private:
    point draw_grid();

    texture &texture_;
    terrain_map const &map_;
    camera const &camera_;
    ini_manager const &ini_mgr_;
    
    GLint texture_id_;

    gl_vector3 position_;
    point last_mouse_;
    point last_cell_;
    GLfloat pulse_;
    GLint pt_size_;
};

#endif
