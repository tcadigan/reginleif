#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "gl-vector3.hpp"

gl_vector3 CameraAngle(void);
void CameraAngleSet(gl_vector3 new_angle);
void CameraAutoToggle();
float CameraDistance(void);
void CameraDistanceSet(float new_distance);
void CameraInit(void);
void CameraNextBehavior(void);
gl_vector3 CameraPosition(void);
void CameraPositionSet(gl_vector3 new_pos);
void CameraReset();
void CameraUpdate(void);
void CameraTerm(void);

void CameraForward(float delta);
void CameraPan(float delta_x);
void CameraPitch(float delta_y);
void CameraYaw(float delta_y);
void CameraVertical(float val);
void CameraLateral(float val);
void CameraMedial(float val);

#endif /* CAMERA_HPP_ */
