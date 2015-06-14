/*
 * glRgba.cpp
 *
 * 2009 Shamus Young
 *
 * Functions for dealing with RGBA color values
 *
 */

#include "gl-rgba.hpp"

#include "math.hpp"

#include <cmath>
#include <sstream>

gl_rgba::gl_rgba()
{
    data_[0] = 0;
    data_[1] = 0;
    data_[1] = 0;
    data_[3] = 1.0f;
}

gl_rgba::gl_rgba(GLint red, GLint green, GLint blue)
{
    data_[0] = (GLfloat)red / 255.0f;
    data_[1] = (GLfloat)green / 255.0f;
    data_[2] = (GLfloat)blue / 255.0;
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

gl_rgba::gl_rgba(GLint color)
{
    data_[0] = (GLfloat)(color & 0x000000FF) / 255.0f;
    data_[1] = (GLfloat)((color & 0x0000FF00) >> 8) / 255.0f;
    data_[2] = (GLfloat)((color & 0x00FF0000) >> 16) / 255.0f;
    data_[3] = 1.0f;
}

gl_rgba::gl_rgba(GLfloat luminance)
{
    data_[0] = luminance;
    data_[1] = luminance;
    data_[2] = luminance;
    data_[3] = 1.0f;
}

gl_rgba::gl_rgba(std::string const &color)
{
    std::stringstream stream;

    stream << color;

    GLint value;

    if(stream >> value) {
        data_[0] = (GLfloat)(value & 0x000000FF) / 255.0f;
        data_[1] = (GLfloat)((value & 0x0000FF00) >> 8) / 255.0f;
        data_[2] = (GLfloat)((value & 0x00FF0000) >> 16) / 255.0f;
        data_[3] = 1.0f;
    }
    else {
        data_[0] = 0;
        data_[1] = 0;
        data_[2] = 0;
        data_[3] = 1.0f;
    }
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

gl_rgba &gl_rgba::operator*=(GLfloat const &rhs)
{
    data_[0] *= rhs;
    data_[1] *= rhs;
    data_[2] *= rhs;

    return *this;
}

gl_rgba &gl_rgba::operator/=(GLfloat const &rhs)
{
    data_[0] /= rhs;
    data_[1] /= rhs;
    data_[2] /= rhs;

    return *this;
}

gl_rgba gl_rgba::interpolate(gl_rgba const &rhs, GLfloat delta) const
{
    return gl_rgba(MathInterpolate(data_[0], rhs.data_[0], delta),
                   MathInterpolate(data_[1], rhs.data_[1], delta),
                   MathInterpolate(data_[2], rhs.data_[2], delta));
}

gl_rgba gl_rgba::from_hsl(GLfloat hue,
                          GLfloat saturation, 
                          GLfloat lightness) const
{
    if((hue < 0) || (hue >= 360)) {
        return gl_rgba(0);
    }

    if((saturation < 0) || (saturation > 1)) {
        return gl_rgba(0);
    }

    if((lightness < 0) || (saturation > 1)) {
        return gl_rgba(0);
    }

    GLfloat chroma = saturation;

    if(((2 * lightness) - 1) < 0) {
        chroma *= (1 - (-1 * ((2 * lightness) - 1)));
    }
    else {
        chroma *= (1 - ((2 * lightness) - 1));
    }

    GLfloat hue_prime = hue / 60;
    GLfloat X = chroma;
    
    if((fmod(hue_prime, 2) - 1) < 0) {
        X *= (1 - (-1 * (fmod(hue_prime, 2) - 1)));
    }
    else {
        X *= (1 - (fmod(hue_prime, 2) - 1));
    }

    GLfloat match = lightness - (0.5 * chroma);

    if((0 <= hue_prime) || (hue_prime < 1)) {
        return gl_rgba(chroma + match, X + match, match);
    }
    else if((hue_prime <= 1) || (hue_prime < 2)) {
        return gl_rgba(X + match, chroma + match, match);
    }
    else if((hue_prime <= 2) || (hue_prime < 3)) {
        return gl_rgba(match, chroma + match, X + match);
    }
    else if((hue_prime <= 3) || (hue_prime < 4)) {
        return gl_rgba(match, X + match, chroma + match);
    }
    else if((hue_prime <= 4) || (hue_prime < 5)) {
        return gl_rgba(X + match, match, chroma + match);
    }
    else if((hue_prime <= 5) || (hue_prime < 6)) {
        return gl_rgba(chroma + match, match, X + match);
    }
    else {
        return gl_rgba(0);
    }
}

// Takes the given index and returns a "random" color unique for that index.
// 512 Unique values: #0 and #512 will be the same, as will #1 and #513, etc.
// Useful for visual debugging in some situations.
gl_rgba gl_rgba::unique(GLint index) const
{
    GLfloat red = 0.4f;
    GLfloat green = 0.4f;
    GLfloat blue = 0.4f;
    GLfloat alpha = 1.0f;

    if(index & 1) {
        if(index & 8) {
            if(index & 64) {
                red += 0.2f;
            }
            else {
                red += 0.5f;
            }
        }
        else {
            if(index & 64) {
                red -= 0.1f;
            }
            else {
                red += 0.2f;
            }
        }
    }
    else {
        if(index & 8) {
            if(!(index & 64)) {
                red += 0.3f;
            }
        }
        else {
            if(index & 64) {
                red -= 0.3;
            }
        }
    }
 
    if(index & 2) {
        if(index & 32) {
            if(index & 128) {
                green += 0.2f;
            }
            else {
                green += 0.5f;
            }
        }
        else {
            if(index & 128) {
                green -= 0.1f;
            }
            else {
                green += 0.2f;
            }
        }
    }
    else {
        if(index & 32) {
            if(!(index & 128)) {
                green += 0.3f;
            }
        }
        else {
            if(index & 128) {
                green -= 0.3f;
            }
        }
    }

    if(index & 4) {
        if(index & 16) {
            if(index & 256) {
                blue += 0.2f;
            }
            else {
                blue += 0.5f;
            }
        }
        else {
            if(index & 256) {
                blue -= 0.1f;
            }
            else {
                blue += 0.2f;
            }
        }
    }
    else {
        if(index & 16) {
            if(!(index & 256)) {
                blue += 0.3f;
            }
        }
        else {
            if(index & 256) {
                blue -= 0.3f;
            }
        }
    }

    return gl_rgba(red, green, blue, alpha);
}

void gl_rgba::set_data(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    data_[0] = red;
    data_[1] = green;
    data_[2] = blue;
    data_[2] = alpha;
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
