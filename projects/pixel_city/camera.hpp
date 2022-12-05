#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <SDL2/SDL_opengl.h>

#include "gl-vector3.hpp"

gl_vector3 camera_angle();
void camera_angle_set(gl_vector3 new_angle);
void camera_auto_toggle();
float camera_distance();
void camera_distance_set(float new_distance);
void camera_init();
void camera_next_behavior();
gl_vector3 camera_position();
void camera_position_set(gl_vector3 new_pos);
void camera_reset();
void camera_update();
void camera_term();

void camera_forward(float delta);
void camera_pan(float delta_x);
void camera_pitch(float delta_y);
void camera_yaw(float delta_y);
void camera_vertical(float val);
void camera_lateral(float val);
void camera_medial(float val);

#endif /* CAMERA_HPP_ */
