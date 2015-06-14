/*
 * building.cpp
 *
 * 2009 Shamus Young
 *
 * This module contains the class to construct the buildings.
 *
 */

#include "building.hpp"

#include <cmath>

#include <GL/gl.h>

#include "decoration.hpp"
#include "gl-vector2.hpp"
#include "gl-vector3.hpp"
#include "gl-vertex.hpp"
#include "light.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "random.hpp"
#include "texture.hpp"
#include "win.hpp"
#include "world.hpp"

#define MAX_VBUFFER 256

// This is used by the recursive roof builder to decide what items may be added.
enum {
    ADDON_NONE,
    ADDON_LOGO,
    ADDON_TRIM,
    ADDON_LIGHTS,
    ADDON_COUNT
};

static gl_vector3 vector_buffer[MAX_VBUFFER];

/*
 * This is the constructor for out building constructor.
 */
Building::Building(int type,
                   int x,
                   int y,
                   int height,
                   int width,
                   int depth,
                   int seed,
                   gl_rgba color)
{
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
    center_ = 
        gl_vector3((float)(x_ + (width / 2)), 0.0f, (float)(y_ + (depth / 2)));
    seed_ = seed;
    texture_type_ = RandomVal();
    color_ = color;
    color_.set_alpha(0.1f);
    have_lights_ = false;
    have_logo_ = false;
    have_trim_ = false;
    roof_tiers_ = 0;
    trim_color_ = WorldLightColor(seed); // Pick a color for logos & roof lights
    mesh_ = new Mesh; // The main textured mesh for the building
    mesh_flat_ = new Mesh; // Flat-color mesh for untextured detail items
    
    switch(type) {
    case BUILDING_SIMPLE:
        CreateSimple();
        break;
    case BUILDING_MODERN:
        CreateModern();
        break;
    case BUILDING_TOWER:
        CreateTower();
        break;
    case BUILDING_BLOCKY:
        CreateBlocky();
        break;
    }
}

Building::~Building()
{
    if(mesh_ != NULL) {
        delete(mesh_);
    }
    if(mesh_flat_ != NULL) {
        delete(mesh_flat_);
    }
}

unsigned int Building::Texture()
{
    return TextureRandomBuilding(texture_type_);
}

int Building::PolyCount()
{
    return(mesh_->PolyCount() + mesh_flat_->PolyCount());
}

void Building::Render()
{
    glColor3fv(color_.get_data());
    mesh_->Render();
}

void Building::RenderFlat(bool colored)
{
    if(colored) {
        glColor3fv(color_.get_data());
    }

    mesh_flat_->Render();
}

void Building::ConstructCube(int left,
                             int right,
                             int front,
                             int back,
                             int bottom,
                             int top)
{
    gl_vertex p[10];
    float x1;
    float x2;
    float z1;
    float z2;
    float y1;
    float y2;
    int i;
    cube c;
    float u;
    float v1;
    float v2;
    float mapping;
    int base_index;

    x1 = (float)left;
    x2 = (float)right;
    y1 = (float)bottom;
    y2 = (float)top;
    z1 = (float)front;
    z2 = (float)back;
    base_index = mesh_->VertexCount();

    mapping = (float)SEGMENTS_PER_TEXTURE;
    u = (float)(RandomVal() % SEGMENTS_PER_TEXTURE) / (float)mapping;
    v1 = (float)bottom / (float)mapping;
    v2 = (float)top / (float)mapping;

    p[0].set_position(gl_vector3(x1, y1, z1));
    p[0].set_uv(gl_vector2(u, v1));
    p[1].set_position(gl_vector3(x1, y2, z1));
    p[1].set_uv(gl_vector2(u, v2));

    u += (float)width_ / mapping;
    p[2].set_position(gl_vector3(x2, y1, z1));
    p[2].set_uv(gl_vector2(u, v1));
    p[3].set_position(gl_vector3(x2, y2, z1));
    p[3].set_uv(gl_vector2(u, v2));

    u += (float)depth_ / mapping;
    p[4].set_position(gl_vector3(x2, y1, z2));
    p[4].set_uv(gl_vector2(u, v1));
    p[5].set_position(gl_vector3(x2, y2, z2));
    p[5].set_uv(gl_vector2(u, v2));

    u += (float)width_ / mapping;
    p[6].set_position(gl_vector3(x1, y1, z2));
    p[6].set_uv(gl_vector2(u, v1));
    p[7].set_position(gl_vector3(x1, y2, z2));
    p[7].set_uv(gl_vector2(u, v2));

    u += (float)width_ / mapping;
    p[8].set_position(gl_vector3(x1, y1, z1));
    p[8].set_uv(gl_vector2(u, v1));
    p[9].set_position(gl_vector3(x1, y2, z1));
    p[9].set_uv(gl_vector2(u, v2));

    for(i = 0; i < 10; ++i) {
        GLfloat pos_x = p[i].get_position().get_x();
        GLfloat pos_z = p[i].get_position().get_z();
        p[i].get_uv().set_x((pos_x + pos_z) / (float)mapping);
        mesh_->VertexAdd(p[i]);
        c.index_list.push_back(base_index + i);
    }

    mesh_->CubeAdd(c);
}

void Building::ConstructCube(float left,
                             float right,
                             float front,
                             float back,
                             float bottom,
                             float top)
{
    gl_vertex p[10];
    float x1;
    float x2;
    float z1;
    float z2;
    float y1;
    float y2;
    int i;
    cube c;
    int base_index;

    x1 = left;
    x2 = right;
    y1 = bottom;
    y2 = top;
    z1 = front;
    z2 = back;
    base_index = mesh_flat_->VertexCount();

    p[0].set_position(gl_vector3(x1, y1, z1));
    p[0].set_uv(gl_vector2(0.0f, 0.0f));

    p[1].set_position(gl_vector3(x1, y2, z1));
    p[1].set_uv(gl_vector2(0.0f, 0.0f));

    p[2].set_position(gl_vector3(x2, y1, z1));
    p[2].set_uv(gl_vector2(0.0f, 0.0f));

    p[3].set_position(gl_vector3(x2, y2, z1));
    p[3].set_uv(gl_vector2(0.0f, 0.0f));

    p[4].set_position(gl_vector3(x2, y1, z2));
    p[4].set_uv(gl_vector2(0.0f, 0.0f));

    p[5].set_position(gl_vector3(x2, y2, z2));
    p[5].set_uv(gl_vector2(0.0f, 0.0f));

    p[6].set_position(gl_vector3(x1, y1, z2));
    p[6].set_uv(gl_vector2(0.0f, 0.0f));

    p[7].set_position(gl_vector3(x1, y2, z2));
    p[7].set_uv(gl_vector2(0.0f, 0.0f));

    p[8].set_position(gl_vector3(x1, y1, z1));
    p[8].set_uv(gl_vector2(0.0f, 0.0f));

    p[9].set_position(gl_vector3(x1, y2, z1));
    p[9].set_uv(gl_vector2(0.0f, 0.0f));

    for(i = 0; i < 10; ++i) {
        GLfloat pos_x = p[i].get_position().get_x();
        GLfloat pos_z = p[i].get_position().get_z();
        p[i].get_uv().set_x((pos_x + pos_z) / (float)SEGMENTS_PER_TEXTURE);

        mesh_flat_->VertexAdd(p[i]);
        c.index_list.push_back(base_index + i);
    }

    mesh_flat_->CubeAdd(c);
}

/*
 * This will take the given area and populate it with rooftop stuff like
 * air conditioners or light towers.
 */
void Building::ConstructRoof(float left,
                             float right,
                             float front,
                             float back,
                             float bottom)
{
    int air_conditioners;
    int i;
    int width;
    int depth;
    int height;
    int face;
    int addon;
    int max_tiers;
    float ac_x;
    float ac_y;
    float ac_base;
    float ac_size;
    float ac_height;
    float logo_offset;
    Decoration *d;
    gl_vector2 start;
    gl_vector2 end;

    roof_tiers_++;
    max_tiers = height_ / 10;
    width = (int)(right - left);
    depth = (int)(back - front);
    height = 5 - roof_tiers_;
    logo_offset = 0.2f;

    // See if this build is special and worth of fancy roof decorations.
    if(bottom > 35.0f) {
        addon = RandomVal(ADDON_COUNT);
    }

    // Build the roof slab
    ConstructCube(left, right, front, back, bottom, bottom + (float)height);

    // Consider putting a logo on the root, if it's tall enough
    if((addon == ADDON_LOGO) && !have_logo_) {
        d = new Decoration;
        
        if(width > depth) {
            if(COIN_FLIP) {
                face = NORTH;
            }
            else {
                face = SOUTH;
            }
        }
        else {
            if(COIN_FLIP) {
                face = EAST;
            }
            else {
                face = WEST;
            }
        }

        switch(face) {
        case NORTH:
            start = gl_vector2((float)left, (float)back + logo_offset);
            end = gl_vector2((float)right, (float)back + logo_offset);
            break;
        case SOUTH:
            start = gl_vector2((float) right, (float)front - logo_offset);
            end = gl_vector2((float)right, (float)front - logo_offset);
            break;
        case EAST:
            start = gl_vector2((float)right + logo_offset, (float)back);
            end = gl_vector2((float)right + logo_offset, (float)front);
            break;
        case WEST:
        default:
            start = gl_vector2((float)left - logo_offset, (float)front);
            end = gl_vector2((float)left - logo_offset, (float)back);
            break;
        }

        d->CreateLogo(start, end, bottom, WorldLogoIndex(), trim_color_);
        have_logo_ = true;
    }
    else if(addon == ADDON_TRIM) {
        d = new Decoration;

        vector_buffer[0] = gl_vector3(left, bottom, back);
        vector_buffer[1] = gl_vector3(left, bottom, front);
        vector_buffer[2] = gl_vector3(right, bottom, front);
        vector_buffer[3] = gl_vector3(right, bottom, back);

        d->CreateLightTrim(vector_buffer,
                           4,
                           (float)RandomVal(2) + 1.0f,
                           seed_,
                           trim_color_);
    }
    else if((addon == ADDON_LIGHTS) && !have_lights_) {
        new Light(gl_vector3(left, (float)(bottom + 2), front), trim_color_, 2);
        new Light(gl_vector3(right, (float)(bottom + 2), front), trim_color_, 2);
        new Light(gl_vector3(right, (float)(bottom + 2), back), trim_color_, 2);
        new Light(gl_vector3(left, (float)(bottom + 2), back), trim_color_, 2);
        have_lights_ = true;
    }

    bottom += (float)height;

    // If the roof is big enough, consider making another layer
    if((width > 7) && (depth > 7) && (roof_tiers_ < max_tiers)) {
        ConstructRoof(left + 1, right - 1, front + 1, back - 1, bottom);
        return;
    }

    // 1 air conditioner block for every 15 floors sounds reasonable
    air_conditioners = height_ / 15;
    for(i = 0; i < air_conditioners; ++i) {
        ac_size = (float)(10 + RandomVal(30)) / 10;
        ac_height = ((float)RandomVal(20) / 10) + 1.0f;
        ac_x = left + (float)RandomVal(width);
        ac_y = front + (float)RandomVal(depth);

        // Make sure the unit doesn't hang off the right edge of the building
        if((ac_x + ac_size) > (float)right) {
            ac_x = (float)right - ac_size;
        }
        

        // Make sure the unit doesn't hang off the back edge of the building
        if((ac_y + ac_size) > (float)back) {
            ac_y = (float)back - ac_size;
        }

        ac_base = (float)bottom;

        // Make sure it doesn't hang off the edge
        ConstructCube(ac_x,
                      ac_x + ac_size,
                      ac_y,
                      ac_y + ac_size,
                      ac_base,
                      ac_base + ac_height);
    }

    if(height_ > 45) {
        d = new Decoration;
        d->CreateRadioTower(gl_vector3((float)(left + right) / 2.0f,
                                       (float)bottom,
                                       (float)(front + back) / 2.0f),
                            15.0f);
    }
}

void Building::ConstructSpike(int left, 
                              int right, 
                              int front,
                              int back,
                              int bottom,
                              int top)
{
    gl_vertex p;
    fan f;
    int i;
    gl_vector3 center;

    for(i = 0; i < 5; ++i) {
        f.index_list.push_back(mesh_flat_->VertexCount() + i);
    }

    f.index_list.push_back(f.index_list[1]);
    p.set_uv(gl_vector2(0.0f, 0.0f));
    center.set_x(((float)left + (float)right) / 2.0f);
    center.set_z(((float)front + (float)back) / 2.0f);
    p.set_position(gl_vector3(center.get_x(), (float)top, center.get_z()));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((float)left, (float)bottom, (float)back));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((float)right, (float)bottom, (float)back));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((float)right, (float)bottom, (float)front));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((float)left, (float)bottom, (float)front));
    mesh_flat_->VertexAdd(p);

    mesh_flat_->FanAdd(f);
}

/*
 * This build an outer fall of a building, with blank (windowless) areas
 * deliberately left. It creates a chain of segments that alternate
 * between windowed and windowless, and it always makes sure the wall
 * is symmetrical. window_groups tells it how many windows to place in a row.
 */
float Building::ConstructWall(int start_x,
                              int start_y,
                              int start_z,
                              int direction,
                              int length,
                              int height,
                              int window_groups,
                              float uv_start,
                              bool blank_corners)
{
    int x;
    int z;
    int step_x;
    int step_z;
    int i;
    quad_strip qs;
    int column;
    int mid;
    int odd;
    gl_vertex v;
    bool blank;
    bool last_blank;

    qs.index_list.reserve(100);

    switch(direction) {
    case NORTH:
        step_z = 1;
        step_x = 0;
        break;
    case WEST:
        step_z = 0;
        step_x = -1;
        break;
    case SOUTH:
        step_z = -1;
        step_x = 0;
        break;
    case EAST:
        step_z = 0;
        step_x = 1;
        break;
    }

    x = start_x;
    z = start_z;
    mid = (length / 2) - 1;
    odd = 1 - (length % 2);
    if(length % 2) {
        mid++;
    }
    // mid = (length / 2);
    v.get_uv().set_x((float)(x + z) / SEGMENTS_PER_TEXTURE);
    v.get_uv().set_x(uv_start);
    blank = false;
    for(i = 0; i <= length; ++i) {
        // Column counts up to the mid point, then back down, 
        // to make it symmetrical
        if(i <= mid) {
            column = i - odd;
        }
        else {
            column = mid - (i - mid);
        }
        last_blank = blank;
        blank = (column % window_groups) > (window_groups / 2);
        if(blank_corners && (i == 0)) {
            blank = true;
        }
        if(blank_corners && (i == (length - 1))) {
            blank = true;
        }
        if((last_blank != blank) || (i == 0) || (i == length)) {
            v.set_position(gl_vector3((float)x, (float)start_y, (float)z));
            v.get_uv().set_y((float)start_y / SEGMENTS_PER_TEXTURE);
            mesh_->VertexAdd(v);
            qs.index_list.push_back(mesh_->VertexCount() - 1);
            v.get_position().set_y((float)(start_y + height));
            v.get_uv().set_y((float)(start_y + height) / SEGMENTS_PER_TEXTURE);
            mesh_->VertexAdd(v);
            qs.index_list.push_back(mesh_->VertexCount() - 1);
        }

        //if(!blank && (i != 0) && (i != (length - 1))) {
        if(!blank && (i != length)) {
            v.get_uv().set_x(v.get_uv().get_x() + (1.0f / SEGMENTS_PER_TEXTURE));
        }

        x += step_x;
        z += step_z;
    }

    mesh_->QuadStripAdd(qs);

    return v.get_uv().get_x();
}

/*
 * This makes a big chunky building of intersecting cubes;
 */
void Building::CreateBlocky()
{
    int min_height;
    int left;
    int right;
    int front;
    int back;
    int max_left;
    int max_right;
    int max_front;
    int max_back;
    int height;
    int mid_x;
    int mid_z;
    int half_depth;
    int half_width;
    int tiers;
    int max_tiers;
    int grouping;
    float lid_height;
    float uv_start;
    bool skip;
    bool blank_corners;

    // Choose if the corners of the building are to be windowless
    blank_corners = COIN_FLIP;

    // Choose a random color on our texture
    uv_start = (float)RandomVal(SEGMENTS_PER_TEXTURE) / SEGMENTS_PER_TEXTURE;
    
    // Choose how the windows are grouped
    grouping = 2 + RandomVal(4);
    
    // Choose how tall the lid should be on top or each section
    lid_height = (float)(RandomVal(3) + 1);

    // Find the center of the building
    mid_x = x_ + (width_ / 2);
    mid_z = y_ + (depth_ / 2);
    max_back = 1;
    max_front = 1;
    max_right = 1;
    max_left = 1;
    height = height_;
    min_height = height_ / 2;
    min_height = 3;
    half_depth = depth_ / 2;
    half_width = width_ / 2;
    tiers = 0;

    if(height_ > 40) {
        max_tiers = 15;
    }
    else if(height_ > 30) {
        max_tiers = 10;
    }
    else if(height_ > 20) {
        max_tiers = 5;
    }
    else if(height_ > 10) {
        max_tiers = 2;
    }
    else {
        max_tiers = 1;
    }

    // We begin at the top of the building, and work our way down.
    // Viewed from above, the sections of the building are randomly sized
    // rectangles that ALWAYS include the center of the building somewhere
    // within their area.
    while(1) {
        if(height < min_height) {
            break;
        }
        
        if(tiers >= max_tiers) {
            break;
        }

        // Pick new locations for our four outer walls
        left = (RandomVal() % half_width) + 1;
        right = (RandomVal() % half_width) + 1;
        front = (RandomVal() % half_depth) + 1;
        back = (RandomVal() % half_depth) + 1;
        skip = false;

        // At least ONE of the walls must reach out beyond a previous maximum
        // Otherwise, this tier would be completely hidden within a previous
        // one.
        if((left <= max_left)
           && (right <= max_right) 
           && (front <= max_front)
           && (back <= max_back)) {
            skip = true;
        }

        // If any of the four walls is in the same position as the previous
        // max, then skip this tier, or else the two walls will end up
        // z-fighting.
        if((left == max_left)
           || (right == max_right)
           || (front == max_front)
           || (back == max_back)) {
            skip = true;
        }

        if(!skip) {
            // If this is the top, then push some lights up here
            max_left = MAX(left, max_left);
            max_right = MAX(right, max_right);
            max_front = MAX(front, max_front);
            max_back = MAX(back, max_back);

            // Now build the four walls of this part
            uv_start = ConstructWall(mid_x - left,
                                     0,
                                     mid_z + back,
                                     SOUTH,
                                     front + back,
                                     height,
                                     grouping,
                                     uv_start,
                                     blank_corners) - ONE_SEGMENT;
            uv_start = ConstructWall(mid_x - left,
                                     0,
                                     mid_z - front,
                                     EAST,
                                     right + left,
                                     height,
                                     grouping,
                                     uv_start,
                                     blank_corners) - ONE_SEGMENT;
            uv_start = ConstructWall(mid_x + right,
                                     0,
                                     mid_z - front,
                                     NORTH,
                                     front + back,
                                     height,
                                     grouping,
                                     uv_start,
                                     blank_corners) - ONE_SEGMENT;
            uv_start = ConstructWall(mid_x + right,
                                     0,
                                     mid_z + back,
                                     WEST,
                                     right + left,
                                     height,
                                     grouping,
                                     uv_start,
                                     blank_corners) - ONE_SEGMENT;

            if(!tiers) {
                ConstructRoof((float)(mid_x - left),
                              (float)(mid_x + right),
                              (float)(mid_z - front),
                              (float)(mid_z + back),
                              (float)height);
            }
            else {
                // Add a flat-color lid onto this section
                ConstructCube((float)(mid_x - left),
                              (float)(mid_x + right),
                              (float)(mid_z - front),
                              (float)(mid_z + back),
                              (float)height,
                              (float)height + lid_height);
            }

            height -= ((RandomVal() % 10) + 1);
            tiers++;
        }

        height--;
    }

    ConstructCube(mid_x - half_width,
                  mid_x + half_width,
                  mid_z - half_depth,
                  mid_z + half_depth,
                  0,
                  2);

    mesh_->Compile();
    mesh_flat_->Compile();
}

/*
 * A single-cube building. Good for low-rise buildings and stuff that will be
 * far from the camera
 */
void Building::CreateSimple()
{
    gl_vertex p;
    float x1;
    float x2;
    float z1;
    float z2;
    float y1;
    float y2;
    quad_strip qs;
    float u;
    float v1;
    float v2;
    float cap_height;
    float ledge;

    for(int i = 0; i <= 10; ++i) {
        qs.index_list.push_back(i);
    }

    // How tall the flat-color roof is
    cap_height = (float)(1 + RandomVal(4));

    // How much the ledge sticks out
    ledge = (float)RandomVal(10) / 30.0f;

    x1 = (float)x_;
    x2 = (float)(x_ + width_);
    y1 = (float)0.0f;
    y2 = (float)height_;
    z2 = (float)y_;
    z1 = (float)(y_ + depth_);

    u = (float)(RandomVal(SEGMENTS_PER_TEXTURE)) / SEGMENTS_PER_TEXTURE;
    v1 = (float)(RandomVal(SEGMENTS_PER_TEXTURE)) / SEGMENTS_PER_TEXTURE;
    v2 = v1 + ((float)height_ * ONE_SEGMENT);

    p.set_position(gl_vector3(x1, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x1, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    u += ((float)depth_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x1, y1, z2));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x1, y2, z2));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    u += ((float)width_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x2, y1, z2));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x2, y2, z2));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    u += ((float)depth_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x2, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x2, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);
    
    u += ((float)depth_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x1, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x1, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    mesh_->QuadStripAdd(qs);
    ConstructCube(x1 - ledge,
                  x2 + ledge,
                  z2 - ledge,
                  z1 + ledge,
                  (float)height_,
                  (float)height_ + cap_height);
    mesh_->Compile();
}

/*
 * This makes a deformed cylinder building.
 */
void Building::CreateModern()
{
    gl_vertex p;
    gl_vector3 center;
    gl_vector3 pos;
    gl_vector2 radius;
    gl_vector2 start;
    gl_vector2 end;
    int angle;
    int windows;
    int cap_height;
    int half_depth;
    int half_width;
    float length;
    quad_strip qs;
    fan f;
    int points;
    int skip_interval;
    int skip_counter;
    int skip_delta;
    int i;
    bool logo_done;
    bool do_trim;
    Decoration *d;
    
    logo_done = false;
    
    // How tall the windowless section on top will be.
    cap_height = 1 + RandomVal(5);

    // How many 10-degree segments to build before the next skip
    skip_interval = 1 + RandomVal(8);

    // When a skip happens, how many degree should be skipped
    skip_delta = (1 + RandomVal(2)) * 30; // 30, 60, or 90
    
    // See if this is eligible for fancy lighting trim on top
    if((height_ > 48) && (RandomVal(3) == 0)) {
        do_trim = true;
    }
    else {
        do_trim = false;
    }

    // Get the center and radius of the circle
    half_depth = depth_ / 2;
    half_width = width_ / 2;
    center = gl_vector3((float)(x_ + half_width), 0.0f, (float)(y_ + half_depth));
    radius = gl_vector2((float)half_width, (float)half_depth);
    windows = 0;
    p.get_uv().set_x(0.0f);
    points = 0;
    skip_counter = 0;
    for(angle = 0; angle <= 360; angle += 10) {
        if((skip_counter >= skip_interval) && ((angle + skip_delta) > 360)) {
            angle += skip_delta;
            skip_counter = 0;
        }

        pos.set_x(center.get_x() - sinf((float)angle * DEGREES_TO_RADIANS) * radius.get_x());
        pos.set_z(center.get_z() + cosf((float)angle * DEGREES_TO_RADIANS) * radius.get_y());
        
        if((angle > 0) && (skip_counter == 0)) {
            length = MathDistance(p.get_position().get_x(),
                                  p.get_position().get_z(),
                                  pos.get_x(),
                                  pos.get_z());

            windows += (int)length;
            if((length > 10) && !logo_done) {
                logo_done = true;
                start = gl_vector2(pos.get_x(), pos.get_z());
                end = gl_vector2(p.get_position().get_x(),
                                 p.get_position().get_z());

                d = new Decoration;
                gl_rgba random_color;
                d->CreateLogo(start, 
                              end,
                              (float)height_,
                              WorldLogoIndex(),
                              random_color.from_hsl((float)RandomVal(255) / 255,
                                                    1.0f,
                                                    1.0f));
            }
        }
        else if(skip_counter != 1) {
            windows++;
        }

        p.set_position(pos);
        p.get_uv().set_x((float)windows / (float)SEGMENTS_PER_TEXTURE);
        p.get_uv().set_y(0.0f);
        p.get_position().set_y(0.0f);
        mesh_->VertexAdd(p);

        p.get_position().set_y((float)height_);
        p.get_uv().set_y((float)height_ / (float)SEGMENTS_PER_TEXTURE);
        mesh_->VertexAdd(p);
        mesh_flat_->VertexAdd(p);

        p.get_position().set_y(p.get_position().get_y() + (float)cap_height);
        mesh_flat_->VertexAdd(p);

        vector_buffer[points / 2] = p.get_position();
        vector_buffer[points / 2].set_y((float)height_ + (cap_height / 4));
        points += 2;
        skip_counter++;
    }

    // If this is a big building and it didn't get a logo, consider
    // giving it a light strip
    if(!logo_done && do_trim) {
        d = new Decoration;
        gl_rgba random_color;
        d->CreateLightTrim(vector_buffer,
                           (points / 2) - 2,
                           (float)cap_height / 2,
                           seed_,
                           random_color.from_hsl((float)RandomVal(255) / 255,
                                                 1.0f,
                                                 1.0f));
    }

    qs.index_list.reserve(points);

    // Add the outer walls
    for(i = 0; i < points; ++i) {
        qs.index_list.push_back(i);
    }

    mesh_->QuadStripAdd(qs);
    mesh_flat_->QuadStripAdd(qs);

    // Add the fan to cap the top of the buildings
    f.index_list.push_back(points);
    for(i = 0; i < (points / 2); ++i) {
        f.index_list.push_back(points - (1 + (i * 2)));
    }
    p.get_position().set_x(center_.get_x());
    p.get_position().set_z(center_.get_z());

    mesh_flat_->VertexAdd(p);
    mesh_flat_->FanAdd(f);

    radius /= 2.0f;

    // ConstructRoof((int)(center_.x - radius),
    //               (int)(center_.x + radius),
    //               (int)(center_.z - radius),
    //               (int)(center_.z + radius),
    //               height_ + cap_height);

    mesh_->Compile();
    mesh_flat_->Compile();
}

void Building::CreateTower()
{
    int left;
    int right;
    int front;
    int back;
    int bottom;
    int section_height;
    int section_width;
    int section_depth;
    int remaining_height;
    int ledge_height;
    int tier_fraction;
    int grouping;
    int foundation;
    int narrowing_interval;
    int tiers;
    float ledge;
    float uv_start;
    bool blank_corners;

    // How much ledges protrude from the building
    ledge = (float)RandomVal(3) * 0.25f;
    
    // How tall the ledges are, in stories
    ledge_height = RandomVal(4) + 1;

    // If the corner of the building have no windows
    blank_corners = RandomVal(4) > 0;
    
    // How the windows are grouped
    grouping = RandomVal(3) + 2;

    // What fraction of the remaining height should be given to each tier
    tier_fraction = 2 + RandomVal(4);

    // How often (in tiers) does the building get narrower?
    narrowing_interval = 1 + RandomVal(10);

    // The height of the windowless slab at the bottom
    foundation = 2 + RandomVal(3);

    // Set our initial parameters
    left = x_;
    right = x_ + width_;
    front = y_;
    back = y_ + depth_;
    bottom = 0;
    tiers = 0;

    // Build the foundations
    ConstructCube((float)left - ledge,
                  (float)right + ledge,
                  (float)front - ledge,
                  (float)back + ledge,
                  (float)bottom,
                  (float)foundation);

    bottom += foundation;

    // Now add tiers until we reach the top
    while(1) {
        remaining_height = height_ - bottom;
        section_depth = back - front;
        section_width = right - left;
        section_height = MAX(remaining_height / tier_fraction, 2);
        if(remaining_height < 10) {
            section_height = remaining_height;
        }

        // Build the four walls
        uv_start = 
            (float)RandomVal(SEGMENTS_PER_TEXTURE) / SEGMENTS_PER_TEXTURE;

        uv_start = ConstructWall(left,
                                 bottom,
                                 back,
                                 SOUTH,
                                 section_depth,
                                 section_height,
                                 grouping,
                                 uv_start,
                                 blank_corners) - ONE_SEGMENT;
        uv_start = ConstructWall(left,
                                 bottom,
                                 front,
                                 EAST,
                                 section_width,
                                 section_height,
                                 grouping,
                                 uv_start,
                                 blank_corners) - ONE_SEGMENT;
        uv_start = ConstructWall(right,
                                 bottom,
                                 front,
                                 NORTH,
                                 section_depth,
                                 section_height,
                                 grouping,
                                 uv_start,
                                 blank_corners) - ONE_SEGMENT;
        uv_start = ConstructWall(right,
                                 bottom,
                                 back,
                                 WEST,
                                 section_width,
                                 section_height,
                                 grouping,
                                 uv_start,
                                 blank_corners) - ONE_SEGMENT;

        bottom += section_height;

        // Build the slab/ledges to cap this section.
        if((bottom + ledge_height) > height_) {
            break;
        }

        ConstructCube((float)left - ledge,
                      (float)right + ledge,
                      (float)front - ledge,
                      (float)back + ledge,
                      (float)bottom,
                      (float)(bottom + ledge_height));

        bottom += ledge_height;
        if(bottom > height_) {
            break;
        }
        
        tiers++;
        if((tiers % narrowing_interval) == 0) {
            if(section_width > 7) {
                left += 1;
                right -= 1;
            }
            if(section_depth > 7) {
                front += 1;
                back -= 1;
            }
        }
    }

    ConstructRoof((float)left, 
                  (float)right, 
                  (float)front,
                  (float)back,
                  (float)bottom);

    mesh_->Compile();
    mesh_flat_->Compile();
}
