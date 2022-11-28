/*
 * sky-point.cpp
 *
 */

#include "sky-point.hpp"

sky_point::sky_point()
{
}

sky_point::~sky_point()
{
}

void sky_point::set_color(gl_rgba const &color)
{
    color_ = color;
}

void sky_point::set_position(gl_vector3 const &position)
{
    position_ = position;
}

gl_rgba sky_point::get_color() const
{
    return color_;
}

gl_vector3 sky_point::get_position() const
{
    return position_;
}
