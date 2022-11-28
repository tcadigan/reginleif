#ifndef GL_VECTOR3_HPP_
#define GL_VECTOR3_HPP_

#include <SDL2/SDL_opengl.h>

class gl_vector3 {
public:
    gl_vector3();
    gl_vector3(GLfloat x, GLfloat y, GLfloat z);
    virtual ~gl_vector3();

    GLfloat length();
    void normalize();
    void reflect(gl_vector3 const &normal);

    gl_vector3 &operator+=(gl_vector3 const &rhs);
    gl_vector3 &operator-=(gl_vector3 const &rhs);
    gl_vector3 &operator*=(GLfloat const &rhs);

    GLfloat dot_product(gl_vector3 const &rhs) const;
    gl_vector3 cross_product(gl_vector3 const &rhs) const;
    gl_vector3 interpolate(gl_vector3 const &rhs, GLfloat scalar) const;

    void set_data(GLfloat x, GLfloat y, GLfloat z);
    void set_x(GLfloat x);
    void set_y(GLfloat y);
    void set_z(GLfloat z);

    GLfloat *get_data();
    GLfloat get_x() const;
    GLfloat get_y() const;
    GLfloat get_z() const;

private:
    GLfloat data_[3];
};
 
inline gl_vector3 operator+(gl_vector3 lhs, gl_vector3 const &rhs)
{
    lhs += rhs;
    
    return lhs;
}

inline gl_vector3 operator-(gl_vector3 lhs, gl_vector3 const &rhs)
{
    lhs -= rhs;

    return lhs;
}

inline gl_vector3 operator*(gl_vector3 lhs, GLfloat const &rhs)
{
    lhs *= rhs;

    return lhs;
}

inline gl_vector3 operator*(GLfloat const &lhs, gl_vector3 rhs)
{
    rhs *= lhs;

    return rhs;
}

inline GLfloat dot_product(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return lhs.dot_product(rhs);
}

inline gl_vector3 cross_product(gl_vector3 const &lhs, gl_vector3 const &rhs)
{
    return lhs.cross_product(rhs);
}

#endif
