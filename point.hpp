#ifndef POINT_HPP_
#define POINT_HPP_

#include <SDL_opengl.h>

class point {
public:
    point();
    virtual ~point();

    GLint get_x() const;
    GLint get_y() const;

    void set(GLint x, GLint y);
    void set_x(GLint x);
    void set_y(GLint y);

private:
    GLint x_;
    GLint y_;
};

#endif
