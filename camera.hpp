#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "gl-vector-3d.hpp"

using namespace std;

gl_vector_3d camera_angle(void);
void camera_angle_set(gl_vector_3d new_angle);
float camera_distance(void);
void camera_distance_set(float new_distance);
void camera_init(void);
gl_vector_3d camera_position(void);
void camera_position_set(gl_vector_3d new_pos);
void camera_update(void);
void camera_term(void);
void camera_forward(float delta);
void camera_selection_pitch(float delta_y);
void camera_selection_yaw(float delta_x);
void camera_selection_zoom(float dleta_y);
void camera_pan(float delta_x);
void camera_pitch(float delta_y);
void camera_yaw (float delta_x);

#endif
