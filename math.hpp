#ifndef MATH_HPP_
#define MATH_HPP_

float math_angle(float x1, float y1, float x2, float y2);
float math_angle_difference(float a1, float a2);
float math_average(float n1, float n2);
float math_interpolate(float n1, float n2, float delta);
float math_line_distance(float x1, 
                        float y1,
                        float x2,
                        float y2,
                        float px,
                        float py);

float math_distance(float x1, float y1, float x2, float y2);
float math_distance_squared(float x1, float y1, float x2, float y2);
float math_smooth_step(float val, float a, float b);

#endif
