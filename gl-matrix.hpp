#ifndef GL_MATRIX_HPP_
#define GL_MATRIX_HPP_

#include "gl-vector-3d.hpp"

class gl_matrix {
public:
    gl_matrix();
    virtual ~gl_matrix();

    float elements_[4][4];
};

gl_matrix gl_matrix_identity(void);
void gl_matrix_elements_set(gl_matrix *m, float *in);
gl_matrix gl_matrix_multiply(gl_matrix a, gl_matrix b);
gl_vector_3d gl_matrix_transform_point(gl_matrix m, gl_vector_3d in);
gl_matrix gl_matrix_rotate(gl_matrix m, float theta, float x, float y, float z);

#endif
