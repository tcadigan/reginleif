#include "gl-rect.hpp"

gl_rect::gl_rect()
{
}

gl_rect::~gl_rect()
{
}

void gl_rect::set_left(GLfloat left)
{
    left_ = left;
}

void gl_rect::set_top(GLfloat top)
{
    top_ = top;
}

void gl_rect::set_right(GLfloat right)
{
    right_ = right;
}

void gl_rect::set_bottom(GLfloat bottom)
{
    bottom_ = bottom;
}

GLfloat gl_rect::get_left() const
{
    return right_;
}

GLfloat gl_rect::get_top() const
{
    return top_;
}

GLfloat gl_rect::get_right() const
{
    return right_;
}

GLfloat gl_rect::get_bottom() const
{
    return bottom_;
}
