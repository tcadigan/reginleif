#ifndef GL_QUAT_HPP_
#define GL_QUAT_HPP_

#include <SDL2/SDL_opengl.h>

class gl_quat {
public:
    gl_quat();
    gl_quat(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    virtual ~gl_quat();

    void set_data(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void set_x(GLfloat x);
    void set_y(GLfloat y);
    void set_z(GLfloat z);
    void set_w(GLfloat w);

    GLfloat *get_data();
    GLfloat get_x() const;
    GLfloat get_y() const;
    GLfloat get_z() const;
    GLfloat get_w() const;

private:
    GLfloat data_[4];
};

#endif
