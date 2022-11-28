#ifndef MATH_HPP_
#define MATH_HPP_

#include <SDL2/SDL_opengl.h>

GLfloat math_angle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLfloat math_angle_difference(GLfloat a1, GLfloat a2);
GLfloat math_average(GLfloat n1, GLfloat n2);
GLfloat math_interpolate(GLfloat n1, GLfloat n2, GLfloat delta);
GLfloat math_line_distance(GLfloat x1, 
                           GLfloat y1,
                           GLfloat x2,
                           GLfloat y2,
                           GLfloat px,
                           GLfloat py);

GLfloat math_distance(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLfloat math_distance_squared(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLfloat math_smooth_step(GLfloat val, GLfloat a, GLfloat b);

#endif
