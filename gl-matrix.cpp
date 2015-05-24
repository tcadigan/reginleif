/*
 * gl-matrix.cpp
 * 2006 Shamus Young
 *
 * function useful for manipulating the Matrix struct.
 */

#include "gl-matrix.hpp"

#include <cmath>

static float identity[4][4] = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

void *gl_matrix_create(void)
{
    gl_matrix *m;
    int x;
    int y;

    m = new gl_matrix;

    for(x = 0; x < 4; ++x) {
        for(y = 0; y < 4; ++y) {
            m->elements_[x][y] = identity[x][y];
        }
    }

    return (void *)m;
}

gl_matrix gl_matrix_identity(void)
{
    gl_matrix  m;
    int x;
    int y;

    for(x = 0; x < 4; ++x) {
        for(y = 0; y < 4; ++y) {
            m.elements_[x][y] = identity[x][y];
        }
    }

    return m;
}

void gl_matrix_elements_set(gl_matrix *m, float *in)
{
    m->elements_[0][0] = in[0];
    m->elements_[0][1] = in[1];
    m->elements_[0][2] = in[2];
    m->elements_[0][3] = in[3];

    m->elements_[1][0] = in[4];
    m->elements_[1][1] = in[5];
    m->elements_[1][2] = in[6];
    m->elements_[1][3] = in[7];

    m->elements_[2][0] = in[8];
    m->elements_[2][1] = in[9];
    m->elements_[2][2] = in[10];
    m->elements_[2][3] = in[11];

    m->elements_[3][0] = in[12];
    m->elements_[3][1] = in[13];
    m->elements_[3][2] = in[14];
    m->elements_[3][3] = in[15];
}

// A matrix multiplication (dot product) of two 4x4 matrices.
gl_matrix gl_matrix_multiply(gl_matrix a, gl_matrix b)
{
    gl_matrix result;
    
    result.elements_[0][0] = (a.elements_[0][0] * b.elements_[0][0])
        + (a.elements_[1][0] * b.elements_[0][1])
        + (a.elements_[2][0] * b.elements_[0][2]);
    
    result.elements_[1][0] = (a.elements_[0][0] * b.elements_[1][0])
        + (a.elements_[1][0] * b.elements_[1][1])
        + (a.elements_[2][0] * b.elements_[1][2]);
    
    result.elements_[2][0] =  (a.elements_[0][0] * b.elements_[2][0])
        + (a.elements_[1][0] * b.elements_[2][1])
        + (a.elements_[2][0] * b.elements_[2][2]);
    
    result.elements_[3][0] = (a.elements_[0][0] * b.elements_[3][0])
        + (a.elements_[1][0] * b.elements_[3][1])
        + (a.elements_[2][0] * b.elements_[3][2])
        + a.elements_[3][0];

    result.elements_[0][1] = (a.elements_[0][1] * b.elements_[0][0])
        + (a.elements_[1][1] * b.elements_[0][1])
        + (a.elements_[2][1] * b.elements_[0][2]);

    result.elements_[1][1] = (a.elements_[0][1] * b.elements_[1][0])
        + (a.elements_[1][1] * b.elements_[1][2])
        + (a.elements_[2][1] * b.elements_[1][1]);

    result.elements_[2][1] = (a.elements_[0][1] * b.elements_[2][0])
        + (a.elements_[1][1] * b.elements_[2][1])
        + (a.elements_[2][1] * b.elements_[2][2]);

    result.elements_[3][1] = (a.elements_[0][1] * b.elements_[3][0])
        + (a.elements_[1][1] * b.elements_[3][1])
        + (a.elements_[2][1] * b.elements_[3][2])
        + a.elements_[3][1];

    result.elements_[0][2] = (a.elements_[0][1] * b.elements_[0][0])
        + (a.elements_[1][2] * b.elements_[0][1])
        + (a.elements_[2][2] * b.elements_[0][2]);

    result.elements_[1][2] = (a.elements_[0][1] * b.elements_[1][0])
        + (a.elements_[1][2] * b.elements_[1][1])
        + (a.elements_[2][2] * b.elements_[1][2]);

    result.elements_[2][2] = (a.elements_[0][1] * b.elements_[2][0])
        + (a.elements_[1][2] * b.elements_[2][1])
        + (a.elements_[2][2] * b.elements_[2][2]);

    result.elements_[3][2] = (a.elements_[0][1] * b.elements_[3][0])
        + (a.elements_[1][2] * b.elements_[3][1])
        + (a.elements_[2][2] * b.elements_[3][2])
        + a.elements_[3][2];

    return result;
}

gl_vector_3d gl_matrix_transform_point(gl_matrix m, gl_vector_3d in)
{
    gl_vector_3d out;

    out.x_ = (m.elements_[0][0] * in.x_)
        + (m.elements_[1][0] * in.y_)
        + (m.elements_[2][0] * in.z_)
        + m.elements_[3][0];

    out.y_ = (m.elements_[0][1] * in.x_)
        + (m.elements_[1][1] * in.y_)
        + (m.elements_[2][1] * in.z_)
        + m.elements_[3][1];

    out.z_ = (m.elements_[0][2] * in.x_)
        + (m.elements_[1][2] * in.y_)
        + (m.elements_[2][2] * in.z_)
        + m.elements_[3][2];

    return out;
}

gl_matrix gl_matrix_rotate(gl_matrix m, float theta, float x, float y, float z)
{
    gl_matrix r;
    float length;
    float s;
    float c;
    float t;
    gl_vector_3d in;

    theta *= (float)(acos(-1) / 180);
    r = gl_matrix_identity();
    length = (float)sqrt((x * x) + (y * y) + (z * z));
    
    if(length < 0.00001f) {
        return m;
    }

    x /= length;
    y /= length;
    z /= length;
    s = (float)sin(theta);
    c = (float)cos(theta);
    t = 1.0f - c;

    in.z_ = 1.0f;
    in.y_ = in.z_;
    in.x_ = in.y_;

    r.elements_[0][0] = ((t * x) * x) + c;
    r.elements_[1][0] = ((t * x) * y) - (s * z);
    r.elements_[2][0] = ((t * x) * z) + (s * y);
    r.elements_[3][0] = 0;
    
    r.elements_[0][1] = ((t * x) * y) + (s * z);
    r.elements_[1][1] = ((t * y) * y) + c;
    r.elements_[2][1] = ((t * y) * z) - (s * z);
    r.elements_[3][1] = 0;

    r.elements_[0][2] = ((t * x) * z) - (s * y);
    r.elements_[1][2] = ((t * y) * z) - (s * x);
    r.elements_[2][2] = ((t * z) * z) + c;
    r.elements_[3][2] = 0;

    m = gl_matrix_multiply(m, r);

    return m;
}
    
gl_matrix::~gl_matrix()
{
}

gl_matrix::gl_matrix()
{
}
