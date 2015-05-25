/*
 * point.cpp
 *
 */

#include "point.hpp"

point::point()
{
}

point::~point()
{
}

GLint point::get_x() const
{
    return x_;
}

GLint point::get_y() const
{
    return y_;
}

void point::set(GLint x, GLint y)
{
    x_ = x;
    y_ = y;
}

void point::set_x(GLint x)
{
    x_ = x;
}

void point::set_y(GLint y)
{
    y_ = y;
}
