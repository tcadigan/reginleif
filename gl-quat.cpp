/*
 * gl-quat.cpp
 * 2006 Shamus Young
 *
 * Functions for dealing with Quaternions
 */

#include "gl-quat.hpp"

gl_quat::gl_quat(float x, float y, float z, float w)
    : x_(x)
    , y_(y)
    , z_(z)
    , w_(w)
{
}

gl_quat::~gl_quat()
{
}
