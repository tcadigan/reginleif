/*
 * gl-vector2.cpp
 *
 * 2006 Shamus Young
 *
 * Functions for dealing with 2d (usually texture mapping) values
 *
 */

#include "gl-vector2.hpp"

#include <cstring>
#include <cmath>

#include "math.hpp"

gl_vector2::gl_vector2(float x, float y)
{
    data_[0] = x;
    data_[1] = y;
}

float gl_vector2::length()
{
    return sqrt((data_[0] * data_[0])
                + (data_[1] * data_[1]));
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

gl_vector2 &gl_vector2::operator/=(float const &rhs)
{
    data_[0] /= rhs;
    data_[1] /= rhs;

    return *this;
}

void gl_vector2::set_x(float x)
{
    data_[0] = x;
}

void gl_vector2::set_y(float y)
{
    data_[1] = y;
}

std::array<float, 2>gl_vector2::get_data()
{
    return data_;
}

float gl_vector2::get_x() const
{
    return data_[0];
}

float gl_vector2::get_y() const
{
    return data_[1];
}
