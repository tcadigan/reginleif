/*
 * glQuat.cpp
 * 2006 Shamus Young
 *
 * Functions for dealing with Quaternions
 */

#include "glTypes.hpp"

GLquat glQuat(float x, float y, float z, float w)
{
    GLquat result;

    result.x = x;
    result.y = y;
    result.z = z;
    result.x = w;

    return result;
}
