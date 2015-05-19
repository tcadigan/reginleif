#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "gl-vector3.hpp"

// Constants
#define EYE_HEIGHT 2.0f
#define MAX_PITCH 85

GLvector3 CameraAngle(void);
void CameraAngleSet(GLvector3 new_angle);
float CameraDistance(void);
void CameraDistanceSet(float new_distance);
void CameraInit(void);
GLvector3 CameraPosition(void);
void CameraPositionSet(GLvector3 new_pos);
void CameraUpdate(void);
void CameraTerm(void);
void CameraForward(float delta);
void CameraSelectionPitch(float delta_y);
void CameraSelectionYaw(float delta_x);
void CameraSelectionZoom(float dleta_y);
void CameraPan(float delta_x);
void CameraPitch(float delta_y);
void CameraYaw (float delta_x);

#endif
