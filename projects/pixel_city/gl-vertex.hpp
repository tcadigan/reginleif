#ifndef __GL_VERTEX_HPP__
#define __GL_VERTEX_HPP__

#include "gl-rgba.hpp"
#include "gl-vector2.hpp"
#include "gl-vector3.hpp"

class gl_vertex {
public:
    void set_position(gl_vector3 const &position);
    void set_uv(gl_vector2 const &uv);
    void set_color(gl_rgba const &color);

    gl_vector3 get_position() const;
    gl_vector2 get_uv() const;
    gl_rgba get_color() const;

private:
    gl_vector3 position_;
    gl_vector2 uv_;
    gl_rgba color_;
};

#endif /* __GL_VERTEX_HPP__ */
