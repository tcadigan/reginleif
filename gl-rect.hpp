#ifndef GL_RECT_HPP_
#define GL_RECT_HPP_

#include <SDL_opengl.h>

class gl_rect {
public:
    gl_rect();
    virtual ~gl_rect();

    void set_left(GLfloat left);
    void set_top(GLfloat top);
    void set_right(GLfloat right);
    void set_bottom(GLfloat bottom);

    GLfloat get_left() const;
    GLfloat get_top() const;
    GLfloat get_right() const;
    GLfloat get_bottom() const;

private:
    GLfloat left_;
    GLfloat top_;
    GLfloat right_;
    GLfloat bottom_;
};

#endif
