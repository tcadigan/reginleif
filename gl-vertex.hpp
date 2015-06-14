#ifndef GL_VERTEX_HPP_
#define GL_VERTEX_HPP_

#include "gl-rgba.hpp"
#include "gl-vector2.hpp"
#include "gl-vector3.hpp"

class gl_vertex {
public:
    gl_vertex();
    virtual ~gl_vertex();

    void set_position(gl_vector3 const &position);
    void set_uv(gl_vector2 const &uv);
    void set_rgba(gl_rgba const &color);

    gl_vector3 get_position() const;
    gl_vector2 get_uv() const;
    gl_rgba get_color() const;

private:
    gl_vector3 position_;
    gl_vector2 uv_;
    gl_rgba color_;
};

#endif
