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
    glColor3fv(color_.get_rgb().data());
    mesh_->render();
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
    return mesh_->poly_count();
}

unsigned Decoration::Texture()
{
    return texture_;
}

void Decoration::CreateRadioTower(gl_vector3 pos, float height)
{
    Light *l;
    float offset;
    gl_vertex v;
    Fan fan;

    for(int i = 0; i < 6; ++i) {
        fan.index_list.push_back(i);
    }

    offset = height / 15.0f;
    center_ = pos;
    use_alpha_ = true;

    // Radio tower
    v.set_position(gl_vector3(center_.get_x(),
                              center_.get_y() + height,
                              center_.get_z()));

    v.set_uv(gl_vector2(0, 1));
    mesh_->vertex_add(v);

    v.set_position(gl_vector3(center_.get_x() - offset,
                              center_.get_y(),
                              center_.get_z() - offset));

    v.set_uv(gl_vector2(1, 0));
    mesh_->vertex_add(v);

    v.set_position(gl_vector3(center_.get_x() + offset,
                              center_.get_y(),
                              center_.get_z() - offset));

    v.set_uv(gl_vector2(0, 0));
    mesh_->vertex_add(v);

    v.set_position(gl_vector3(center_.get_x() + offset,
                              center_.get_y(),
                              center_.get_z() + offset));

    v.set_uv(gl_vector2(1, 0));
    mesh_->vertex_add(v);

    v.set_position(gl_vector3(center_.get_x() - offset,
                              center_.get_y(),
                              center_.get_z() + offset));

    v.set_uv(gl_vector2(0, 0));
    mesh_->vertex_add(v);

    v.set_position(gl_vector3(center_.get_x() - offset,
                              center_.get_y(),
                              center_.get_z() - offset));

    v.set_uv(gl_vector2(1, 0));
    mesh_->vertex_add(v);

    mesh_->fan_add(fan);

    l = new Light(gl_vector3(center_.get_x(),
                             center_.get_y() + height + 1.0f,
                             center_.get_z()),
                  gl_rgba(255, 192, 160),
                  1);
    l->Blink();

    texture_ = TextureId(TEXTURE_LATTICE);
}

void Decoration::CreateLogo(gl_vector2 start,
                            gl_vector2 end,
                            float bottom,
                            int seed,
                            gl_rgba color)
{
    gl_vertex p;
    QuadStrip qs;
    float u1;
    float u2;
    float v1;
    float v2;
    float top;
    float height;
    float length;
    gl_vector2 center2d;
    gl_vector3 to;
    gl_vector3 out;
    int logo_index;

    qs.index_list.push_back(0);
    qs.index_list.push_back(1);
    qs.index_list.push_back(3);
    qs.index_list.push_back(2);

    use_alpha_ = true;
    color_ = color;
    logo_index = seed % LOGO_ROWS;

    to = gl_vector3(start.get_x(), 0.0f, start.get_y())
        - gl_vector3(end.get_x(), 0.0f, end.get_y());

    to.normalize();

    out = cross_product(gl_vector3(0.0f, 1.0f, 0.0f), to) * LOGO_OFFSET;

    center2d = (start + end) / 2;
    center_ = gl_vector3(center2d.get_x(), bottom, center2d.get_y());
    gl_vector2 temp = start - end;
    length= temp.length();
    height = (length / 8.0f) * 1.5f;
    top = bottom + height;
    u1 = 0.0f;
    u2 = 0.5f; // We actually only use the left half of the texture
    v1 = (float)logo_index / LOGO_ROWS;
    v2 = v1 + (1.0f / LOGO_ROWS);

    p.set_position(gl_vector3(start.get_x(), bottom, start.get_y()) + out);
    p.set_uv(gl_vector2(u1, v1));
    mesh_->vertex_add(p);

    p.set_position(gl_vector3(end.get_x(), bottom, end.get_y()) + out);
    p.set_uv(gl_vector2(u2, v1));
    mesh_->vertex_add(p);

    p.set_position(gl_vector3(end.get_x(), top, end.get_y()) + out);
    p.set_uv(gl_vector2(u2, v2));
    mesh_->vertex_add(p);

    p.set_position(gl_vector3(start.get_x(), top, start.get_y()) + out);
    p.set_uv(gl_vector2(u1, v2));
    mesh_->vertex_add(p);

    mesh_->quad_strip_add(qs);

    texture_ = TextureId(TEXTURE_LOGOS);
}

void Decoration::CreateLightStrip(float x,
                                  float z,
                                  float width,
                                  float depth,
                                  float height,
                                  gl_rgba color)
{
    gl_vertex p;
    QuadStrip qs1;
    float u;
    float v;

    qs1.index_list.push_back(0);
    qs1.index_list.push_back(1);
    qs1.index_list.push_back(3);
    qs1.index_list.push_back(2);

    color_ = color;
    use_alpha_ = true;
    center_ = gl_vector3(x + (width / 2), height, z + (depth / 2));
    if(width > depth) {
        u = 1.0f;
        v = (float)((int)(depth / width));
    }
    else {
        v = 1.0f;
        u = (float)((int)(width / depth));
    }

    texture_ = TextureId(TEXTURE_LIGHT);

    p.set_position(gl_vector3(x, height, z));
    p.set_uv(gl_vector2(0.0f, 0.0f));
    mesh_->vertex_add(p);

    p.set_position(gl_vector3(x, height, z + depth));
    p.set_uv(gl_vector2(0.0f, v));
    mesh_->vertex_add(p);

    p.set_position(gl_vector3(x + width, height, z + depth));
    p.set_uv(gl_vector2(u, v));
    mesh_->vertex_add(p);

    p.set_position(gl_vector3(x + width, height, z));
    p.set_uv(gl_vector2(u, 0.0f));
    mesh_->vertex_add(p);

    mesh_->quad_strip_add(qs1);

    mesh_->compile();
}

void Decoration::CreateLightTrim(std::array<gl_vector3, MAX_VBUFFER> &chain,
                                 int count,
                                 float height,
                                 int seed,
                                 gl_rgba color)
{
    gl_vertex p;
    gl_vector3 to;
    gl_vector3 out;
    int i;
    int index;
    int prev;
    int next;
    float u;
    float v1;
    float v2;
    float row;
    QuadStrip qs;

    color_ = color;
    center_ = gl_vector3(0.0f, 0.0f, 0.0f);
    qs.index_list.reserve((count * 2) + 2);

    for(i = 0; i < count; ++i) {
        center_ = center_ + chain.at(i);
    }

    center_ /= (float)count;
    row = (float)(seed % TRIM_ROWS);
    v1 = row * TRIM_SIZE;
    v2 = (row + 1.0f) * TRIM_SIZE;
    index = 0;
    u = 0.0f;

    for(i = 0; i < (count + 1); ++i) {
        if(i) {
            gl_vector3 temp = chain.at(i % count) - p.get_position();
            u += (temp.length() * 0.1f);
        }

        // Add the bottom point
        prev = i - 1;
        if(prev < 0) {
            prev = count + prev;
        }

        next = (i + 1) % count;
        to = chain.at(next) - chain.at(prev);
        to.normalize();
        out = cross_product(gl_vector3(0.0f, 1.0f, 0.0f), to) * LOGO_OFFSET;

        p.set_position(chain.at(i % count) + out);
        p.set_uv(gl_vector2(u, v2));
        mesh_->vertex_add(p);

        qs.index_list.push_back(index++);

        // Top point
        p.get_position().set_y(p.get_position().get_y() + height);
        p.set_uv(gl_vector2(u, v1));
        mesh_->vertex_add(p);

        qs.index_list.push_back(index++);
    }

    mesh_->quad_strip_add(qs);

    texture_ = TextureId(TEXTURE_TRIM);

    mesh_->compile();
}
