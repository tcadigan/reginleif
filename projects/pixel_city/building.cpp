/*
 * Building.cpp
 *
 * 2009 Shamus Young
 *
 * This module contains the class to construct the buildings.
 *
 */

#include "building.hpp"

#include <cmath> // sinf, cosf

#include "decoration.hpp" // Decoration
#include "light.hpp" // Light
#include "macro.hpp"
#include "math.hpp"
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
Building::Building(GLint type,
                   GLint x,
                   GLint y,
                   GLint height,
                   GLint width,
                   GLint depth,
                   GLint seed,
                   gl_rgba color)
{
    x_ = x;
    y_ = y;
    width_ = width;
    height_ = height;
    center_ = 
        gl_vector3((GLfloat)(x_ + (width / 2)), 0.0f, (GLfloat)(y_ + (depth / 2)));
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
        create_simple();
        break;
    case BUILDING_MODERN:
        create_modern();
        break;
    case BUILDING_TOWER:
        create_tower();
        break;
    case BUILDING_BLOCKY:
        create_blocky();
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

GLuint Building::texture()
{
    return TextureRandomBuilding(texture_type_);
}

GLint Building::poly_count()
{
    return(mesh_->PolyCount() + mesh_flat_->PolyCount());
}

void Building::render()
{
    glColor3fv(color_.get_data());
    mesh_->Render();
}

void Building::render_flat(GLboolean colored)
{
    if(colored) {
        glColor3fv(color_.get_data());
    }

    mesh_flat_->Render();
}

void Building::construct_cube(GLint left,
                              GLint right,
                              GLint front,
                              GLint back,
                              GLint bottom,
                              GLint top)
{
    gl_vertex p[10];
    GLfloat x1;
    GLfloat x2;
    GLfloat z1;
    GLfloat z2;
    GLfloat y1;
    GLfloat y2;
    GLint i;
    cube c;
    GLfloat u;
    GLfloat v1;
    GLfloat v2;
    GLfloat mapping;
    GLint base_index;

    x1 = (GLfloat)left;
    x2 = (GLfloat)right;
    y1 = (GLfloat)bottom;
    y2 = (GLfloat)top;
    z1 = (GLfloat)front;
    z2 = (GLfloat)back;
    base_index = mesh_->VertexCount();

    mapping = (GLfloat)SEGMENTS_PER_TEXTURE;
    u = (GLfloat)(RandomVal() % SEGMENTS_PER_TEXTURE) / (GLfloat)mapping;
    v1 = (GLfloat)bottom / (GLfloat)mapping;
    v2 = (GLfloat)top / (GLfloat)mapping;

    p[0].set_position(gl_vector3(x1, y1, z1));
    p[0].set_uv(gl_vector2(u, v1));
    p[1].set_position(gl_vector3(x1, y2, z1));
    p[1].set_uv(gl_vector2(u, v2));

    u += (GLfloat)width_ / mapping;
    p[2].set_position(gl_vector3(x2, y1, z1));
    p[2].set_uv(gl_vector2(u, v1));
    p[3].set_position(gl_vector3(x2, y2, z1));
    p[3].set_uv(gl_vector2(u, v2));

    u += (GLfloat)depth_ / mapping;
    p[4].set_position(gl_vector3(x2, y1, z2));
    p[4].set_uv(gl_vector2(u, v1));
    p[5].set_position(gl_vector3(x2, y2, z2));
    p[5].set_uv(gl_vector2(u, v2));

    u += (GLfloat)width_ / mapping;
    p[6].set_position(gl_vector3(x1, y1, z2));
    p[6].set_uv(gl_vector2(u, v1));
    p[7].set_position(gl_vector3(x1, y2, z2));
    p[7].set_uv(gl_vector2(u, v2));

    u += (GLfloat)width_ / mapping;
    p[8].set_position(gl_vector3(x1, y1, z1));
    p[8].set_uv(gl_vector2(u, v1));
    p[9].set_position(gl_vector3(x1, y2, z1));
    p[9].set_uv(gl_vector2(u, v2));

    for(i = 0; i < 10; ++i) {
        GLfloat pos_x = p[i].get_position().get_x();
        GLfloat pos_z = p[i].get_position().get_z();
        p[i].get_uv().set_x((pos_x + pos_z) / (GLfloat)mapping);
        mesh_->VertexAdd(p[i]);
        c.index_list.push_back(base_index + i);
    }

    mesh_->CubeAdd(c);
}

void Building::construct_cube(GLfloat left,
                              GLfloat right,
                              GLfloat front,
                              GLfloat back,
                              GLfloat bottom,
                              GLfloat top)
{
    gl_vertex p[10];
    GLfloat x1;
    GLfloat x2;
    GLfloat z1;
    GLfloat z2;
    GLfloat y1;
    GLfloat y2;
    GLint i;
    cube c;
    GLint base_index;

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
        p[i].get_uv().set_x((pos_x + pos_z) / (GLfloat)SEGMENTS_PER_TEXTURE);

        mesh_flat_->VertexAdd(p[i]);
        c.index_list.push_back(base_index + i);
    }

    mesh_flat_->CubeAdd(c);
}

/*
 * This will take the given area and populate it with rooftop stuff like
 * air conditioners or light towers.
 */
void Building::construct_roof(GLfloat left,
                              GLfloat right,
                              GLfloat front,
                              GLfloat back,
                              GLfloat bottom)
{
    GLint air_conditioners;
    GLint i;
    GLint width;
    GLint depth;
    GLint height;
    GLint face;
    GLint addon;
    GLint max_tiers;
    GLfloat ac_x;
    GLfloat ac_y;
    GLfloat ac_base;
    GLfloat ac_size;
    GLfloat ac_height;
    GLfloat logo_offset;
    Decoration *d;
    gl_vector2 start;
    gl_vector2 end;

    roof_tiers_++;
    max_tiers = height_ / 10;
    width = (GLint)(right - left);
    depth = (GLint)(back - front);
    height = 5 - roof_tiers_;
    logo_offset = 0.2f;

    // See if this build is special and worth of fancy roof decorations.
    if(bottom > 35.0f) {
        addon = RandomVal(ADDON_COUNT);
    }

    // Build the roof slab
    construct_cube(left, right, front, back, bottom, bottom + (GLfloat)height);

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
            start = gl_vector2((GLfloat)left, (GLfloat)back + logo_offset);
            end = gl_vector2((GLfloat)right, (GLfloat)back + logo_offset);
            break;
        case SOUTH:
            start = gl_vector2((GLfloat) right, (GLfloat)front - logo_offset);
            end = gl_vector2((GLfloat)right, (GLfloat)front - logo_offset);
            break;
        case EAST:
            start = gl_vector2((GLfloat)right + logo_offset, (GLfloat)back);
            end = gl_vector2((GLfloat)right + logo_offset, (GLfloat)front);
            break;
        case WEST:
        default:
            start = gl_vector2((GLfloat)left - logo_offset, (GLfloat)front);
            end = gl_vector2((GLfloat)left - logo_offset, (GLfloat)back);
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
                           (GLfloat)RandomVal(2) + 1.0f,
                           seed_,
                           trim_color_);
    }
    else if((addon == ADDON_LIGHTS) && !have_lights_) {
        new Light(gl_vector3(left, (GLfloat)(bottom + 2), front), trim_color_, 2);
        new Light(gl_vector3(right, (GLfloat)(bottom + 2), front), trim_color_, 2);
        new Light(gl_vector3(right, (GLfloat)(bottom + 2), back), trim_color_, 2);
        new Light(gl_vector3(left, (GLfloat)(bottom + 2), back), trim_color_, 2);
        have_lights_ = true;
    }

    bottom += (GLfloat)height;

    // If the roof is big enough, consider making another layer
    if((width > 7) && (depth > 7) && (roof_tiers_ < max_tiers)) {
        construct_roof(left + 1, right - 1, front + 1, back - 1, bottom);
        return;
    }

    // 1 air conditioner block for every 15 floors sounds reasonable
    air_conditioners = height_ / 15;
    for(i = 0; i < air_conditioners; ++i) {
        ac_size = (GLfloat)(10 + RandomVal(30)) / 10;
        ac_height = ((GLfloat)RandomVal(20) / 10) + 1.0f;
        ac_x = left + (GLfloat)RandomVal(width);
        ac_y = front + (GLfloat)RandomVal(depth);

        // Make sure the unit doesn't hang off the right edge of the building
        if((ac_x + ac_size) > (GLfloat)right) {
            ac_x = (GLfloat)right - ac_size;
        }
        

        // Make sure the unit doesn't hang off the back edge of the building
        if((ac_y + ac_size) > (GLfloat)back) {
            ac_y = (GLfloat)back - ac_size;
        }

        ac_base = (GLfloat)bottom;

        // Make sure it doesn't hang off the edge
        construct_cube(ac_x,
                       ac_x + ac_size,
                       ac_y,
                       ac_y + ac_size,
                       ac_base,
                       ac_base + ac_height);
    }

    if(height_ > 45) {
        d = new Decoration;
        d->CreateRadioTower(gl_vector3((GLfloat)(left + right) / 2.0f,
                                       (GLfloat)bottom,
                                       (GLfloat)(front + back) / 2.0f),
                            15.0f);
    }
}

void Building::construct_spike(GLint left, 
                               GLint right, 
                               GLint front,
                               GLint back,
                               GLint bottom,
                               GLint top)
{
    gl_vertex p;
    fan f;
    GLint i;
    gl_vector3 center;

    for(i = 0; i < 5; ++i) {
        f.index_list.push_back(mesh_flat_->VertexCount() + i);
    }

    f.index_list.push_back(f.index_list[1]);
    p.set_uv(gl_vector2(0.0f, 0.0f));
    center.set_x(((GLfloat)left + (GLfloat)right) / 2.0f);
    center.set_z(((GLfloat)front + (GLfloat)back) / 2.0f);
    p.set_position(gl_vector3(center.get_x(), (GLfloat)top, center.get_z()));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((GLfloat)left, (GLfloat)bottom, (GLfloat)back));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((GLfloat)right, (GLfloat)bottom, (GLfloat)back));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((GLfloat)right, (GLfloat)bottom, (GLfloat)front));
    mesh_flat_->VertexAdd(p);

    p.set_position(gl_vector3((GLfloat)left, (GLfloat)bottom, (GLfloat)front));
    mesh_flat_->VertexAdd(p);

    mesh_flat_->FanAdd(f);
}

/*
 * This build an outer fall of a building, with blank (windowless) areas
 * deliberately left. It creates a chain of segments that alternate
 * between windowed and windowless, and it always makes sure the wall
 * is symmetrical. window_groups tells it how many windows to place in a row.
 */
GLfloat Building::construct_wall(GLint start_x,
                                 GLint start_y,
                                 GLint start_z,
                                 GLint direction,
                                 GLint length,
                                 GLint height,
                                 GLint window_groups,
                                 GLfloat uv_start,
                                 GLboolean blank_corners)
{
    GLint x;
    GLint z;
    GLint step_x;
    GLint step_z;
    GLint i;
    quad_strip qs;
    GLint column;
    GLint mid;
    GLint odd;
    gl_vertex v;
    GLboolean blank;
    GLboolean last_blank;

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
    v.get_uv().set_x((GLfloat)(x + z) / SEGMENTS_PER_TEXTURE);
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
            v.set_position(gl_vector3((GLfloat)x, (GLfloat)start_y, (GLfloat)z));
            v.get_uv().set_y((GLfloat)start_y / SEGMENTS_PER_TEXTURE);
            mesh_->VertexAdd(v);
            qs.index_list.push_back(mesh_->VertexCount() - 1);
            v.get_position().set_y((GLfloat)(start_y + height));
            v.get_uv().set_y((GLfloat)(start_y + height) / SEGMENTS_PER_TEXTURE);
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
void Building::create_blocky()
{
    GLint min_height;
    GLint left;
    GLint right;
    GLint front;
    GLint back;
    GLint max_left;
    GLint max_right;
    GLint max_front;
    GLint max_back;
    GLint height;
    GLint mid_x;
    GLint mid_z;
    GLint half_depth;
    GLint half_width;
    GLint tiers;
    GLint max_tiers;
    GLint grouping;
    GLfloat lid_height;
    GLfloat uv_start;
    GLboolean skip;
    GLboolean blank_corners;

    // Choose if the corners of the building are to be windowless
    blank_corners = COIN_FLIP;

    // Choose a random color on our texture
    uv_start = (GLfloat)RandomVal(SEGMENTS_PER_TEXTURE) / SEGMENTS_PER_TEXTURE;
    
    // Choose how the windows are grouped
    grouping = 2 + RandomVal(4);
    
    // Choose how tall the lid should be on top or each section
    lid_height = (GLfloat)(RandomVal(3) + 1);

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
            uv_start = construct_wall(mid_x - left,
                                      0,
                                      mid_z + back,
                                      SOUTH,
                                      front + back,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;
            uv_start = construct_wall(mid_x - left,
                                      0,
                                      mid_z - front,
                                      EAST,
                                      right + left,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;
            uv_start = construct_wall(mid_x + right,
                                      0,
                                      mid_z - front,
                                      NORTH,
                                      front + back,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;
            uv_start = construct_wall(mid_x + right,
                                      0,
                                      mid_z + back,
                                      WEST,
                                      right + left,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;

            if(!tiers) {
                construct_roof((GLfloat)(mid_x - left),
                               (GLfloat)(mid_x + right),
                               (GLfloat)(mid_z - front),
                               (GLfloat)(mid_z + back),
                               (GLfloat)height);
            }
            else {
                // Add a flat-color lid onto this section
                construct_cube((GLfloat)(mid_x - left),
                               (GLfloat)(mid_x + right),
                               (GLfloat)(mid_z - front),
                               (GLfloat)(mid_z + back),
                               (GLfloat)height,
                               (GLfloat)height + lid_height);
            }

            height -= ((RandomVal() % 10) + 1);
            tiers++;
        }

        height--;
    }

    construct_cube(mid_x - half_width,
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
void Building::create_simple()
{
    gl_vertex p;
    GLfloat x1;
    GLfloat x2;
    GLfloat z1;
    GLfloat z2;
    GLfloat y1;
    GLfloat y2;
    quad_strip qs;
    GLfloat u;
    GLfloat v1;
    GLfloat v2;
    GLfloat cap_height;
    GLfloat ledge;

    for(GLint i = 0; i <= 10; ++i) {
        qs.index_list.push_back(i);
    }

    // How tall the flat-color roof is
    cap_height = (GLfloat)(1 + RandomVal(4));

    // How much the ledge sticks out
    ledge = (GLfloat)RandomVal(10) / 30.0f;

    x1 = (GLfloat)x_;
    x2 = (GLfloat)(x_ + width_);
    y1 = (GLfloat)0.0f;
    y2 = (GLfloat)height_;
    z2 = (GLfloat)y_;
    z1 = (GLfloat)(y_ + depth_);

    u = (GLfloat)(RandomVal(SEGMENTS_PER_TEXTURE)) / SEGMENTS_PER_TEXTURE;
    v1 = (GLfloat)(RandomVal(SEGMENTS_PER_TEXTURE)) / SEGMENTS_PER_TEXTURE;
    v2 = v1 + ((GLfloat)height_ * ONE_SEGMENT);

    p.set_position(gl_vector3(x1, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x1, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    u += ((GLfloat)depth_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x1, y1, z2));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x1, y2, z2));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    u += ((GLfloat)width_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x2, y1, z2));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x2, y2, z2));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    u += ((GLfloat)depth_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x2, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x2, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);
    
    u += ((GLfloat)depth_ / SEGMENTS_PER_TEXTURE);
    p.set_position(gl_vector3(x1, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->VertexAdd(p);
    p.set_position(gl_vector3(x1, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->VertexAdd(p);

    mesh_->QuadStripAdd(qs);
    construct_cube(x1 - ledge,
                   x2 + ledge,
                   z2 - ledge,
                   z1 + ledge,
                   (GLfloat)height_,
                   (GLfloat)height_ + cap_height);
    mesh_->Compile();
}

/*
 * This makes a deformed cylinder building.
 */
void Building::create_modern()
{
    gl_vertex p;
    gl_vector3 center;
    gl_vector3 pos;
    gl_vector2 radius;
    gl_vector2 start;
    gl_vector2 end;
    GLint angle;
    GLint windows;
    GLint cap_height;
    GLint half_depth;
    GLint half_width;
    GLfloat length;
    quad_strip qs;
    fan f;
    GLint points;
    GLint skip_interval;
    GLint skip_counter;
    GLint skip_delta;
    GLint i;
    GLboolean logo_done;
    GLboolean do_trim;
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
    center = gl_vector3((GLfloat)(x_ + half_width), 0.0f, (GLfloat)(y_ + half_depth));
    radius = gl_vector2((GLfloat)half_width, (GLfloat)half_depth);
    windows = 0;
    p.get_uv().set_x(0.0f);
    points = 0;
    skip_counter = 0;
    for(angle = 0; angle <= 360; angle += 10) {
        if((skip_counter >= skip_interval) && ((angle + skip_delta) > 360)) {
            angle += skip_delta;
            skip_counter = 0;
        }

        pos.set_x(center.get_x() - sinf((GLfloat)angle * DEGREES_TO_RADIANS) * radius.get_x());
        pos.set_z(center.get_z() + cosf((GLfloat)angle * DEGREES_TO_RADIANS) * radius.get_y());
        
        if((angle > 0) && (skip_counter == 0)) {
            length = MathDistance(p.get_position().get_x(),
                                  p.get_position().get_z(),
                                  pos.get_x(),
                                  pos.get_z());

            windows += (GLint)length;
            if((length > 10) && !logo_done) {
                logo_done = true;
                start = gl_vector2(pos.get_x(), pos.get_z());
                end = gl_vector2(p.get_position().get_x(),
                                 p.get_position().get_z());

                d = new Decoration;
                gl_rgba random_color;
                d->CreateLogo(start, 
                              end,
                              (GLfloat)height_,
                              WorldLogoIndex(),
                              random_color.from_hsl((GLfloat)RandomVal(255) / 255,
                                                    1.0f,
                                                    1.0f));
            }
        }
        else if(skip_counter != 1) {
            windows++;
        }

        p.set_position(pos);
        p.get_uv().set_x((GLfloat)windows / (GLfloat)SEGMENTS_PER_TEXTURE);
        p.get_uv().set_y(0.0f);
        p.get_position().set_y(0.0f);
        mesh_->VertexAdd(p);

        p.get_position().set_y((GLfloat)height_);
        p.get_uv().set_y((GLfloat)height_ / (GLfloat)SEGMENTS_PER_TEXTURE);
        mesh_->VertexAdd(p);
        mesh_flat_->VertexAdd(p);

        p.get_position().set_y(p.get_position().get_y() + (GLfloat)cap_height);
        mesh_flat_->VertexAdd(p);

        vector_buffer[points / 2] = p.get_position();
        vector_buffer[points / 2].set_y((GLfloat)height_ + (cap_height / 4));
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
                           (GLfloat)cap_height / 2,
                           seed_,
                           random_color.from_hsl((GLfloat)RandomVal(255) / 255,
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

    // ConstructRoof((GLint)(center_.x - radius),
    //               (GLint)(center_.x + radius),
    //               (GLint)(center_.z - radius),
    //               (GLint)(center_.z + radius),
    //               height_ + cap_height);

    mesh_->Compile();
    mesh_flat_->Compile();
}

void Building::create_tower()
{
    GLint left;
    GLint right;
    GLint front;
    GLint back;
    GLint bottom;
    GLint section_height;
    GLint section_width;
    GLint section_depth;
    GLint remaining_height;
    GLint ledge_height;
    GLint tier_fraction;
    GLint grouping;
    GLint foundation;
    GLint narrowing_interval;
    GLint tiers;
    GLfloat ledge;
    GLfloat uv_start;
    GLboolean blank_corners;

    // How much ledges protrude from the building
    ledge = (GLfloat)RandomVal(3) * 0.25f;
    
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
    construct_cube((GLfloat)left - ledge,
                   (GLfloat)right + ledge,
                   (GLfloat)front - ledge,
                   (GLfloat)back + ledge,
                   (GLfloat)bottom,
                   (GLfloat)foundation);

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
            (GLfloat)RandomVal(SEGMENTS_PER_TEXTURE) / SEGMENTS_PER_TEXTURE;

        uv_start = construct_wall(left,
                                  bottom,
                                  back,
                                  SOUTH,
                                  section_depth,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;
        uv_start = construct_wall(left,
                                  bottom,
                                  front,
                                  EAST,
                                  section_width,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;
        uv_start = construct_wall(right,
                                  bottom,
                                  front,
                                  NORTH,
                                  section_depth,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;
        uv_start = construct_wall(right,
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

        construct_cube((GLfloat)left - ledge,
                       (GLfloat)right + ledge,
                       (GLfloat)front - ledge,
                       (GLfloat)back + ledge,
                       (GLfloat)bottom,
                       (GLfloat)(bottom + ledge_height));

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

    construct_roof((GLfloat)left, 
                   (GLfloat)right, 
                   (GLfloat)front,
                   (GLfloat)back,
                   (GLfloat)bottom);

    mesh_->Compile();
    mesh_flat_->Compile();
}
