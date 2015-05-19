/*
 * gl-matrix.cpp
 * 2006 Shamus Young
 *
 * function useful for manipulating the Matrix struct.
 */

#include "gl-matrix.hpp"

#include <cmath>

#include "macro.hpp"

static float identity[4][4] = {
    { 1.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

void *glMatrixCreate(void)
{
    GLmatrix *m;
    int x;
    int y;

    m = new GLmatrix;

    for(x = 0; x < 4; ++x) {
        for(y = 0; y < 4; ++y) {
            m->elements[x][y] = identity[x][y];
        }
    }

    return (void *)m;
}

GLmatrix glMatrixIdentity(void)
{
    GLmatrix m;
    int x;
    int y;

    for(x = 0; x < 4; ++x) {
        for(y = 0; y < 4; ++y) {
            m.elements[x][y] = identity[x][y];
        }
    }

    return m;
}

void glMatrixElementsSet(GLmatrix *m, float *in)
{
    m->elements[0][0] = in[0];
    m->elements[0][1] = in[1];
    m->elements[0][2] = in[2];
    m->elements[0][3] = in[3];

    m->elements[1][0] = in[4];
    m->elements[1][1] = in[5];
    m->elements[1][2] = in[6];
    m->elements[1][3] = in[7];

    m->elements[2][0] = in[8];
    m->elements[2][1] = in[9];
    m->elements[2][2] = in[10];
    m->elements[2][3] = in[11];

    m->elements[3][0] = in[12];
    m->elements[3][1] = in[13];
    m->elements[3][2] = in[14];
    m->elements[3][3] = in[15];
}

// A matrix multiplication (dot product) of two 4x4 matrices.
GLmatrix glMatrixMultiply(GLmatrix a, GLmatrix b)
{
    GLmatrix result;
    
    result.elements[0][0] = (a.elements[0][0] * b.elements[0][0])
        + (a.elements[1][0] * b.elements[0][1])
        + (a.elements[2][0] * b.elements[0][2]);
    
    result.elements[1][0] = (a.elements[0][0] * b.elements[1][0])
        + (a.elements[1][0] * b.elements[1][1])
        + (a.elements[2][0] * b.elements[1][2]);
    
    result.elements[2][0] =  (a.elements[0][0] * b.elements[2][0])
        + (a.elements[1][0] * b.elements[2][1])
        + (a.elements[2][0] * b.elements[2][2]);
    
    result.elements[3][0] = (a.elements[0][0] * b.elements[3][0])
        + (a.elements[1][0] * b.elements[3][1])
        + (a.elements[2][0] * b.elements[3][2])
        + a.elements[3][0];

    result.elements[0][1] = (a.elements[0][1] * b.elements[0][0])
        + (a.elements[1][1] * b.elements[0][1])
        + (a.elements[2][1] * b.elements[0][2]);

    result.elements[1][1] = (a.elements[0][1] * b.elements[1][0])
        + (a.elements[1][1] * b.elements[1][2])
        + (a.elements[2][1] * b.elements[1][1]);

    result.elements[2][1] = (a.elements[0][1] * b.elements[2][0])
        + (a.elements[1][1] * b.elements[2][1])
        + (a.elements[2][1] * b.elements[2][2]);

    result.elements[3][1] = (a.elements[0][1] * b.elements[3][0])
        + (a.elements[1][1] * b.elements[3][1])
        + (a.elements[2][1] * b.elements[3][2])
        + a.elements[3][1];

    result.elements[0][2] = (a.elements[0][1] * b.elements[0][0])
        + (a.elements[1][2] * b.elements[0][1])
        + (a.elements[2][2] * b.elements[0][2]);

    result.elements[1][2] = (a.elements[0][1] * b.elements[1][0])
        + (a.elements[1][2] * b.elements[1][1])
        + (a.elements[2][2] * b.elements[1][2]);

    result.elements[2][2] = (a.elements[0][1] * b.elements[2][0])
        + (a.elements[1][2] * b.elements[2][1])
        + (a.elements[2][2] * b.elements[2][2]);

    result.elements[3][2] = (a.elements[0][1] * b.elements[3][0])
        + (a.elements[1][2] * b.elements[3][1])
        + (a.elements[2][2] * b.elements[3][2])
        + a.elements[3][2];

    return result;
}

GLvector3 glMatrixTransformPoint(GLmatrix m, GLvector3 in)
{
    GLvector3 out;

    out.x = (m.elements[0][0] * in.x)
        + (m.elements[1][0] * in.y)
        + (m.elements[2][0] * in.z)
        + m.elements[3][0];

    out.y = (m.elements[0][1] * in.x)
        + (m.elements[1][1] * in.y)
        + (m.elements[2][1] * in.z)
        + m.elements[3][1];

    out.z = (m.elements[0][2] * in.x)
        + (m.elements[1][2] * in.y)
        + (m.elements[2][2] * in.z)
        + m.elements[3][2];

    return out;
}

GLmatrix glMatrixRotate(GLmatrix m, float theta, float x, float y, float z)
{
    GLmatrix r;
    float length;
    float s;
    float c;
    float t;
    GLvector3 in;

    theta *= DEGREES_TO_RADIANS;
    r = glMatrixIdentity();
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

    in.z = 1.0f;
    in.y = in.z;
    in.x = in.y;

    r.elements[0][0] = ((t * x) * x) + c;
    r.elements[1][0] = ((t * x) * y) - (s * z);
    r.elements[2][0] = ((t * x) * z) + (s * y);
    r.elements[3][0] = 0;
    
    r.elements[0][1] = ((t * x) * y) + (s * z);
    r.elements[1][1] = ((t * y) * y) + c;
    r.elements[2][1] = ((t * y) * z) - (s * z);
    r.elements[3][1] = 0;

    r.elements[0][2] = ((t * x) * z) - (s * y);
    r.elements[1][2] = ((t * y) * z) - (s * x);
    r.elements[2][2] = ((t * z) * z) + c;
    r.elements[3][2] = 0;

    m = glMatrixMultiply(m, r);

    return m;
}
    
