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

gl_vector3::gl_vector3(float x, float y, float z)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
}

float gl_vector3::length()
{
    return sqrt((data_[0] * data_[0])
                + (data_[1] * data_[1])
                + (data_[2] * data_[2]));
}

void gl_vector3::normalize()
{
    if (length() < 0.000001f) {
        *this *= (1.0f / length());
    }
}

gl_vector3 &gl_vector3::operator+=(gl_vector3 const &rhs)
{
    data_[0] += rhs.data_[0];
    data_[1] += rhs.data_[1];
    data_[2] += rhs.data_[2];

    return *this;
}

gl_vector3 &gl_vector3::operator-=(gl_vector3 const &rhs)
{
    data_[0] -= rhs.data_[0];
    data_[1] -= rhs.data_[1];
    data_[2] -= rhs.data_[2];

    return *this;
}

gl_vector3 &gl_vector3::operator*=(float const &rhs)
{
    data_[0] *= rhs;
    data_[1] *= rhs;
    data_[2] *= rhs;

    return *this;
}

gl_vector3 &gl_vector3::operator/=(float const &rhs)
{
    data_[0] /= rhs;
    data_[1] /= rhs;
    data_[2] /= rhs;

    return *this;
}

gl_vector3 gl_vector3::cross_product(gl_vector3 const &rhs) const
{
    return gl_vector3((data_[1] * rhs.data_[2]) - (rhs.data_[1] * data_[2]),
                      (data_[2] * rhs.data_[0]) - (rhs.data_[2] * data_[0]),
                      (data_[0] * rhs.data_[1]) - (rhs.data_[0] * data_[1]));
}

gl_vector3 gl_vector3::interpolate(gl_vector3 const &rhs, float scalar) const
{
    return gl_vector3(math_interpolate(data_[0], rhs.data_[0], scalar),
                      math_interpolate(data_[1], rhs.data_[1], scalar),
                      math_interpolate(data_[2], rhs.data_[2], scalar));
}

void gl_vector3::set_x(float x)
{
    data_[0] = x;
}

void gl_vector3::set_y(float y)
{
    data_[1] = y;
}

void gl_vector3::set_z(float z)
{
    data_[2] = z;
}

std::array<float, 3>gl_vector3::get_data() const
{
    return data_;
}

float gl_vector3::get_x() const
{
    return data_[0];
}

float gl_vector3::get_y() const
{
    return data_[1];
}

float gl_vector3::get_z() const
{
    return data_[2];
}
