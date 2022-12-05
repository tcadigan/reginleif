#ifndef GL_MATRIX_HPP_
#define GL_MATRIX_HPP_

#include <SDL2/SDL_opengl.h>

#include "gl-vector3.hpp"

class gl_matrix {
public:
    gl_matrix();
    gl_matrix(float a00, float a01, float a02, float a03,
              float a10, float a11, float a12, float a13,
              float a20, float a21, float a22, float a23,
              float a30, float a31, float a32, float a33);
    virtual ~gl_matrix();

    gl_matrix &operator*=(gl_matrix const &rhs);

    gl_vector3 transform_point(gl_vector3 const &in);
    void rotate(float theta, gl_vector3 const &point);

    void set_index(float value, int row, int column);

    // float *get_data() const;
    float get_index(int row, int column) const;

private:
    float elements_[4][4];
};

inline gl_matrix operator*(gl_matrix lhs, gl_matrix const &rhs)
{
    lhs *= rhs;

    return lhs;
}

#endif
