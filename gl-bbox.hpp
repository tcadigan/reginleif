#ifndef GL_BBOX_HPP_
#define GL_BBOX_HPP_

#include "gl-vector3.hpp"

class GLbbox {
public:
    GLvector3 min;
    GLvector3 max;
};

bool glBboxTestPoint(GLbbox box, GLvector3 point);
GLbbox glBboxContainPoint(GLbbox box, GLvector3 point);
GLbbox glBboxClear(void);

#endif
