/*
 * mesh.cpp
 *
 * 2009 Shamus Young
 *
 * This class is used to make constructing objects easier. It handles
 * allocating vertex lists, polygon lists, and such like.
 *
 * If you were going to implement vertex buffers, this would be the
 * place to do it. Take away the vertex_ member variable and store vertices
 * for ALL meshes in a common list, which could then be unloaded onto the
 * good ol' GPU
 *
 */

#include "mesh.hpp"

#include <vector>

Mesh::Mesh()
{
    list_ = glGenLists(1);
    compiled_ = false;
    poly_count_ = 0;
}

Mesh::~Mesh()
{
    glDeleteLists(list_, 1);
    vertex_.clear();
    fan_.clear();
    quad_strip_.clear();
    cube_.clear();
}

void Mesh::vertex_add(const gl_vertex &v)
{
    vertex_.push_back(v);
}

int Mesh::vertex_count()
{
    return vertex_.size();
}

int Mesh::poly_count()
{
    return poly_count_;
}

void Mesh::cube_add(Cube const &cube)
{
    cube_.push_back(cube);
    poly_count_ += 5;
}

void Mesh::quad_strip_add(QuadStrip const &qs)
{
    quad_strip_.push_back(qs);
    poly_count_ += ((qs.index_list.size() - 2) / 2);
}

void Mesh::fan_add(Fan const &fan)
{
    fan_.push_back(fan);
    poly_count_ += (fan.index_list.size() - 2);
}

void Mesh::render()
{
    if (compiled_) {
        glCallList(list_);
        return;
    }

    for (QuadStrip const &quad_strip : quad_strip_) {
        glBegin(GL_QUAD_STRIP);

        for (int const &index : quad_strip.index_list) {
            glTexCoord2fv(vertex_[index].get_uv().get_data());
            glVertex3fv(vertex_[index].get_position().get_data());
        }

        glEnd();
    }

    for (Cube const &cube : cube_) {
        glBegin(GL_QUAD_STRIP);

        for (int const &index : cube.index_list) {
            glTexCoord2fv(vertex_[index].get_uv().get_data());
            glVertex3fv(vertex_[index].get_position().get_data());
        }

        glEnd();

        glBegin(GL_QUADS);
        glTexCoord2fv(vertex_[cube.index_list[7]].get_uv().get_data());
        glVertex3fv(vertex_[cube.index_list[7]].get_position().get_data());
        glVertex3fv(vertex_[cube.index_list[5]].get_position().get_data());
        glVertex3fv(vertex_[cube.index_list[3]].get_position().get_data());
        glVertex3fv(vertex_[cube.index_list[1]].get_position().get_data());
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord2fv(vertex_[cube.index_list[6]].get_uv().get_data());
        glVertex3fv(vertex_[cube.index_list[0]].get_position().get_data());
        glVertex3fv(vertex_[cube.index_list[2]].get_position().get_data());
        glVertex3fv(vertex_[cube.index_list[4]].get_position().get_data());
        glVertex3fv(vertex_[cube.index_list[6]].get_position().get_data());
        glEnd();
    }

    for (Fan const &fan : fan_) {
        glBegin(GL_TRIANGLE_FAN);

        for (int const &index : fan.index_list) {
            glTexCoord2fv(vertex_[index].get_uv().get_data());
            glVertex3fv(vertex_[index].get_position().get_data());
        }

        glEnd();
    }
}

void Mesh::compile()
{
    glNewList(list_, GL_COMPILE);
    render();
    glEndList();
    compiled_ = true;
}
