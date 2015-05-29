#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "camera-fwd.hpp"

#include "entity-item.hpp"

#include <SDL_opengl.h>

#include "entity-manager.hpp"
#include "gl-vector3.hpp"
#include "ini-manager.hpp"
#include "terrain-map-fwd.hpp"
#include "world-fwd.hpp"

class camera : public entity_item {
public:
    camera(world const &world);
    virtual ~camera();

    void init(terrain_map const &terrain_map,
              entity_manager const &entity_mgr,
              ini_manager const &ini_mgr);

    void term();

    void yaw(GLfloat delta);
    void pitch(GLfloat delta);
    void pan(GLfloat delta);
    void forward(GLfloat delta);
    void selection_pitch(GLfloat delta);
    void selection_yaw(GLfloat delta);
    void selection_zoom(GLfloat delta);

    gl_vector3 get_position() const;
    gl_vector3 get_angle() const;

    void set_position(gl_vector3 new_pos);
    void set_angle(gl_vector3 new_angle);

    virtual void update();
    virtual void render();

private:
    gl_vector3 angle_;
    gl_vector3 position_;
    GLfloat movement_;
    GLboolean moving_;
    GLfloat eye_height_;
    GLint max_pitch_;

    world const &world_;
    terrain_map const *terrain_map_;
    entity_manager const *entity_mgr_;
    ini_manager const *ini_mgr_;
};

#endif
