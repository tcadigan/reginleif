#ifndef GL_VECTOR3_HPP_
#define GL_VECTOR3_HPP_

class GLvector3 {
public:
    float x;
    float y;
    float z;
};

GLvector3 glVector(float x, float y, float z);
GLvector3 glVectorAdd(GLvector3 v1, GLvector3 v2);
GLvector3 glVectorCrossProduct(GLvector3 v1, GLvector3 v2);
float glVectorDotProduct(GLvector3 v1, GLvector3 v2);
void glVectorGl(GLvector3 v);
GLvector3 glVectorInterpolate(GLvector3 v1, GLvector3 v2, float scalar);
GLvector3 glVectorInvert(GLvector3 v);
float glVectorLength(GLvector3 v);
GLvector3 glVectorNormalize(GLvector3 v);
GLvector3 glVectorScale(GLvector3 v, float scale);
GLvector3 glVectorSubtract(GLvector3 v1, GLvector3 v2);
GLvector3 glVectorReflect(GLvector3 ray, GLvector3 normal);

#endif
