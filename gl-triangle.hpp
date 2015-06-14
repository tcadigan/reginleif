#ifndef GL_TRIANGLE_HPP_
#define GL_TRIANGLE_HPP_

#include <SDL_opengl.h>

class gl_triangle {
public:
    gl_triangle();
    virtual ~gl_triangle();

    void set_vertex(GLint vertex, GLint value);
    void set_normal(GLint normal, GLint value);

    GLint get_vertex(GLint vertex) const;
    GLint get_normal(GLint normal) const;

private:
    GLint v1_;
    GLint v2_;
    GLint v3_;
    GLint normal1_;
    GLint normal2_;
    GLint normal3_;
};

#endif
