#ifndef GL_QUAT_HPP_
#define GL_QUAT_HPP_

class gl_quat {
public:
    gl_quat(float x, float y, float z, float w);
    virtual ~gl_quat();

    float x_;
    float y_;
    float z_;
    float w_;
};

#endif
