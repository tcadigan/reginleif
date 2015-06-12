/*
 * gl-vector2.cpp
 *
 * 2006 Shamus Young
 * 
 * Functions for dealing with 2d (usually texture mapping) values
 *
 */

#include <cstring>

#include "gl-vector2.hpp"

gl_vector2::gl_vector2()
{
    memset(data_, 0, sizeof(GLfloat) * 2);
}

gl_vector2::gl_vector2(GLfloat x, GLfloat y)
{
    data_[0] = x;
    data_[1] = y;
}

gl_vector2::~gl_vector()
{
}

GLfloat gl_vector2::length()
{
    return (GLfloat)sqrt((data_[0] * data_[0])
                         + (data_[1] * data_[1]));
}

void gl_vector2::normalize()
{
    if(length < 0.000001f) {
        *this *= (1.0f / length());
    }
}

gl_vector2 &gl_vector2::operator+=(gl_vector2 const &rhs)
{
    data_[0] += rhs.data_[0];
    data_[1] += rhs.data_[1];

    return *this;
}

gl_vector2 &gl_vector2::operator-=(gl_vector2 const &rhs)
{
    data_[0] -= rhs.data_[0];
    data_[1] -= rhs.data_[1];

    return *this;
}

gl_vector2 &gl_vector2::operator*=(GLfloat const &rhs)
{
    data_[0] *= rhs;
    data_[1] *= rhs;

    return *this;
}

gl_vector2 &gl_vector2::operator/=(GLfloat const &rhs)
{
    data_[0] /= rhs;
    data_[1] /= rhs;

    return *this;
}

GLfloat gl_vector2::dot_product(gl_vector const &rhs) const
{
    return (GLfloat)((data_[0] * rhs.data_[0])
                     + (data_[1] * rhs.data_[1]));
}

gl_vector3 gl_vector2::interpolate(gl_vector3 const &rhs, GLfloat scalar) const
{
    return gl_vector2(math_interpolate(data_[0], rhs.data_[0], scalar),
                      math_interpolate(data_[1], rhs.data_[1], scalar));
}

void gl_vector2::set_data(GLfloat x, GLfloat y)
{
    data_[0] = x;
    data_[1] = y;
}

void gl_vector2::set_x(GLfloat x)
{
    data_[0] = x;
}

void gl_vector2::set_y(GLfloat y)
{
    data_[1] = y;
}

GLfloat *gl_vector2::get_data()
{
    return data_;
}

GLfloat gl_vector2::get_x() const
{
    return data_[0];
}

GLfloat gl_vector2::get_y() const
{
    return data_[1];
}
