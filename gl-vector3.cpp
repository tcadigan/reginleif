/*
 * gl-vector3.cpp
 *
 * 2006 Shamus Young
 *
 * Functions for dealing with 3d vectors
 *
 */

#include "gl-vector3.hpp"

#include <cmath>
#include <cstring>

#include "math.hpp"

gl_vector3::gl_vector3()
{
    memset(data_, 0, sizeof(GLfloat) * 3);
}

gl_vector3::gl_vector3(GLfloat x, Glfloat y, GLfloat z)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
}

gl_vector3::~gl_vector3()
{
}

GLfloat gl_vector3::length()
{
    return (GLfloat)sqrt((data_[0] * data_[0])
                         + (data_[1] * data_[1])
                         + (data_[2] * data_[2]));
}

void gl_vector3::normalize()
{
    if(length() < 0.000001f) {
        *this *= (1.0f / length());
    }
}

void gl_vector3::reflect(gl_vector3 const &normal)
{
    *this -= (normal * (2.0 * dot_product(normal)));
}

gl_vector3 &gl_vector3::operator+=(gl_vector3 const &rhs)
{
    data_[0] += rhs.data_[0];
    data_[1] += rhs.data_[1];
    data_[2] += rhs.data_[2];

    return *this;
}

gl_vector3 &gl_vector3::operator+=(GLfloat const &rhs)
{
    data_[0] += rhs;
    data_[1] += rhs;
    data_[2] += rhs;

    return *this;
}

gl_vector3 &gl_vector3::operator-=(gl_vector3 const &rhs)
{
    data_[0] -= rhs.data_[0];
    data_[1] -= rhs.data_[1];
    data_[2] -= rhs.data_[2];

    return *this;
}

gl_vector3 &gl_vector3::operator-=(GLfloat const &rhs)
{
    data_[0] -= rhs;
    data_[1] -= rhs;
    data_[2] -= rhs;

    return *this;
}

gl_vector3 &gl_vector3::operator*=(gl_vector3 const &rhs)
{
    data_[0] *= rhs.data_[0];
    data_[1] *= rhs.data_[1];
    data_[2] *= rhs.data_[2];

    return *this;
}

gl_vector3 &gl_vector3::operator*=(GLfloat const &rhs)
{
    data_[0] *= rhs;
    data_[1] *= rhs;
    data_[2] *= rhs;

    return *this;
}

gl_vector3 &gl_vector3::operator/=(gl_vector3 const &rhs)
{
    data_[0] /= rhs.data_[0];
    data_[1] /= rhs.data_[1];
    data_[2] /= rhs.data_[2];
}

gl_vector3 &gl_vector3::operator/=(GLfloat const &rhs)
{
    data_[0] /= rhs;
    data_[1] /= rhs;
    data_[2] /= rhs;
}

GLfloat gl_vector3::dot_product(gl_vector3 const &rhs) const
{
    return (GLfloat)((data_[0] * rhs.data_[0])
                     + (data_[1] * rhs.data_[1])
                     + (data_[2] * rhs.data_[2]));
}

gl_vector3 gl_vector3::cross_product(gl_vector3 const &rhs) const
{
    return gl_vector3((data_[1] * rhs.data_[2]) - (rhs.data_[1] * data_[2]),
                      (data_[2] * rhs.data_[0]) - (rhs.data_[2] * data_[0]),
                      (data_[0] * rhs.data_[1]) - (rhs.data_[0] * data_[1]));
}

gl_vector3 gl_vector::interpolate(gl_vector3 const &rhs, GLfloat scalar) const
{
    return gl_vector3(math_interpolate(data_[0], rhs.data_[0], scalar),
                      math_interpolate(data_[1], rhs.data_[1], scalar),
                      math_interpolate(data_[2], rhs.data_[2], scalar));
}


bool gl_vector3::operator==(GLvector const &rhs)
{
    return ((data_[0] == rhs.data_[0])
            && (data_[1] == rhs.data_[1])
            && (data_[2] == rhs.data_[2]))
}

void gl_vector3::set_data(GLfloat x, GLfloat y, GLfloat z)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
}

void gl_vector3::set_x(GLfloat x)
{
    data_[0] = x;
}

void gl_vector3::set_y(GLfloat y)
{
    data_[1] = y;
}

void gl_vector3::set_z(GLfloat z)
{
    data_[2] = z;
}

GLfloat *gl_vector3::get_data()
{
    return data_;
}

GLfloat gl_vector3::get_x() const
{
    return data_[0];
}

GLfloat gl_vector3::get_y() const
{
    return data_[1];
}

GLfloat gl_vector3::get_z() const
{
    return data_[2];
}
