/*
 * gl-rgba.cpp
 * 2006 Shamus Young
 *
 * Functions for dealing with RGBA color values.
 */

#include "gl-rgba.hpp"

#include "math.hpp"

gl_rgba gl_rgba_interpolate(gl_rgba c1, gl_rgba c2, float delta)
{
    gl_rgba result(math_interpolate(c1.red_, c2.red_, delta),
                   math_interpolate(c1.green_, c2.green_, delta),
                   math_interpolate(c1.blue_, c2.blue_, delta),
                   math_interpolate(c1.alpha_, c2.alpha_, delta));

    return result;
}

gl_rgba gl_rgba_add(gl_rgba c1, gl_rgba c2)
{
    gl_rgba result(c1.red_ + c2.red_,
                   c1.green_ + c2.green_,
                   c1.blue_ + c2.blue_);

    return result;
}

gl_rgba gl_rgba_subtract(gl_rgba c1, gl_rgba c2)
{
    gl_rgba result(c1.red_ - c2.red_,
                   c1.green_ - c2.green_,
                   c1.blue_ - c2.blue_);

    return result;
}

gl_rgba gl_rgba_multiply(gl_rgba c1, gl_rgba c2)
{
    gl_rgba result(c1.red_ * c2.red_,
                   c1.green_ * c2.green_,
                   c1.blue_ * c2.blue_);

    return result;
}

gl_rgba gl_rgba_scale(gl_rgba c, float scale)
{
    c.red_ *= scale;
    c.green_ *= scale;
    c.blue_ *= scale;

    return c;
}

gl_rgba::gl_rgba()
    : red_(0.0f)
    , green_(0.0f)
    , blue_(0.0f)
    , alpha_(1.0f)
{
}

gl_rgba::gl_rgba(float red, float green, float blue)
    : red_(red)
    , green_(green)
    , blue_(blue)
    , alpha_(1.0f)
{
}

gl_rgba::gl_rgba(float red, float green, float blue, float alpha)
    : red_(red)
    , green_(green)
    , blue_(blue)
    , alpha_(alpha)
{
}

gl_rgba::gl_rgba(long c)
    : red_((float)((c & 0xFF000000) >> 24) / 255.0f)
    , green_((float)((c & 0x00FF0000) >> 16) / 255.0f)
    , blue_((float)((c & 0x0000FF00) >> 8) / 255.0f)
    , alpha_(1.0f)
{
}

gl_rgba::gl_rgba(float luminance)
  : red_(luminance)
  , green_(luminance)
  , blue_(luminance)
  , alpha_(1.0f)
{
}

gl_rgba::~gl_rgba()
{
}
