/*
 * decoration.cpp
 *
 * 2009 Shamus Young
 *
 * This handles building and rendering decoration objects - infrastructure
 * & such around the city.
 *
 */

#include "decoration.hpp"

#include <cmath>

#include <GL/gl.h>
#include <GL/glu.h>

#include "light.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "world.hpp"

#define LOGO_OFFSET 0.2f // How far a logo sticks out from the given surface

Decoration::~Decoration()
{
    delete mesh_;
}

Decoration::Decoration()
{
    mesh_ = new Mesh();
    use_alpha_ = false;
}

void Decoration::Render()
{
    glColor3fv(&color_.red);
    mesh_->Render();
}

void Decoration::RenderFlat(bool colored)
{
}

bool Decoration::Alpha()
{
    return use_alpha_;
}

int Decoration::PolyCount()
{
    return mesh_->PolyCount();
}

unsigned Decoration::Texture()
{
    return texture_;
}

void Decoration::CreateRadioTower(GLvector pos, float height)
{
    Light *l;
    float offset;
    GLvertex v;
    fan f;

    for(int i = 0; i < 6; ++i) {
        f.index_list.push_back(i);
    }

    offset = height / 15.0f;
    center_ = pos;
    use_alpha_ = true;

    // Radio tower
    v.position = glVector(center_.x, center_.y + height, center_.z);
    v.uv = glVector(0, 1);
    mesh_->VertexAdd(v);
    
    v.position = glVector(center_.x - offset, center_.y, center_.z - offset);
    v.uv = glVector(1, 0);
    mesh_->VertexAdd(v);
    
    v.position = glVector(center_.x + offset, center_.y, center_.z - offset);
    v.uv = glVector(0, 0);
    mesh_->VertexAdd(v);

    v.position = glVector(center_.x + offset, center_.y, center_.z + offset);
    v.uv = glVector(1, 0);
    mesh_->VertexAdd(v);

    v.position = glVector(center_.x - offset, center_.y, center_.z + offset);
    v.uv = glVector(0, 0);
    mesh_->VertexAdd(v);

    v.position = glVector(center_.x - offset, center_.y, center_.z - offset);
    v.uv = glVector(1, 0);
    mesh_->VertexAdd(v);

    mesh_->FanAdd(f);

    l = new Light(glVector(center_.x, center_.y + height + 1.0f, center_.z),
                   glRgba(255, 192, 160),
                   1);
    l->Blink();
    
    texture_ = TextureId(TEXTURE_LATTICE);
}

void Decoration::CreateLogo(GLvector2 start,
                            GLvector2 end,
                            float bottom, 
                            int seed,
                            GLrgba color)
{
    GLvertex p;
    quad_strip qs;
    float u1;
    float u2;
    float v1;
    float v2;
    float top;
    float height;
    float length;
    GLvector2 center2d;
    GLvector to;
    GLvector out;
    int logo_index;

    qs.index_list.push_back(0);
    qs.index_list.push_back(1);
    qs.index_list.push_back(3);
    qs.index_list.push_back(2);

    use_alpha_ = true;
    color_ = color;
    logo_index = seed % LOGO_ROWS;

    to = glVector(start.x, 0.0f, start.y) - glVector(end.x, 0.0f, end.y);
    to = glVectorNormalize(to);

    out = glVectorCrossProduct(glVector(0.0f, 1.0f, 0.0f), to) * LOGO_OFFSET;
    
    center2d = (start + end) / 2;
    center_ = glVector(center2d.x, bottom, center2d.y);
    length = glVectorLength(start - end);
    height = (length / 8.0f) * 1.5f;
    top = bottom + height;
    u1 = 0.0f;
    u2 = 0.5f; // We actually only use the left half of the texture
    v1 = (float)logo_index / LOGO_ROWS;
    v2 = v1 + (1.0f / LOGO_ROWS);
    
    p.position = glVector(start.x, bottom, start.y) + out;
    p.uv = glVector(u1, v1);
    mesh_->VertexAdd(p);

    p.position = glVector(end.x, bottom, end.y) + out;
    p.uv = glVector(u2, v1);
    mesh_->VertexAdd(p);

    p.position = glVector(end.x, top, end.y) + out;
    p.uv = glVector(u2, v2);
    mesh_->VertexAdd(p);

    p.position = glVector(start.x, top, start.y) + out;
    p.uv = glVector(u1, v2);
    mesh_->VertexAdd(p);

    mesh_->QuadStripAdd(qs);

    texture_ = TextureId(TEXTURE_LOGOS);
}

void Decoration::CreateLightStrip(float x,
                                  float z,
                                  float width,
                                  float depth,
                                  float height,
                                  GLrgba color)
{
    GLvertex p;
    quad_strip qs1;
    float u;
    float v;

    qs1.index_list.push_back(0);
    qs1.index_list.push_back(1);
    qs1.index_list.push_back(3);
    qs1.index_list.push_back(2);

    color_ = color;
    use_alpha_ = true;
    center_ = glVector(x + (width / 2), height, z + (depth / 2));
    if(width > depth) {
        u = 1.0f;
        v = (float)((int)(depth / width));
    }
    else {
        v = 1.0f;
        v = (float)((int)(width / depth));
    }

    texture_ = TextureId(TEXTURE_LIGHT);

    p.position = glVector(x, height, z);
    p.uv = glVector(0.0f, 0.0f);
    mesh_->VertexAdd(p);

    p.position = glVector(x, height, z + depth);
    p.uv = glVector(0.0f, v);
    mesh_->VertexAdd(p);

    p.position = glVector(x + width, height, z + depth);
    p.uv = glVector(u, v);
    mesh_->VertexAdd(p);

    p.position = glVector(x + width, height, z); 
    p.uv = glVector(u, 0.0f);
    mesh_->VertexAdd(p);

    mesh_->QuadStripAdd(qs1);

    mesh_->Compile();
}

void Decoration::CreateLightTrim(GLvector *chain,
                                 int count,
                                 float height,
                                 int seed,
                                 GLrgba color)
{
    GLvertex p;
    GLvertex to;
    GLvertex out;
    int i;
    int index;
    int prev;
    int next;
    float u;
    float v1;
    float v2;
    float row;
    quad_strip qs;

    color_ = color;
    center_ = glVector(0.0f, 0.0f, 0.0f);
    qs.index_list.reserve((count * 2) + 2);

    for(i = 0; i < count; ++i) {
        center_ += chain[i];
    }

    center_ /= (float)count;
    row = (float)(seed % TRIM_ROWS);
    v1 = row * TRIM_SIZE;
    v2 = (row + 1.0f) * TRIM_SIZE;
    index = 0;
    u = 0.0f;

    for(i = 0; i < (count + 1); ++i) {
        if(i) {
            u += glVectorLength(chain[i % count] - p.position) * 0.1f;
        }

        // Add the bottom point
        prev = i - 1;
        if(prev < 0) {
            prev = count + prev;
        }

        next = (i + 1) % count;
        to = glVectorNormalize(chain[next] - chain[prev]);
        out = 
            glVectorCrossProduct(glVector(0.0f, 1.0f, 0.0f), to) * LOGO_OFFSET;

        p.position = chain[i % count] + out;
        p.uv = glVector(u, v2);
        mesh_->VertexAdd(p);

        qs.index_list.push_back(index++);

        // Top point
        p.position.y += height;
        p.uv = glVector(u, v1);
        mesh_->VertexAdd(p);

        qs.index_list.push_back(index++);
    }

    mesh_->QuadStripAdd(qs);

    texture_ = TextureId(TEXTURE_TRIM);
    
    mesh_->Compile();
}
