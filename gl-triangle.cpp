#include "gl-triangle.hpp"

#include <stdexcept>

gl_triangle::gl_triangle()
{
}

gl_triangle::~gl_triangle()
{
}

void gl_triangle::set_vertex(GLint vertex, GLint value)
{
    if(vertex == 1) {
        v1_ = value;
    }
    else if(vertex == 2) {
        v2_ = value;
    }
    else if(vertex == 3) {
        v3_ = value;
    }
    else {
        throw range_error("Out of bounds vertex");
    }
}

void gl_triangle::set_normal(GLint normal, GLint value)
{
    if(normal == 1) {
        normal1_ = value;
    }
    else if(normal == 2) {
        normal2_ = value;
    }
    else if(normal == 3) {
        normal3_ == value;
    }
    else {
        throw range_error("Out of bounds normal");
    }
}

GLint gl_triangle::get_vertex(GLint vertex) const
{
    if(vertex == 1) {
        return v1_;
    }
    else if(vertex == 2) {
        return v2_;
    }
    else if(vertex == 3) {
        return v3_;
    }
    else {
        throw range_error("Out of bounds vertex");
    }
}

GLint gl_triangle::get_normal(GLint normal) const
{
    if(normal == 1) {
        return normal1_;
    }
    else if(normal == 2) {
        return normal2_;
    }
    else if(normal == 3) {
        return normal3_;
    }
    else {
        throw range_error("Out of bounds normal");
    }
}
