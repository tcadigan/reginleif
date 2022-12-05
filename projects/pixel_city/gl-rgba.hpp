#ifndef GL_RGBA_HPP_
#define GL_RGBA_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <string>

class gl_rgba {
public:
    gl_rgba();
    gl_rgba(int red, int green, int blue);
    gl_rgba(float red, float green, float blue);
    gl_rgba(float red, float green, float blue, float alpha);
    gl_rgba(int color);
    gl_rgba(float luminance);
    gl_rgba(std::string const &color);
    virtual ~gl_rgba();

    gl_rgba &operator+=(gl_rgba const &rhs);
    gl_rgba &operator-=(gl_rgba const &rhs);
    gl_rgba &operator*=(float const &rhs);
    gl_rgba &operator/=(float const &rhs);

    gl_rgba interpolate(gl_rgba const &rhs, float delta) const;
    gl_rgba from_hsl(float hue, float saturation, float lightness) const;
    gl_rgba unique(int index) const;

    void set_data(float red, float green, float blue, float alpha);
    void set_red(float red);
    void set_green(float green);
    void set_blue(float blue);
    void set_alpha(float alpha);

    SDL_Color get_data();
    float get_red() const;
    float get_green() const;
    float get_blue() const;
    float get_alpha() const;

private:
    SDL_Color data;
};

inline gl_rgba operator+(gl_rgba lhs, gl_rgba const &rhs)
{
    lhs += rhs;

    return lhs;
}

inline gl_rgba operator-(gl_rgba lhs, gl_rgba const &rhs)
{
    lhs -= rhs;

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

inline bool operator==(gl_rgba const &lhs, gl_rgba const &rhs)
{
    return ((lhs.get_red() == rhs.get_red())
            && (lhs.get_green() == rhs.get_green())
            && (lhs.get_blue() == rhs.get_blue()));
}

inline bool operator!=(gl_rgba const &lhs, gl_rgba const &rhs)
{
    return !operator==(lhs, rhs);
}

#endif
