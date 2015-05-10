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

#include <GL/gl.h>
#include <GL/glu.h>

#include "primitives/types.hpp"

Mesh::Mesh()
{
    list_ = glGenLists(1);
    compiled_ = false;
    polycount_ = 0;
}

Mesh::~Mesh()
{
    glDeleteLists(list_, 1);
    vertex_.clear();
    fan_.clear();
    quad_strip_.clear();
    cube_.clear();
}

void Mesh::VertexAdd(const GLvertex &v)
{
    vertex_.push_back(v);
}

int Mesh::VertexCount()
{
    return vertex_.size();
}

int Mesh::PolyCount()
{
    return polycount_;
}

void Mesh::CubeAdd(const cube &c)
{
    cube_.push_back(c);
    polycount_ += 5;
}

void Mesh::QuadStripAdd(const quad_strip &qs)
{
    quad_strip_.push_back(qs);
    polycount_ += ((qs.index_list.size() - 2) / 2);
}

void Mesh::FanAdd(const fan &f)
{
    fan_.push_back(f);
    polycount_ += (f.index_list.size() - 2);
}

void Mesh::Render()
{
    std::vector<quad_strip>::iterator qsi;
    std::vector<cube>::iterator ci;
    std::vector<fan>::iterator fi;
    std::vector<int>::iterator n;

    if(compiled_) {
        glCallList(list_);
        return;
    }

    for(qsi = quad_strip_.begin(); qsi < quad_strip_.end(); ++qsi) {
        glBegin(GL_QUAD_STRIP);
        
        for(n = qsi->index_list.begin(); n < qsi->index_list.end(); ++n) {
            glTexCoord2fv(&vertex_[*n].uv.x);
            glVertex3fv(&vertex_[*n].position.x);
        }

        glEnd();
    }

    for(ci = cube_.begin(); ci < cube_.end(); ++ci) {
        glBegin(GL_QUAD_STRIP);

        for(n = ci->index_list.begin(); n < ci->index_list.end(); ++n) {
            glTexCoord2fv(&vertex_[*n].uv.x);
            glVertex3fv(&vertex_[*n].position.x);
        }

        glEnd();

        glBegin(GL_QUADS);
        glTexCoord2fv(&vertex_[ci->index_list[7]].uv.x);
        glVertex3fv(&vertex_[ci->index_list[7]].position.x);
        glVertex3fv(&vertex_[ci->index_list[5]].position.x);
        glVertex3fv(&vertex_[ci->index_list[3]].position.x);
        glVertex3fv(&vertex_[ci->index_list[1]].position.x);
        glEnd();

        glBegin(GL_QUADS);
        glTexCoord2fv(&vertex_[ci->index_list[6]].uv.x);
        glVertex3fv(&vertex_[ci->index_list[0]].position.x);
        glVertex3fv(&vertex_[ci->index_list[2]].position.x);
        glVertex3fv(&vertex_[ci->index_list[4]].position.x);
        glVertex3fv(&vertex_[ci->index_list[6]].position.x);
        glEnd();
    }

    for(fi = fan_.begin(); fi < fan_.end(); ++fi) {
        glBegin(GL_TRIANGLE_FAN);
        
        for(n = fi->index_list.begin(); n < fi->index_list.end(); ++n) {
            glTexCoord2fv(&vertex_[*n].uv.x);
            glVertex3fv(&vertex_[*n].position.x);
        }

        glEnd();
    }
}

void Mesh::Compile()
{
    glNewList(list_, GL_COMPILE);
    Render();
    glEndList();
    compiled_ = true;
}
