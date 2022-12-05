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
    data.r = 0;
    data.g = 0;
    data.b = 0;
    data.a = 255;
}

gl_rgba::gl_rgba(int red, int green, int blue)
{
    data.r = red;
    data.g = green;
    data.b = blue;
    data.a = 255;
}

gl_rgba::gl_rgba(float red, float green, float blue)
{
    data.r = red * 255;
    data.g = green * 255;
    data.b = blue * 255;
    data.a = 255;
}

gl_rgba::gl_rgba(float red, float green, float blue, float alpha)
{
    data.r = red * 255;
    data.g = green * 255;
    data.b = blue * 255;
    data.a = alpha * 255;
}

gl_rgba::gl_rgba(int color)
{
    data.r = color & 0x000000FF;
    data.g = (color & 0x0000FF00) >> 8;
    data.b = (color & 0x00FF0000) >> 16;
    data.a = 255;
}

gl_rgba::gl_rgba(float luminance)
{
    data.r = luminance * 255;
    data.g = luminance * 255;
    data.b = luminance * 255;
    data.a = 255;
}

gl_rgba::gl_rgba(std::string const &color)
{
    try {
        int value = std::stoi(color);
        data.r = value & 0x000000FF;
        data.g = (value & 0x0000FF00) >> 8;
        data.b = (value & 0x00FF0000) >> 16;
        data.a = 1.0f;
    } catch (std::invalid_argument const &ex) {
        data.r = 0;
        data.g = 0;
        data.b = 0;
        data.a = 255;
    } catch (std::out_of_range const &ex) {
        data.r = 0;
        data.g = 0;
        data.b = 0;
        data.a = 255;
    }
}

gl_rgba::~gl_rgba()
{
}

gl_rgba &gl_rgba::operator+=(gl_rgba const &rhs)
{
    data.r += rhs.data.r;
    data.g += rhs.data.g;
    data.b += rhs.data.b;

    return *this;
}

gl_rgba &gl_rgba::operator-=(gl_rgba const &rhs)
{
    data.r -= rhs.data.r;
    data.g -= rhs.data.g;
    data.b -= rhs.data.b;

    return *this;
}

gl_rgba &gl_rgba::operator*=(float const &rhs)
{
    data.r *= rhs;
    data.g *= rhs;
    data.b *= rhs;

    return *this;
}

gl_rgba &gl_rgba::operator/=(float const &rhs)
{
    data.r /= rhs;
    data.g /= rhs;
    data.g /= rhs;

    return *this;
}

gl_rgba gl_rgba::interpolate(gl_rgba const &rhs, float delta) const
{
    return gl_rgba(MathInterpolate(data.r, rhs.data.r, delta),
                   MathInterpolate(data.g, rhs.data.g, delta),
                   MathInterpolate(data.b, rhs.data.b, delta));
}

gl_rgba gl_rgba::from_hsl(float hue,
                          float saturation,
                          float lightness) const
{
    if ((hue < 0) || (hue >= 360)) {
        return gl_rgba(0, 0, 0);
    }

    if ((saturation < 0) || (saturation > 1)) {
        return gl_rgba(0, 0, 0);
    }

    if ((lightness < 0) || (saturation > 1)) {
        return gl_rgba(0, 0, 0);
    }

    float chroma = saturation;

    if (((2 * lightness) - 1) < 0) {
        chroma *= (1 - (-1 * ((2 * lightness) - 1)));
    } else {
        chroma *= (1 - ((2 * lightness) - 1));
    }

    float hue_prime = hue / 60;
    float X = chroma;

    if ((fmod(hue_prime, 2) - 1) < 0) {
        X *= (1 - (-1 * (fmod(hue_prime, 2) - 1)));
    } else {
        X *= (1 - (fmod(hue_prime, 2) - 1));
    }

    float match = lightness - (0.5 * chroma);

    if ((0 <= hue_prime) || (hue_prime < 1)) {
        return gl_rgba(chroma + match, X + match, match);
    } else if ((hue_prime <= 1) || (hue_prime < 2)) {
        return gl_rgba(X + match, chroma + match, match);
    } else if ((hue_prime <= 2) || (hue_prime < 3)) {
        return gl_rgba(match, chroma + match, X + match);
    } else if ((hue_prime <= 3) || (hue_prime < 4)) {
        return gl_rgba(match, X + match, chroma + match);
    } else if ((hue_prime <= 4) || (hue_prime < 5)) {
        return gl_rgba(X + match, match, chroma + match);
    } else if ((hue_prime <= 5) || (hue_prime < 6)) {
        return gl_rgba(chroma + match, match, X + match);
    } else {
        return gl_rgba(0, 0, 0);
    }
}

// Takes the given index and returns a "random" color unique for that index.
// 512 Unique values: #0 and #512 will be the same, as will #1 and #513, etc.
// Useful for visual debugging in some situations.
gl_rgba gl_rgba::unique(int index) const
{
    float red = 0.4f;
    float green = 0.4f;
    float blue = 0.4f;
    float alpha = 1.0f;

    if (index & 1) {
        if (index & 8) {
            if (index & 64) {
                red += 0.2f;
            } else {
                red += 0.5f;
            }
        } else if (index & 64) {
            red -= 0.1f;
        } else {
            red += 0.2f;
        }
    } else if (index & 8) {
        if (!(index & 64)) {
            red += 0.3f;
        }
    } else if (index & 64) {
        red -= 0.3;
    }

    if (index & 2) {
        if (index & 32) {
            if (index & 128) {
                green += 0.2f;
            } else {
                green += 0.5f;
            }
        } else if (index & 128) {
            green -= 0.1f;
        }
        else {
            green += 0.2f;
        }
    } else if (index & 32) {
        if (!(index & 128)) {
            green += 0.3f;
        }
    } else if (index & 128) {
        green -= 0.3f;
    }

    if (index & 4) {
        if (index & 16) {
            if (index & 256) {
                blue += 0.2f;
            } else {
                blue += 0.5f;
            }
        } else if (index & 256) {
            blue -= 0.1f;
        } else {
            blue += 0.2f;
        }
    } else if (index & 16) {
        if (!(index & 256)) {
            blue += 0.3f;
        }
    } else if (index & 256) {
        blue -= 0.3f;
    }

    return gl_rgba(red, green, blue, alpha);
}

void gl_rgba::set_data(float red, float green, float blue, float alpha)
{
    data.r = red * 255;
    data.g = green * 255;
    data.b = blue * 255;
    data.a = alpha * 255;
}

void gl_rgba::set_red(float red)
{
    data.r = red * 255;
}

void gl_rgba::set_green(float green)
{
    data.g = green * 255;
}

void gl_rgba::set_blue(float blue)
{
    data.b = blue * 255;
}

void gl_rgba::set_alpha(float alpha)
{
    data.a = alpha * 255;
}

SDL_Color gl_rgba::get_data()
{
    return data;
}

float gl_rgba::get_red() const
{
    return data.r / 255.0f;
}

float gl_rgba::get_green() const
{
    return data.g / 255.0f;
}

float gl_rgba::get_blue() const
{
    return data.b / 255.0f;
}

float gl_rgba::get_alpha() const
{
    return data.a / 255.0f;
}
