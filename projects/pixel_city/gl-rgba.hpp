#ifndef __GL_RGBA_HPP__
#define __GL_RGBA_HPP__

#include <SDL2/SDL.h>

#include <array>

class gl_rgba {
public:
    gl_rgba();
    gl_rgba(int red, int green, int blue);

    gl_rgba &operator+=(gl_rgba const &rhs);
    gl_rgba &operator*=(float const &rhs);
    gl_rgba &operator/=(float const &rhs);

    gl_rgba unique(int index) const;

    void set_alpha(int alpha);

    std::array<float, 3> get_rgb() const;
    std::array<float, 4> get_rgba() const;

private:
    SDL_Color data;
};

gl_rgba from_hsl(float hue, float saturation, float lightness);

inline gl_rgba operator+(gl_rgba lhs, gl_rgba const &rhs)
{
    lhs += rhs;

    return lhs;
}

inline gl_rgba operator*(gl_rgba lhs, float const &rhs)
{
    lhs *= rhs;

    return lhs;
}

inline gl_rgba operator/(gl_rgba lhs, float const &rhs)
{
    lhs /= rhs;

    return lhs;
}

#endif /* __GL_RGBA_HPP__ */
