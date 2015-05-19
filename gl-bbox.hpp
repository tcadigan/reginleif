#ifndef GL_BBOX_HPP_
#define GL_BBOX_HPP_

#include "gl-vector3.hpp"

// Constants
#define MAX_VALUE 999999999999999.9f

class GLbbox {
public:
    GLvector3 min;
    GLvector3 max;
};

bool glBboxTestPoint(GLbbox box, GLvector3 point);
GLbbox glBboxContainPoint(GLbbox box, GLvector3 point);
GLbbox glBboxClear(void);

#endif
