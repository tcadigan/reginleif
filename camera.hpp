#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "camera-fwd.hpp"

#include <SDL_opengl.h>

#include "gl-vector3.hpp"
#include "ini-manager.hpp"
#include "terrain-map-fwd.hpp"

using namespace std;

class camera {
public:
    camera(ini_manager const &ini_mgr);
    virtual ~camera();

    void init();
    void update(terrain_map const &map);
    void term();

    void yaw(GLfloat delta);
    void pitch(GLfloat delta);
    void pan(GLfloat delta);
    void forward(GLfloat delta);
    // void selection_pitch(GLfloat delta);
    // void selection_yaw(GLfloat delta);
    // void selection_zoom(GLfloat delta);

    gl_vector3 get_position() const;
    gl_vector3 get_angle() const;

    void set_position(gl_vector3 new_pos, terrain_map const &map);
    void set_angle(gl_vector3 new_angle);

private:
    gl_vector3 angle_;
    gl_vector3 position_;
    GLfloat movement_;
    GLboolean moving_;
    GLfloat eye_height_;
    GLint max_pitch_;

    ini_manager const &ini_mgr_;
};

#endif
