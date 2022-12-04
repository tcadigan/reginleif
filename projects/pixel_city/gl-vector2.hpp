#ifndef GL_VECTOR2_HPP_
#define GL_VECTOR2_HPP_

#include <SDL2/SDL_opengl.h>

class gl_vector2 {
public:
    gl_vector2();
    gl_vector2(GLfloat x, GLfloat y);
    virtual ~gl_vector2();

    GLfloat length();
    void normalize();
    void reflect(gl_vector2 const &normal);

    gl_vector2 &operator+=(gl_vector2 const &rhs);
    gl_vector2 &operator-=(gl_vector2 const &rhs);
    gl_vector2 &operator*=(GLfloat const &rhs);
    gl_vector2 &operator/=(GLfloat const &rhs);

    GLfloat dot_product(gl_vector2 const &rhs) const;
    gl_vector2 interpolate(gl_vector2 const &rhs, GLfloat scalar) const;

    void set_data(GLfloat x, GLfloat y);
    void set_x(GLfloat x);
    void set_y(GLfloat y);

    GLfloat *get_data();
    GLfloat get_x() const;
    GLfloat get_y() const;

private:
    GLfloat data_[2];
};

inline gl_vector2 operator+(gl_vector2 lhs, gl_vector2 const &rhs)
{
    lhs += rhs;

    return lhs;
}

inline gl_vector2 operator-(gl_vector2 lhs, gl_vector2 const &rhs)
{
    lhs -= rhs;

    return lhs;
}

inline gl_vector2 operator*(gl_vector2 lhs, GLfloat const &rhs)
{
    lhs *= rhs;

    return lhs;
}

inline gl_vector2 operator/(gl_vector2 lhs, GLfloat const &rhs)
{
    lhs /= rhs;

    return lhs;
}

inline GLfloat dot_product(gl_vector2 const &lhs, gl_vector2 const &rhs)
{
    return lhs.dot_product(rhs);
}

inline bool operator==(gl_vector2 const &lhs, gl_vector2 const &rhs)
{
    return ((lhs.get_x() == rhs.get_x())
            && (lhs.get_y() == rhs.get_y()));
}

inline bool operator!=(gl_vector2 const &lhs, gl_vector2 const &rhs)
{
    return !operator==(lhs, rhs);
}

#endif
