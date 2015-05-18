/*
 * gl-vector2.hpp
 * 2006 Shamus Young
 *
 * Functions for dealing with 2D (usually texture mapping) values.
 */

#include "gl-vector2.hpp"

GLvector2 glVector(float x, float y)
{
    GLvector2 val;

    val.x = x;
    val.y = y;

    return val;
}

GLvector2 glVectorAdd(GLvector2 val1, GLvector2 val2)
{
    GLvector2 result;

    result.x = val1.x + val2.x;
    result.y = val1.y + val2.y;

    return result;
}

GLvector2 glVectorSubtract(GLvector2 val1, GLvector2 val2)
{
    GLvector2 result;

    result.x = val1.x - val2.x;
    result.y = val1.y = val2.y;

    return result;
}
