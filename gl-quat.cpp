/*
 * glQuat.cpp
 *
 * 2006 Shamus Young
 *
 * Functions for dealing with Quaternions
 *
 */

#include <cstring>

#include "gl-quat.hpp"

gl_quat::gl_quat()
{
    memset(data_, 0, sizeof(GLfloat) * 4);
}

gl_quat::ql_quat(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = w;
}

gl_quat::~gl_quat()
{
}

void gl_quat::set_data(GLfloat x, GLfloat y, Glfloat z, GLfloat w)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
    data_[3] = w;
}

void gl_quat::set_x(GLfloat x)
{
    data_[0] = x;
}

void gl_quat::set_y(GLfloat y)
{
    data_[1] = y;
}

void gl_quat::set_z(GLfloat z)
{
    data_[2] = z;
}

void gl_quat::set_w(GLfloat w)
{
    data_[3] = w;
}

GLfloat *gl_quat::get_data()
{
    return data_;
}

GLfloat gl_quat::get_x() const
{
    return data_[0];
}

GLfloat gl_quat::get_y() const
{
    return data_[1];
}

GLfloat gl_quat::get_z() const
{
    return data_[2];
}

GLfloat gl_quat::get_w() const
{
    return data_[3];
}
