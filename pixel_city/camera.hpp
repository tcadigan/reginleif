#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <SDL_opengl.h>

#include "gl-vector3.hpp"

gl_vector3 camera_angle();
void camera_angle_set(gl_vector3 new_angle);
void camera_auto_toggle();
GLfloat camera_distance();
void camera_distance_set(GLfloat new_distance);
void camera_init();
void camera_next_behavior();
gl_vector3 camera_position();
void camera_position_set(gl_vector3 new_pos);
void camera_reset();
void camera_update();
void camera_term();

void camera_forward(GLfloat delta);
void camera_pan(GLfloat delta_x);
void camera_pitch(GLfloat delta_y);
void camera_yaw(GLfloat delta_y);
void camera_vertical(GLfloat val);
void camera_lateral(GLfloat val);
void camera_medial(GLfloat val);

#endif /* CAMERA_HPP_ */
