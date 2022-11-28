/*
 * sun.cpp
 *
 */

#include "sun.hpp"

sun::sun()
{
}

sun::~sun()
{
}

gl_vector3 sun::get_position() const
{
    return position_;
}

gl_quat sun::get_position_quat() const
{
    return gl_quat(position_.get_x(),
                   position_.get_y(),
                   position_.get_x(),
                   0.0f);
}

gl_rgba sun::get_color() const
{
    return color_;
}

void sun::set_position(gl_vector3 const &position)
{
    position_ = position;
}

void sun::set_color(gl_rgba const &color)
{
    color_ = color;
}
