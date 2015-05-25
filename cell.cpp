/*
 * cell.hpp
 *
 */

#include "cell.hpp"

#include <cstring>

cell::cell()
    : shadow_(false)
    , distance_(0.0f)
{
    memset(layer_, 0, sizeof(GLushort) * (terrainspace::LAYER_COUNT - 1));
}

cell::~cell()
{
}

void cell::set_shadow(GLboolean shadow)
{
    shadow_ = shadow;
}

void cell::set_distance(GLfloat distance)
{
    distance_ = distance;
}

void cell::set_layer(GLushort value, GLint index)
{
    layer_[index] = value;
}

void cell::set_position(gl_vector3 const &position)
{
    position_ = position;
}

void cell::set_normal(gl_vector3 const &normal)
{
    normal_ = normal;
}

void cell::set_light(gl_rgba const &light)
{
    light_ = light;
}

GLboolean cell::get_shadow() const
{
    return shadow_;
}

GLfloat cell::get_distance() const
{
    return distance_;
}

GLushort cell::get_layer(GLint index) const
{
    return layer_[index];
}

gl_vector3 cell::get_position() const
{
    return position_;
}

gl_vector3 cell::get_normal() const
{
    return normal_;
}

gl_rgba cell::get_light() const
{
    return light_;
}
