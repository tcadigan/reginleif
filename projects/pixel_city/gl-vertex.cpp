#include "gl-vertex.hpp"

void gl_vertex::set_position(gl_vector3 const &position) {
  position_ = position;
}

void gl_vertex::set_uv(gl_vector2 const &uv) { uv_ = uv; }

void gl_vertex::set_color(gl_rgba const &color) { color_ = color; }

gl_vector3 gl_vertex::get_position() const { return position_; }

gl_vector2 gl_vertex::get_uv() const { return uv_; }

gl_rgba gl_vertex::get_color() const { return color_; }
