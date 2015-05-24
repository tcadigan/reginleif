/*
 * gl-vector-2d.hpp
 * 2006 Shamus Young
 *
 * Functions for dealing with 2D (usually texture mapping) values.
 */

#include "gl-vector-2d.hpp"

gl_vector_2d::gl_vector_2d()
{
}

gl_vector_2d::gl_vector_2d(float x, float y)
    : x_(x)
    , y_(y)
{
}

gl_vector_2d::~gl_vector_2d()
{
}

gl_vector_2d glVectorAdd(gl_vector_2d val1, gl_vector_2d val2)
{
    gl_vector_2d result(val1.x_ + val2.x_,
                        val1.y_ + val2.y_);

    return result;
}

gl_vector_2d glVectorSubtract(gl_vector_2d val1, gl_vector_2d val2)
{
    gl_vector_2d result(val1.x_ - val2.x_,
                        val1.y_ - val2.y_);

    return result;
}
