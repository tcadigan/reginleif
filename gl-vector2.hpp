#ifndef GL_VECTOR2_HPP_
#define GL_VECTOR2_HPP_

class GLvector2 {
public:
    float x;
    float y;
};

GLvector2 glVector(float x, float y);
GLvector2 glVectorAdd(GLvector2 val1, GLvector2 val2);
GLvector2 glVectorSubtract(GLvector2 val1, GLvector2 val2);

#endif
