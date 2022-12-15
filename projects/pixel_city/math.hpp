#ifndef __MATH_HPP__
#define __MATH_HPP__

#include <cmath>

static float const PI = std::acos(-1);
static float const DEGREES_TO_RADIANS = PI / 180;

float math_angle(float angle);
float math_angle(float x1, float y1, float x2, float y2);
float math_angle_difference(float a1, float a2);
float math_average(float n1, float n2);
float math_interpolate(float n1, float n2, float delta);
float math_distance(float x1, float y1, float x2, float y2);
float math_distance_2(float x1, float y1, float x2, float y2);
float math_smooth_step(float val, float a, float b);
float math_scalar_curve(float val);

#endif /* __MATH_HPP__ */
