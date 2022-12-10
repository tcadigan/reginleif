/*
 * Building.cpp
 *
 * 2009 Shamus Young
 *
 * This module contains the class to construct the buildings.
 *
 */

#include "building.hpp"

#include <array>
#include <cmath> // sinf, cosf

#include "decoration.hpp" // Decoration
#include "light.hpp" // Light
#include "math.hpp"
#include "random.hpp"
#include "texture.hpp"
#include "win.hpp"
#include "world.hpp"

// This is used by the recursive roof builder to decide what items may be added.
enum class addon_t {
    none = 0,
    logo,
    trim,
    lights,
    count
};

static std::array<gl_vector3, MAX_VBUFFER> vector_buffer;

/*
 * This is the constructor for out building constructor.
 */
Building::Building(building_t type,
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
        gl_vector3((x_ + (width / 2.0f)), 0.0f, (y_ + (depth / 2.0f)));
    seed_ = seed;
    texture_type_ = random_val();
    color_ = color;
    color_.set_alpha(25);
    have_lights_ = false;
    have_logo_ = false;
    have_trim_ = false;
    roof_tiers_ = 0;
    trim_color_ = world_light_color(seed); // Pick a color for logos & roof lights
    mesh_ = std::make_unique<Mesh>(); // The main textured mesh for the building
    mesh_flat_ = std::make_unique<Mesh>(); // Flat-color mesh for untextured detail items

    switch (type) {
    case building_t::simple:
        create_simple();
        break;
    case building_t::modern:
        create_modern();
        break;
    case building_t::tower:
        create_tower();
        break;
    case building_t::blocky:
        create_blocky();
        break;
    }
}

Building::~Building()
{
}

unsigned int Building::texture() const
{
    return texture_random_building(texture_type_);
}

int Building::poly_count() const
{
    return(mesh_->poly_count() + mesh_flat_->poly_count());
}

void Building::render() const
{
    glColor3fv(color_.get_rgb().data());
    mesh_->render();
}

void Building::render_flat(bool colored) const
{
    if (colored) {
        glColor3fv(color_.get_rgb().data());
    }

    mesh_flat_->render();
}

void Building::construct_cube(int left,
                              int right,
                              int front,
                              int back,
                              int bottom,
                              int top)
{
    std::array<gl_vertex, 10> p;
    Cube cube;

    float x1 = left;
    float x2 = right;
    float y1 = bottom;
    float y2 = top;
    float z1 = front;
    float z2 = back;
    int base_index = mesh_->vertex_count();

    float mapping = SEGMENTS_PER_TEXTURE;
    float u = (random_val() % SEGMENTS_PER_TEXTURE) / mapping;
    float v1 = bottom / mapping;
    float v2 = top / mapping;

    p.at(0).set_position(gl_vector3(x1, y1, z1));
    p.at(0).set_uv(gl_vector2(u, v1));
    p.at(1).set_position(gl_vector3(x1, y2, z1));
    p.at(1).set_uv(gl_vector2(u, v2));

    u += width_ / mapping;
    p.at(2).set_position(gl_vector3(x2, y1, z1));
    p.at(2).set_uv(gl_vector2(u, v1));
    p.at(3).set_position(gl_vector3(x2, y2, z1));
    p.at(3).set_uv(gl_vector2(u, v2));

    u += depth_ / mapping;
    p.at(4).set_position(gl_vector3(x2, y1, z2));
    p.at(4).set_uv(gl_vector2(u, v1));
    p.at(5).set_position(gl_vector3(x2, y2, z2));
    p.at(5).set_uv(gl_vector2(u, v2));

    u += width_ / mapping;
    p.at(6).set_position(gl_vector3(x1, y1, z2));
    p.at(6).set_uv(gl_vector2(u, v1));
    p.at(7).set_position(gl_vector3(x1, y2, z2));
    p.at(7).set_uv(gl_vector2(u, v2));

    u += width_ / mapping;
    p.at(8).set_position(gl_vector3(x1, y1, z1));
    p.at(8).set_uv(gl_vector2(u, v1));
    p.at(9).set_position(gl_vector3(x1, y2, z1));
    p.at(9).set_uv(gl_vector2(u, v2));

    for (int i = 0; i < p.size(); ++i) {
        float pos_x = p.at(i).get_position().get_x();
        float pos_z = p.at(i).get_position().get_z();
        p.at(i).get_uv().set_x((pos_x + pos_z) / mapping);
        mesh_->vertex_add(p.at(i));
        cube.index_list.push_back(base_index + i);
    }

    mesh_->cube_add(cube);
}

void Building::construct_cube_float(float left,
                                    float right,
                                    float front,
                                    float back,
                                    float bottom,
                                    float top)
{
    std::array<gl_vertex, 10> p;
    Cube cube;

    float x1 = left;
    float x2 = right;
    float y1 = bottom;
    float y2 = top;
    float z1 = front;
    float z2 = back;
    int base_index = mesh_flat_->vertex_count();

    p.at(0).set_position(gl_vector3(x1, y1, z1));
    p.at(0).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(1).set_position(gl_vector3(x1, y2, z1));
    p.at(1).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(2).set_position(gl_vector3(x2, y1, z1));
    p.at(2).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(3).set_position(gl_vector3(x2, y2, z1));
    p.at(3).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(4).set_position(gl_vector3(x2, y1, z2));
    p.at(4).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(5).set_position(gl_vector3(x2, y2, z2));
    p.at(5).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(6).set_position(gl_vector3(x1, y1, z2));
    p.at(6).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(7).set_position(gl_vector3(x1, y2, z2));
    p.at(7).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(8).set_position(gl_vector3(x1, y1, z1));
    p.at(8).set_uv(gl_vector2(0.0f, 0.0f));

    p.at(9).set_position(gl_vector3(x1, y2, z1));
    p.at(9).set_uv(gl_vector2(0.0f, 0.0f));

    for (int i = 0; i < 10; ++i) {
        float pos_x = p.at(i).get_position().get_x();
        float pos_z = p.at(i).get_position().get_z();
        p.at(i).get_uv().set_x((pos_x + pos_z) / SEGMENTS_PER_TEXTURE);

        mesh_flat_->vertex_add(p.at(i));
        cube.index_list.push_back(base_index + i);
    }

    mesh_flat_->cube_add(cube);
}

/*
 * This will take the given area and populate it with rooftop stuff like
 * air conditioners or light towers.
 */
void Building::construct_roof(float left,
                              float right,
                              float front,
                              float back,
                              float bottom)
{
    addon_t addon = addon_t::none;

    ++roof_tiers_;
    int max_tiers = height_ / 10;
    int width = std::floor(right - left);
    int depth = std::floor(back - front);
    int height = 5 - roof_tiers_;
    float logo_offset = 0.2f;

    // See if this build is special and worth of fancy roof decorations.
    if (bottom > 35.0f) {
        switch (random_val(static_cast<int>(addon_t::count))) {
        case 0:
            addon = addon_t::none;
            break;
        case 1:
            addon = addon_t::logo;
            break;
        case 2:
            addon = addon_t::trim;
            break;
        case 3:
            addon = addon_t::lights;
            break;
        }
    }

    // Build the roof slab
    construct_cube(left, right, front, back, bottom, bottom + height);

    // Consider putting a logo on the root, if it's tall enough
    if ((addon == addon_t::logo) && !have_logo_) {
        Decoration *dec = new Decoration;
        direction_t face;

        if (width > depth) {
            if (random_val(2) == 0) {
                face = direction_t::north;
            } else {
                face = direction_t::south;
            }
        } else if (random_val(2) == 0) {
            face = direction_t::east;
        }
        else {
            face = direction_t::west;
        }

        gl_vector2 start;
        gl_vector2 end;

        switch (face) {
        case direction_t::north:
            start = gl_vector2(left, back + logo_offset);
            end = gl_vector2(right, back + logo_offset);
            break;
        case direction_t::south:
            start = gl_vector2(right, front - logo_offset);
            end = gl_vector2(right, front - logo_offset);
            break;
        case direction_t::east:
            start = gl_vector2(right + logo_offset, back);
            end = gl_vector2(right + logo_offset, front);
            break;
        case direction_t::west:
        default:
            start = gl_vector2(left - logo_offset, front);
            end = gl_vector2(left - logo_offset, back);
            break;
        }

        dec->create_logo(start, end, bottom, world_logo_index(), trim_color_);
        have_logo_ = true;
    } else if (addon == addon_t::trim) {
        Decoration *dec = new Decoration;

        vector_buffer.at(0) = gl_vector3(left, bottom, back);
        vector_buffer.at(1) = gl_vector3(left, bottom, front);
        vector_buffer.at(2) = gl_vector3(right, bottom, front);
        vector_buffer.at(3) = gl_vector3(right, bottom, back);

        dec->create_light_trim(vector_buffer,
                             4,
                             random_val(2) + 1.0f,
                             seed_,
                             trim_color_);
    } else if ((addon == addon_t::lights) && !have_lights_) {
        new Light(gl_vector3(left, (bottom + 2), front), trim_color_, 2);
        new Light(gl_vector3(right, (bottom + 2), front), trim_color_, 2);
        new Light(gl_vector3(right, (bottom + 2), back), trim_color_, 2);
        new Light(gl_vector3(left, (bottom + 2), back), trim_color_, 2);
        have_lights_ = true;
    }

    bottom += height;

    // If the roof is big enough, consider making another layer
    if ((width > 7) && (depth > 7) && (roof_tiers_ < max_tiers)) {
        construct_roof(left + 1, right - 1, front + 1, back - 1, bottom);
        return;
    }

    // 1 air conditioner block for every 15 floors sounds reasonable
    int air_conditioners = height_ / 15;
    for (int i = 0; i < air_conditioners; ++i) {
        float ac_size = (10 + random_val(30)) / 10.0f;
        float ac_height = (10 + random_val(20)) / 10.0f;
        float ac_x = left + random_val(width);
        float ac_y = front + random_val(depth);

        // Make sure the unit doesn't hang off the right edge of the building
        if ((ac_x + ac_size) > right) {
            ac_x = right - ac_size;
        }


        // Make sure the unit doesn't hang off the back edge of the building
        if ((ac_y + ac_size) > back) {
            ac_y = back - ac_size;
        }

        float ac_base = bottom;

        // Make sure it doesn't hang off the edge
        construct_cube(ac_x,
                       ac_x + ac_size,
                       ac_y,
                       ac_y + ac_size,
                       ac_base,
                       ac_base + ac_height);
    }

    if (height_ > 45) {
        Decoration *dec = new Decoration;
        dec->create_radio_tower(gl_vector3((left + right) / 2.0f,
                                         bottom,
                                         (front + back) / 2.0f),
                              15.0f);
    }
}

void Building::construct_spike(int left,
                               int right,
                               int front,
                               int back,
                               int bottom,
                               int top)
{
    gl_vertex p;
    Fan fan;
    gl_vector3 center;

    for (int i = 0; i < 5; ++i) {
        fan.index_list.push_back(mesh_flat_->vertex_count() + i);
    }

    fan.index_list.push_back(fan.index_list[1]);
    p.set_uv(gl_vector2(0.0f, 0.0f));
    center.set_x((left + right) / 2.0f);
    center.set_z((front + back) / 2.0f);
    p.set_position(gl_vector3(center.get_x(), top, center.get_z()));
    mesh_flat_->vertex_add(p);

    p.set_position(gl_vector3(left, bottom, back));
    mesh_flat_->vertex_add(p);

    p.set_position(gl_vector3(right, bottom, back));
    mesh_flat_->vertex_add(p);

    p.set_position(gl_vector3(right, bottom, front));
    mesh_flat_->vertex_add(p);

    p.set_position(gl_vector3(left, bottom, front));
    mesh_flat_->vertex_add(p);

    mesh_flat_->fan_add(fan);
}

/*
 * This build an outer fall of a building, with blank (windowless) areas
 * deliberately left. It creates a chain of segments that alternate
 * between windowed and windowless, and it always makes sure the wall
 * is symmetrical. window_groups tells it how many windows to place in a row.
 */
float Building::construct_wall(int start_x,
                               int start_y,
                               int start_z,
                               direction_t direction,
                               int length,
                               int height,
                               int window_groups,
                               float uv_start,
                               bool blank_corners)
{
    int step_x;
    int step_z;
    QuadStrip qs;
    int column;
    gl_vertex v;
    bool last_blank;

    qs.index_list.reserve(100);

    switch (direction) {
    case direction_t::north:
        step_z = 1;
        step_x = 0;
        break;
    case direction_t::west:
        step_z = 0;
        step_x = -1;
        break;
    case direction_t::south:
        step_z = -1;
        step_x = 0;
        break;
    case direction_t::east:
        step_z = 0;
        step_x = 1;
        break;
    }

    int x = start_x;
    int z = start_z;
    int mid = (length / 2) - 1;
    int odd = 1 - (length % 2);
    if (length % 2) {
        mid++;
    }
    // mid = (length / 2);
    v.get_uv().set_x((x + z) / static_cast<float>(SEGMENTS_PER_TEXTURE));
    v.get_uv().set_x(uv_start);
    bool blank = false;
    for (int i = 0; i <= length; ++i) {
        // Column counts up to the mid point, then back down,
        // to make it symmetrical
        if (i <= mid) {
            column = i - odd;
        } else {
            column = mid - (i - mid);
        }
        last_blank = blank;
        blank = (column % window_groups) > (window_groups / 2);
        if (blank_corners && (i == 0)) {
            blank = true;
        }
        if (blank_corners && (i == (length - 1))) {
            blank = true;
        }
        if ((last_blank != blank) || (i == 0) || (i == length)) {
            v.set_position(gl_vector3(x, start_y, z));
            v.get_uv().set_y(start_y / static_cast<float>(SEGMENTS_PER_TEXTURE));
            mesh_->vertex_add(v);
            qs.index_list.push_back(mesh_->vertex_count() - 1);
            v.get_position().set_y((start_y + height));
            v.get_uv().set_y((start_y + height) / static_cast<float>(SEGMENTS_PER_TEXTURE));
            mesh_->vertex_add(v);
            qs.index_list.push_back(mesh_->vertex_count() - 1);
        }

        //if (!blank && (i != 0) && (i != (length - 1))) {
        if (!blank && (i != length)) {
            v.get_uv().set_x(v.get_uv().get_x() + (1.0f / SEGMENTS_PER_TEXTURE));
        }

        x += step_x;
        z += step_z;
    }

    mesh_->quad_strip_add(qs);

    return v.get_uv().get_x();
}

/*
 * This makes a big chunky building of intersecting cubes;
 */
void Building::create_blocky()
{
    int max_tiers;

    // Choose if the corners of the building are to be windowless
    bool blank_corners = random_val(2) == 0;

    // Choose a random color on our texture
    float uv_start = random_val(SEGMENTS_PER_TEXTURE) / static_cast<float>(SEGMENTS_PER_TEXTURE);

    // Choose how the windows are grouped
    int grouping = 2 + random_val(4);

    // Choose how tall the lid should be on top or each section
    float lid_height = (random_val(3) + 1);

    // Find the center of the building
    int mid_x = x_ + (width_ / 2);
    int mid_z = y_ + (depth_ / 2);
    int max_back = 1;
    int max_front = 1;
    int max_right = 1;
    int max_left = 1;
    int height = height_;
    int min_height = height_ / 2;
    min_height = 3;
    int half_depth = depth_ / 2;
    int half_width = width_ / 2;
    int tiers = 0;

    if (height_ > 40) {
        max_tiers = 15;
    } else if (height_ > 30) {
        max_tiers = 10;
    } else if (height_ > 20) {
        max_tiers = 5;
    } else if (height_ > 10) {
        max_tiers = 2;
    } else {
        max_tiers = 1;
    }

    // We begin at the top of the building, and work our way down.
    // Viewed from above, the sections of the building are randomly sized
    // rectangles that ALWAYS include the center of the building somewhere
    // within their area.
    while (true) {
        if (height < min_height) {
            break;
        }

        if (tiers >= max_tiers) {
            break;
        }

        // Pick new locations for our four outer walls
        int left = (random_val() % half_width) + 1;
        int right = (random_val() % half_width) + 1;
        int front = (random_val() % half_depth) + 1;
        int back = (random_val() % half_depth) + 1;
        bool skip = false;

        // At least ONE of the walls must reach out beyond a previous maximum
        // Otherwise, this tier would be completely hidden within a previous
        // one.
        if ((left <= max_left)
           && (right <= max_right)
           && (front <= max_front)
           && (back <= max_back)) {
            skip = true;
        }

        // If any of the four walls is in the same position as the previous
        // max, then skip this tier, or else the two walls will end up
        // z-fighting.
        if ((left == max_left)
           || (right == max_right)
           || (front == max_front)
           || (back == max_back)) {
            skip = true;
        }

        if (!skip) {
            // If this is the top, then push some lights up here
            max_left = std::max(left, max_left);
            max_right = std::max(right, max_right);
            max_front = std::max(front, max_front);
            max_back = std::max(back, max_back);

            // Now build the four walls of this part
            uv_start = construct_wall(mid_x - left,
                                      0,
                                      mid_z + back,
                                      direction_t::south,
                                      front + back,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;
            uv_start = construct_wall(mid_x - left,
                                      0,
                                      mid_z - front,
                                      direction_t::east,
                                      right + left,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;
            uv_start = construct_wall(mid_x + right,
                                      0,
                                      mid_z - front,
                                      direction_t::north,
                                      front + back,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;
            uv_start = construct_wall(mid_x + right,
                                      0,
                                      mid_z + back,
                                      direction_t::west,
                                      right + left,
                                      height,
                                      grouping,
                                      uv_start,
                                      blank_corners) - ONE_SEGMENT;

            if (!tiers) {
                construct_roof((mid_x - left),
                               (mid_x + right),
                               (mid_z - front),
                               (mid_z + back),
                               height);
            } else {
                // Add a flat-color lid onto this section
                construct_cube_float((mid_x - left),
                                     (mid_x + right),
                                     (mid_z - front),
                                     (mid_z + back),
                                     height,
                                     height + lid_height);
            }

            height -= ((random_val() % 10) + 1);
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

    mesh_->compile();
    mesh_flat_->compile();
}

/*
 * A single-cube building. Good for low-rise buildings and stuff that will be
 * far from the camera
 */
void Building::create_simple()
{
    gl_vertex p;
    QuadStrip qs;

    for (int i = 0; i <= 10; ++i) {
        qs.index_list.push_back(i);
    }

    // How tall the flat-color roof is
    float cap_height = (1 + random_val(4));

    // How much the ledge sticks out
    float ledge = random_val(10) / 30.0f;

    float x1 = x_;
    float x2 = (x_ + width_);
    float y1 = 0.0f;
    float y2 = height_;
    float z2 = y_;
    float z1 = (y_ + depth_);

    float u = (random_val(SEGMENTS_PER_TEXTURE)) / static_cast<float>(SEGMENTS_PER_TEXTURE);
    float v1 = (random_val(SEGMENTS_PER_TEXTURE)) / static_cast<float>(SEGMENTS_PER_TEXTURE);
    float v2 = v1 + (height_ * ONE_SEGMENT);

    p.set_position(gl_vector3(x1, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->vertex_add(p);
    p.set_position(gl_vector3(x1, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->vertex_add(p);

    u += (depth_ / static_cast<float>(SEGMENTS_PER_TEXTURE));
    p.set_position(gl_vector3(x1, y1, z2));
    p.set_uv(gl_vector2(u, v1));
    mesh_->vertex_add(p);
    p.set_position(gl_vector3(x1, y2, z2));
    p.set_uv(gl_vector2(u, v2));
    mesh_->vertex_add(p);

    u += (width_ / static_cast<float>(SEGMENTS_PER_TEXTURE));
    p.set_position(gl_vector3(x2, y1, z2));
    p.set_uv(gl_vector2(u, v1));
    mesh_->vertex_add(p);
    p.set_position(gl_vector3(x2, y2, z2));
    p.set_uv(gl_vector2(u, v2));
    mesh_->vertex_add(p);

    u += (depth_ / static_cast<float>(SEGMENTS_PER_TEXTURE));
    p.set_position(gl_vector3(x2, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->vertex_add(p);
    p.set_position(gl_vector3(x2, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->vertex_add(p);

    u += (depth_ / static_cast<float>(SEGMENTS_PER_TEXTURE));
    p.set_position(gl_vector3(x1, y1, z1));
    p.set_uv(gl_vector2(u, v1));
    mesh_->vertex_add(p);
    p.set_position(gl_vector3(x1, y2, z1));
    p.set_uv(gl_vector2(u, v2));
    mesh_->vertex_add(p);

    mesh_->quad_strip_add(qs);
    construct_cube_float(x1 - ledge,
                         x2 + ledge,
                         z2 - ledge,
                         z1 + ledge,
                         height_,
                         height_ + cap_height);
    mesh_->compile();
}

/*
 * This makes a deformed cylinder building.
 */
void Building::create_modern()
{
    gl_vertex p;
    gl_vector3 pos;
    gl_vector2 start;
    gl_vector2 end;
    float length;
    QuadStrip qs;
    Fan fan;
    bool do_trim;
    Decoration *d;

    bool logo_done = false;

    // How tall the windowless section on top will be.
    int cap_height = 1 + random_val(5);

    // How many 10-degree segments to build before the next skip
    int skip_interval = 1 + random_val(8);

    // When a skip happens, how many degree should be skipped
    int skip_delta = (1 + random_val(2)) * 30; // 30, 60, or 90

    // See if this is eligible for fancy lighting trim on top
    if ((height_ > 48) && (random_val(3) == 0)) {
        do_trim = true;
    } else {
        do_trim = false;
    }

    // Get the center and radius of the circle
    int half_depth = depth_ / 2;
    int half_width = width_ / 2;
    gl_vector3 center = gl_vector3((x_ + half_width), 0.0f, (y_ + half_depth));
    gl_vector2 radius = gl_vector2(half_width, half_depth);
    int windows = 0;
    p.get_uv().set_x(0.0f);
    int points = 0;
    int skip_counter = 0;
    for (int angle = 0; angle <= 360; angle += 10) {
        if ((skip_counter >= skip_interval) && ((angle + skip_delta) > 360)) {
            angle += skip_delta;
            skip_counter = 0;
        }

        pos.set_x(center.get_x() - sinf(angle * DEGREES_TO_RADIANS) * radius.get_x());
        pos.set_z(center.get_z() + cosf(angle * DEGREES_TO_RADIANS) * radius.get_y());

        if ((angle > 0) && (skip_counter == 0)) {
            length = math_distance(p.get_position().get_x(),
                                  p.get_position().get_z(),
                                  pos.get_x(),
                                  pos.get_z());

            windows += (int)length;
            if ((length > 10) && !logo_done) {
                logo_done = true;
                start = gl_vector2(pos.get_x(), pos.get_z());
                end = gl_vector2(p.get_position().get_x(),
                                 p.get_position().get_z());

                d = new Decoration;
                gl_rgba random_color;
                d->create_logo(start,
                              end,
                              height_,
                              world_logo_index(),
                              from_hsl(random_val(255) / 255.0f,
                                       1.0f,
                                       1.0f));
            }
        } else if (skip_counter != 1) {
            windows++;
        }

        p.set_position(pos);
        p.get_uv().set_x(windows / static_cast<float>(SEGMENTS_PER_TEXTURE));
        p.get_uv().set_y(0.0f);
        p.get_position().set_y(0.0f);
        mesh_->vertex_add(p);

        p.get_position().set_y(height_);
        p.get_uv().set_y(height_ / static_cast<float>(SEGMENTS_PER_TEXTURE));
        mesh_->vertex_add(p);
        mesh_flat_->vertex_add(p);

        p.get_position().set_y(p.get_position().get_y() + cap_height);
        mesh_flat_->vertex_add(p);

        vector_buffer.at(points / 2) = p.get_position();
        vector_buffer.at(points / 2).set_y(height_ + (cap_height / 4));
        points += 2;
        skip_counter++;
    }

    // If this is a big building and it didn't get a logo, consider
    // giving it a light strip
    if (!logo_done && do_trim) {
        d = new Decoration;
        gl_rgba random_color;
        d->create_light_trim(vector_buffer,
                           (points / 2) - 2,
                           cap_height / 2.0f,
                           seed_,
                           from_hsl(random_val(255) / 255.0f,
                                    1.0f,
                                    1.0f));
    }

    qs.index_list.reserve(points);

    // Add the outer walls
    for (int i = 0; i < points; ++i) {
        qs.index_list.push_back(i);
    }

    mesh_->quad_strip_add(qs);
    mesh_flat_->quad_strip_add(qs);

    // Add the fan to cap the top of the buildings
    fan.index_list.push_back(points);
    for (int i = 0; i < (points / 2); ++i) {
        fan.index_list.push_back(points - (1 + (i * 2)));
    }
    p.get_position().set_x(center_.get_x());
    p.get_position().set_z(center_.get_z());

    mesh_flat_->vertex_add(p);
    mesh_flat_->fan_add(fan);

    radius /= 2.0f;

    // ConstructRoof((int)(center_.x - radius),
    //               (int)(center_.x + radius),
    //               (int)(center_.z - radius),
    //               (int)(center_.z + radius),
    //               height_ + cap_height);

    mesh_->compile();
    mesh_flat_->compile();
}

void Building::create_tower()
{
    int section_height;
    int section_width;
    int section_depth;
    int remaining_height;
    float uv_start;

    // How much ledges protrude from the building
    float ledge = random_val(3) * 0.25f;

    // How tall the ledges are, in stories
    int ledge_height = random_val(4) + 1;

    // If the corner of the building have no windows
    bool blank_corners = random_val(4) > 0;

    // How the windows are grouped
    int grouping = random_val(3) + 2;

    // What fraction of the remaining height should be given to each tier
    int tier_fraction = 2 + random_val(4);

    // How often (in tiers) does the building get narrower?
    int narrowing_interval = 1 + random_val(10);

    // The height of the windowless slab at the bottom
    int foundation = 2 + random_val(3);

    // Set our initial parameters
    int left = x_;
    int right = x_ + width_;
    int front = y_;
    int back = y_ + depth_;
    int bottom = 0;
    int tiers = 0;

    // Build the foundations
    construct_cube_float(left - ledge,
                         right + ledge,
                         front - ledge,
                         back + ledge,
                         bottom,
                         foundation);

    bottom += foundation;

    // Now add tiers until we reach the top
    while (true) {
        remaining_height = height_ - bottom;
        section_depth = back - front;
        section_width = right - left;
        section_height = std::max(remaining_height / tier_fraction, 2);
        if (remaining_height < 10) {
            section_height = remaining_height;
        }

        // Build the four walls
        uv_start =
            random_val(SEGMENTS_PER_TEXTURE) / static_cast<float>(SEGMENTS_PER_TEXTURE);

        uv_start = construct_wall(left,
                                  bottom,
                                  back,
                                  direction_t::south,
                                  section_depth,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;
        uv_start = construct_wall(left,
                                  bottom,
                                  front,
                                  direction_t::east,
                                  section_width,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;
        uv_start = construct_wall(right,
                                  bottom,
                                  front,
                                  direction_t::north,
                                  section_depth,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;
        uv_start = construct_wall(right,
                                  bottom,
                                  back,
                                  direction_t::west,
                                  section_width,
                                  section_height,
                                  grouping,
                                  uv_start,
                                  blank_corners) - ONE_SEGMENT;

        bottom += section_height;

        // Build the slab/ledges to cap this section.
        if ((bottom + ledge_height) > height_) {
            break;
        }

        construct_cube_float(left - ledge,
                             right + ledge,
                             front - ledge,
                             back + ledge,
                             bottom,
                             (bottom + ledge_height));

        bottom += ledge_height;
        if (bottom > height_) {
            break;
        }

        tiers++;
        if ((tiers % narrowing_interval) == 0) {
            if (section_width > 7) {
                left += 1;
                right -= 1;
            }
            if (section_depth > 7) {
                front += 1;
                back -= 1;
            }
        }
    }

    construct_roof(left, right, front, back, bottom);

    mesh_->compile();
    mesh_flat_->compile();
}
