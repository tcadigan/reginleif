/*
 * texture-item.cpp
 *
 */

#include "texture-item.hpp"

texture_item::texture_item()
    : id_(0)
{
}

texture_item::~texture_item()
{
}

GLuint *texture_item::get_id_data()
{
    return &id_;
}

GLuint texture_item::get_id() const
{
    return id_;
}

std::string texture_item::get_name() const
{
    return name_;
}

void texture_item::set_id(GLuint id)
{
    id_ = id;
}

void texture_item::set_name(std::string const &name)
{
    name_ = name;
}
