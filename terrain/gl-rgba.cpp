/*
 * gl-rgba.cpp
 * 2006 Shamus Young
 *
 * Functions for dealing with RGBA color values.
 */

#include "gl-rgba.hpp"

#include "math.hpp"

gl_rgba::gl_rgba()
{
    data_[0] = 0;
    data_[1] = 0;
    data_[2] = 0;
    data_[3] = 1.0f;
}

gl_rgba::gl_rgba(GLfloat red, GLfloat green, GLfloat blue)
{
    data_[0] = red;
    data_[1] = green;
    data_[2] = blue;
    data_[3] = 1.0f;
}

gl_rgba::gl_rgba(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    data_[0] = red;
    data_[1] = green;
    data_[2] = blue;
    data_[3] = alpha;
}

gl_rgba::gl_rgba(GLint c)
{
    data_[0] = (GLfloat)((c & 0xFF000000) >> 24) / 255.0f;
    data_[1] = (GLfloat)((c & 0x00FF0000) >> 16) / 255.0f;
    data_[2] = (GLfloat)((c & 0x0000FF00) >> 8) / 255.0f;
    data_[3] = 1.0f;
}

gl_rgba::gl_rgba(GLfloat luminance)
{
    data_[0] = luminance;
    data_[1] = luminance;
    data_[2] = luminance;
    data_[3] = 1.0f;
}

gl_rgba::~gl_rgba()
{
}

gl_rgba &gl_rgba::operator+=(gl_rgba const &rhs)
{
    data_[0] += rhs.data_[0];
    data_[1] += rhs.data_[1];
    data_[2] += rhs.data_[2];

    return *this;
}

gl_rgba &gl_rgba::operator-=(gl_rgba const &rhs)
{
    data_[0] -= rhs.data_[0];
    data_[1] -= rhs.data_[1];
    data_[2] -= rhs.data_[2];

    return *this;
}

gl_rgba &gl_rgba::operator*=(gl_rgba const &rhs)
{
    data_[0] *= rhs.data_[0];
    data_[1] *= rhs.data_[1];
    data_[2] *= rhs.data_[2];

    return *this;
}

gl_rgba &gl_rgba::operator*=(GLfloat const &rhs)
{
    data_[0] *= rhs;
    data_[1] *= rhs;
    data_[2] *= rhs;

    return *this;
}

gl_rgba gl_rgba::interpolate(gl_rgba const &rhs, GLfloat delta) const
{
    return gl_rgba(math_interpolate(data_[0], rhs.data_[0], delta),
                   math_interpolate(data_[1], rhs.data_[1], delta),
                   math_interpolate(data_[2], rhs.data_[2], delta),
                   math_interpolate(data_[3], rhs.data_[3], delta));
}

void gl_rgba::set_data(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    data_[0] = red;
    data_[1] = green;
    data_[2] = blue;
    data_[3] = alpha;
}

void gl_rgba::set_red(GLfloat red)
{
    data_[0] = red;
}

void gl_rgba::set_green(GLfloat green)
{
    data_[1] = green;
}

void gl_rgba::set_blue(GLfloat blue)
{
    data_[2] = blue;
}

void gl_rgba::set_alpha(GLfloat alpha)
{
    data_[3] = alpha;
}

GLfloat *gl_rgba::get_data()
{
    return data_;
}

GLfloat gl_rgba::get_red() const
{
    return data_[0];
}

GLfloat gl_rgba::get_green() const
{
    return data_[1];
}

GLfloat gl_rgba::get_blue() const
{
    return data_[2];
}

GLfloat gl_rgba::get_alpha() const
{
    return data_[3];
}
