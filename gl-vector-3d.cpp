/*
 * gl-vector-3d.cpp
 * 2006 Shamus Young
 *
 * Functions for dealing with 3D vectors.
 */

#include "gl-vector-3d.hpp"

#include <cmath>
#include "math.hpp"

gl_vector_3d gl_vector_reflect(gl_vector_3d ray, gl_vector_3d normal)
{
    float dot;

    dot = gl_vector_dot_product(ray, normal);
    
    return gl_vector_subtract(ray, gl_vector_scale(normal, 2.0f * dot));
}

gl_vector_3d::gl_vector_3d(float x, float y, float z)
    : x_(x)
    , y_(y)
    , z_(z)
{
}

gl_vector_3d gl_vector_interpolate(gl_vector_3d v1, 
                                   gl_vector_3d v2,
                                   float scalar)
{
    gl_vector_3d result(math_interpolate(v1.x_, v2.x_, scalar),
                        math_interpolate(v1.y_, v2.y_, scalar),
                        math_interpolate(v1.z_, v2.z_, scalar));

    return result;
}

gl_vector_3d gl_vector_subtract(gl_vector_3d v1, gl_vector_3d v2)
{
    gl_vector_3d result(v1.x_ - v2.x_,
                        v1.y_ - v2.y_,
                        v1.z_ - v2.z_);
    
    return result;
}

gl_vector_3d gl_vector_add(gl_vector_3d v1, gl_vector_3d v2)
{
    gl_vector_3d result(v1.x_ + v2.x_,
                        v1.y_ + v2.y_,
                        v1.z_ + v2.z_);

    return result;
}

float gl_vector_length(gl_vector_3d v)
{
    return (float)sqrt((v.x_ * v.x_) + (v.y_ * v.y_) + (v.z_ * v.z_));
}

float gl_vector_dot_product(gl_vector_3d v1, gl_vector_3d v2)
{
    return ((v1.x_ * v2.x_) + (v1.y_ * v2.y_) + (v1.z_ * v2.z_));
}

gl_vector_3d gl_vector_cross_product(gl_vector_3d v1, gl_vector_3d v2)
{
    gl_vector_3d result((v1.y_ * v2.z_) - (v2.y_ * v1.z_),
                        (v1.z_ * v2.x_) - (v2.z_ * v1.x_),
                        (v1.x_ * v2.y_) - (v2.x_ * v1.y_));

    return result;
}

gl_vector_3d gl_vector_invert(gl_vector_3d v)
{
    v.x_ *= -v.x_;
    v.y_ *= -v.y_;
    v.z_ *= -v.z_;

    return v;
}

gl_vector_3d gl_vector_scale(gl_vector_3d v, float scale)
{
    v.x_ *= scale;
    v.y_ *= scale;
    v.z_ *= scale;

    return v;
}

gl_vector_3d gl_vector_normalize(gl_vector_3d v)
{
    float length;
    
    length = gl_vector_length(v);

    if(length < 0.000001f) {
        return v;
    }

    return gl_vector_scale(v, 1.0f / length);
}

gl_vector_3d::gl_vector_3d()
{
}

gl_vector_3d::~gl_vector_3d()
{
}
