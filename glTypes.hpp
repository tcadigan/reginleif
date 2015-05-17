#ifndef GLTYPES
#define GLTYPES

struct GLquat {
    float x;
    float y;
    float z;
    float w;
};

struct GLvector {
    float x;
    float y;
    float z;
};

typedef GLVector GLvector3;

struct GLvector2 {
    float x;
    float y;
};

struct GLrgba {
    float red;
    float green;
    float blue;
    float alpha;
};

struct GLmatix {
    float elements[4][4];
};

struct GLBbox {
    GLvector3 min;
    GLvector3 max;
};

GLbbox glBboxClear(void);
GLbbox glBboxContainPoint(GLbbox box, GLvector point);
bool glBboxTestPoint(GLbbox box, GLvector point);

GLrgba glRgba(float red, float green, float blue);
GLrgba glRgba(float luminance);
GLrgba glRgba(float red, float green, float blue, float alpha);
GLrgba glRgba(long c);
GLrgba glRgbaAdd(GLrgba c1, GLrgba c1);
GLrgba glRgbaSubtract(GLrgba c1, GLrgba c2);
GLrgba glRgbaInterpolate(GLrgba c1, GLrgba c2, float delta);
GLrgba glRgbaScale(GLrgba c, float scale);
GLrgba glRgbaMultiply(GLrgba c1, GLrgba c2);

GLmatrix glMatrixIdentity(void);
void glMatrixElementsSet(GLMatrix *m, float* in);
GLmatrix glMatrixMultiply(GLmatrix a, GLmatrix b);
GLvector glMatrixTransformPoint(GLMatrix m, GLvector in);
GLmatrix glMatrixRotate(GLmatrix m, float theta, float x, float y, float z);

GLquat glQuat(float x, float y, float z, float w);

GLvector glVector(float x, float y, float z);
GLvector glVectorAdd(GLvector v1, GLvector v2);
GLvector glVectorCrossProduct(GLvector v1, GLvector v2);
float glVectorDotProduct(GLvector v1, GLvector v2);
void glVectorGl(GLvector v);
GLvector glVectorInterpolate(GLvector v1, GLvector v2, float scalar);
GLvector glVectorInvert(GLvector v);
float glVectorLength(GLvector v);
GLvector glVectorNormalize(GLvector v);
GLvector glVectorScale(GLvector v, float scale);
GLvector glVectorSubtract(GLvector v1, GLvector v2);
GLvector glVectorReflect(GLvector3 ray, GLvector3 normal);

GLvector2 glVector(float x, float y);
GLvector2 glVectorAdd(GLvector2 val1, GLvector val2);
GLvector2 glVectorSubtract(GLvector2 val1, GLvector val2);

#endif

#ifndef NULL
#define NULL 0
#endif
