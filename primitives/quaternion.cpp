/*
 * glQuat.cpp
 *
 * 2006 Shamus Young
 *
 * Functions for dealing with Quaternions
 *
 */

#include <cfloat>
#include <cmath>

#include <GL/gl.h>

#include "glTypes.hpp"
#include "Math.hpp"

enum QuatPart {
    X,
    Y,
    Z,
    W
};

GLquat glQuat(float x, float y, float z, float w)
{
    GLquat result;

    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;

    return result;
}

// Convert quaternion to Euler angles (in radians)
// EulerAngles Eul_FromQuat(Quat q, int order)
// {
//     HMatrix M;
//     double Nq = (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
//     double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;

//     double zs = q.z * s;
//     double ys = q.y * s;
//     double xs = q.x * s;

//     double wz = q.w * zs;
//     double wy = q.w * ys;
//     double wx = q.w * xs;

//     double xz = q.x * zs;
//     double xy = q.x * ys;
//     double xx = q.x * xs;

//     double zz = q.z * zs;
//     double yz = q.y * zs;
//     double yy = q.y * ys;

//     M[X][Z] = xz + wy;
//     M[X][Y] = xy - wz;
//     M[X][X] = 1.0 - (yy + zz);

//     M[Y][Z] = yz - wx;
//     M[Y][Y] = 1.0 - (xx + zz);
//     M[Y][X] = xy + wz;

//     M[Z][Z] = 1.0 - (xx + yy);
//     M[Z][Y] = yz + wx;
//     M[Z][X] = xz - wy;

//     M[W][W] = 1.0;

//     M[Z][W] = 0.0;
//     M[Y][W] = 0.0;
//     M[X][W] = 0.0;
//     M[W][Z] = 0.0;
//     M[W][Y] = 0.0;
//     M[W][X] = 0.0;

//     return Eul_FromHMatrix(M, order);
// }

GLvector glQuatToEuler(GLquat q, int order)
{
    GLmatrix M;

    float Nq = (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    float s = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;

    float zs = q.z * s;
    float ys = q.y * s;
    float xs = q.x * s;

    float wz = q.w * zs;
    float wy = q.w * ys;
    float wx = q.w * xs;

    float xz = q.x * zs;
    float xy = q.x * ys;
    float xx = q.x * xs;

    float zz = q.z * zs;
    float yz = q.y * zs;
    float yy = q.y * ys;

    M.elements[X][Z] = xz + wy;
    M.elements[X][Y] = xy - wz;
    M.elements[X][X] = 1.0f - (yy + zz);

    M.elements[Y][Z] = yz - wx;
    M.elements[Y][Y] = 1.0f - (xx + zz);
    M.elements[Y][X] = xy + wz;

    M.elements[Z][Z] = 1.0f - (xx + yy);
    M.elements[Z][Y] = yz + wx;
    M.elements[Z][X] = xz - wy;

    M.elements[Z][W] = 0.0f;
    M.elements[Y][W] = 0.0f;
    M.elements[X][W] = 0.0f;

    M.elements[W][Z] = 0.0f;
    M.elements[W][Y] = 0.0f;
    M.elements[W][X] = 0.0f;

    M.elements[W][W] = 1.0f;

    return glMatrixToEuler(M, order);
}
