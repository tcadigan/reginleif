/*
 * ztexture.cpp
 *
 */

#include "ztexture.hpp"

ztexture::ztexture()
    : ready_(false)
    , size_(0)
    , texture_(0)
{
}

ztexture::~ztexture()
{
}

void ztexture::set_ready(GLboolean ready)
{
    ready_ = ready;
}

void ztexture::set_size(GLint size)
{
    size_ = size;
}

void ztexture::set_texture(GLuint texture)
{
    texture_ = texture;
}

GLboolean ztexture::get_ready() const
{
    return ready_;
}

GLint ztexture::get_size() const
{
    return size_;
}

GLuint *ztexture::get_texture_address()
{
    return &texture_;
}

GLuint ztexture::get_texture() const
{
    return texture_;
}
