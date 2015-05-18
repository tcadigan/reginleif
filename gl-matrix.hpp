#ifndef GL_MATRIX_HPP_
#define GL_MATRIX_HPP_

#include "gl-vector3.hpp"

class GLmatrix {
public:
    float elements[4][4];
};

GLmatrix glMatrixIdentity(void);
void glMatrixElementsSet(GLmatrix *m, float *in);
GLmatrix glMatrixMultiply(GLmatrix a, GLmatrix b);
GLvector3 glMatrixTransformPoint(GLmatrix m, GLvector3 in);
GLmatrix glMatrixRotate(GLmatrix m, float theta, float x, float y, float z);

#endif
