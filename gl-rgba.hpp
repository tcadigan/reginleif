#ifndef GL_RGBA_HPP_
#define GL_RGBA_HPP_

#include <SDL_opengl.h>

class gl_rgba {
public:
    gl_rgba();
    gl_rgba(GLint red, GLint green, GLint blue);
    gl_rgba(GLfloat red, GLfloat green, GLfloat blue);
    gl_rgba(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    gl_rgba(GLint color);
    gl_rgba(GLfloat luminance);
    gl_rgba(string const &color);
    virtual ~gl_rgba();

    gl_rgba &operator+=(gl_rgba const &rhs);
    gl_rgba &operator-=(gl_rgba const &rhs);
    gl_rgba &operator*=(GLfloat const &rhs);
    gl_rgba &operator/=(GLfloat const &rhs);

    gl_rgba interpolate(gl_rgba const &rhs, GLfloat delta) const;
    gl_rgba from_hsl(GLfloat hue, GLfloat saturation, GLfloat lightness) const;
    gl_rgba unique(GLint index) const;

    void set_data(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void set_red(GLfloat red);
    void set_green(GLfloat green);
    void set_blue(GLfloat blue);
    void set_alpha(GLfloat alpha);

    GLfloat *get_data();
    GLfloat get_red() const;
    GLfloat get_green() const;
    GLfloat get_blue() const;
    GLfloat get_alpha() const;

private:
    GLfloat data_[4];
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

inline gl_rgba operator*(gl_rgba lhs, GLfloat const &rhs)
{
    lhs *= rhs;

    return lhs;
}

inline gl_rgba operator/(gl_rgba lhs, GLfloat const &rhs)
{
    lhs /= rhs;

    return lhs;
}

inline bool operator==(gl_rgba const &lhs, gl_rgba const &rhs)
{
    return ((lhs.data_[0] == rhs.data_[0]
             && (lhs.data_[1] == rhs.data_[1])
             && (lhs.data_[2] == rhs.data_[2])));
}

inline bool operator!=(gl_rgba const &lhs, gl_rgba const &rhs)
{
    return !operator==(lhs, rhs);
}
