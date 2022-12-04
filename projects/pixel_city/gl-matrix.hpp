#ifndef GL_MATRIX_HPP_
#define GL_MATRIX_HPP_

#include <SDL2/SDL_opengl.h>

#include "gl-vector3.hpp"

class gl_matrix {
public:
    gl_matrix();
    gl_matrix(GLfloat a00, GLfloat a01, GLfloat a02, GLfloat a03,
              GLfloat a10, GLfloat a11, GLfloat a12, GLfloat a13,
              GLfloat a20, GLfloat a21, GLfloat a22, GLfloat a23,
              GLfloat a30, GLfloat a31, GLfloat a32, GLfloat a33);
    virtual ~gl_matrix();

    gl_matrix &operator*=(gl_matrix const &rhs);

    gl_vector3 transform_point(gl_vector3 const &in);
    void rotate(GLfloat theta, gl_vector3 const &point);

    void set_index(GLfloat value, GLint row, GLint column);

    // GLfloat *get_data() const;
    GLfloat get_index(GLint row, GLint column) const;

private:
    GLfloat elements_[4][4];
};

inline gl_matrix operator*(gl_matrix lhs, gl_matrix const &rhs)
{
    lhs *= rhs;

    return lhs;
}

#endif
