/*
 * world.cpp
 *
 * 2009 Shamus Young
 *
 * This holds a bunch of variables used by other modules. It has the
 * claim system, which tracks all of the "property" that is being
 * used: As roads, buildings, etc.
 *
 */

#include "world.hpp"

#include <SDL2/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "building.hpp"
#include "camera.hpp"
#include "car.hpp"
#include "decoration.hpp"
#include "light.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "random.hpp"
#include "render.hpp"
#include "sky.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

#define LIGHT_COLOR_COUNT (sizeof(light_colors) / sizeof(HSL))

class Plot {
public:
    int x;
    int z;
    int width;
    int depth;
};

enum class fade_t {
    idle,
    out,
    wait,
    in
};

class HSL {
public:
    HSL(float hue, float sat, float lum);

    float hue;
    float sat;
    float lum;
};

HSL::HSL(float hue, float sat, float lum)
    : hue(hue)
    , sat(sat)
    , lum(lum)
{}

class Street {
public:
    int _x;
    int _y;
    int _width;
    int _depth;
    int _mesh;

    Street(int x, int y, int width, int depth);
    ~Street();
    void render();
};

static std::array<HSL, 14> light_colors = {
    HSL{0.04f, 0.9f, 0.93f}, // Amber/pink
    HSL{0.055f, 0.95f, 0.93f}, // Slightly brighter amber
    HSL{0.08f, 0.7f, 0.93f}, // Very pale amber
    HSL{0.07f, 0.9f, 0.93f}, // Very pale orange
    HSL{0.1f, 0.9f, 0.85f},  // Peach
    HSL{0.13f, 0.9f, 0.93f}, // Pale yellow
    HSL{0.15f, 0.9f, 0.93f}, // Yellow
    HSL{0.17f, 1.0f, 0.85f}, // Saturated yellow
    HSL{0.55f, 0.9f, 0.93f}, // Cyan
    HSL{0.6f, 0.9f, 0.93f}, // Pale Blue
    HSL{0.65f, 0.9f, 0.93f}, // Pale blue II, the palening
    HSL{0.65f, 0.4f, 0.99f}, // Pure white. bo-ring.
    HSL{0.65f, 0.0f, 0.8f}, // Dimmer white
    HSL{0.65, 0.0f, 0.6f}, // Dimmest white
};

static gl_rgba bloom_color;
static long int last_update;
static std::array<std::array<usage_t, WORLD_SIZE>, WORLD_SIZE> world;
static Sky *sky;
static fade_t fade_state;
static unsigned int fade_start;
static float fade_current;
static int modern_count;
static int tower_count;
static int blocky_count;
static bool reset_needed;
static int skyscrapers;
static gl_bbox hot_zone;
static int logo_index;
static unsigned int start_time;
static int scene_begin;

static gl_rgba get_light_color(float sat, float lum)
{
    int index = random_val(LIGHT_COLOR_COUNT);
    gl_rgba temp;
    return temp.from_hsl(light_colors[index].hue, sat, lum);
}

static void claim(int x, int y, int width, int depth, usage_t val)
{
    for (int xx = x; xx < (x + width); ++xx) {
        for (int yy = y; yy < (y + depth); ++yy) {
            int x_index = std::clamp(xx, 0, WORLD_SIZE - 1);
            int y_index = std::clamp(yy, 0, WORLD_SIZE - 1);

            world[x_index][y_index] = world[x_index][y_index] | val;
        }
    }
}

static bool claimed(int x, int y, int width, int depth)
{
    for (int xx = x; xx < (x + width); ++xx) {
        for (int yy = y; yy < (y + width); ++yy) {
            int x_index = std::clamp(xx, 0, WORLD_SIZE - 1);
            int y_index = std::clamp(yy, 0, WORLD_SIZE - 1);

            if (world[x_index][y_index] != usage_t::none) {
                return true;
            }
        }
    }

    return false;
}

static void build_road(int x1, int y1, int width, int depth)
{
    int lanes;
    int divider;

    // The given rectangle defines a street and its sidewalk. See which way
    // it goes.
    if (width > depth) {
        lanes = depth;
    } else {
        lanes = width;
    }

    // If we don't have room for both lanes and sidewalk, abort
    if (lanes < 4) {
        return;
    }

    // If we have an odd number of lanes give the extra to a divider
    if (lanes % 2) {
        lanes--;
        divider = 1;
    } else {
        divider = 0;
    }

    // No more than 10 traffic lanes, give the rest to sidewalks
    int sidewalk = std::max(2, (lanes - 10));
    lanes -= sidewalk;
    sidewalk /= 2;

    // Take the remaining space and give half to each direction
    lanes /= 2;

    // Mark the entire rectangle as used
    claim(x1, y1, width, depth, usage_t::claim_walk);

    // No place the directional roads
    if (width > depth) {
        claim(x1, y1 + sidewalk, width, lanes, usage_t::claim_road | usage_t::map_road_west);
        claim(x1,
              y1 + sidewalk + lanes + divider,
              width,
              lanes,
              usage_t::claim_road | usage_t::map_road_west);
    } else {
        claim(x1 + sidewalk, y1, lanes, depth, usage_t::claim_road | usage_t::map_road_south);
        claim(x1 + sidewalk + lanes + divider,
              y1,
              lanes,
              depth,
              usage_t::claim_road | usage_t::map_road_south);
    }
}

static Plot find_plot(int x, int z)
{
    Plot p;

    // We've been given the location of an open bit of land, be we have
    // no idea how big it is. Find the boundary.
    int x2 = x;
    int x1 = x;

    while (!claimed(x1 - 1, z, 1, 1) && (x1 > 0)) {
        x1--;
    }

    while (!claimed(x2 + 1, z, 1, 1) && (x2 < WORLD_SIZE)) {
        x2++;
    }

    int z2 = z;
    int z1 = z;

    while (!claimed(x, z1 - 1, 1, 1) && (z1 > 0)) {
        z1--;
    }

    while (!claimed(x, z2 + 1, 1, 1) && (z2 < WORLD_SIZE)) {
        z2++;
    }

    p.width = x2 - x1;
    p.depth = z2 - z1;
    p.x = x1;
    p.z = z1;

    return p;
}

static Plot make_plot(int x, int z, int width, int depth)
{
    Plot p = {x, z, width, depth};

    return p;
}

void do_building(Plot p)
{
    int height;
    building_t type;

    // Now we know how bit the rectangle plot is.
    int area = p.width * p.depth;
    gl_rgba color = world_light_color(random_val());
    int seed = random_val();

    // Make sure the plot is big enough for a building
    if ((p.width < 10) || (p.depth < 10)) {
        return;
    }

    // If the area is too big for one building subdivide it
    if (area > 800) {
        if (random_val(2) == 0) {
            p.width /= 2;
            if (random_val(2) == 0) {
                do_building(make_plot(p.x, p.z, p.width, p.depth));
            } else {
                do_building(make_plot(p.x + p.width, p.z, p.width, p.depth));
            }

            return;
        } else {
            p.depth /= 2;
            if (random_val(2) == 0) {
                do_building(make_plot(p.x, p.z, p.width, p.depth));
            } else {
                do_building(make_plot(p.x, p.z + p.depth, p.width, p.depth));
            }

            return;
        }
    }

    if (area < 100) {
        return;
    }

    // The plot is "square" if width and depth are close
    bool square = (abs(p.width - p.depth) < 10);

    // Mark the land as used so other buildings don't appear here,
    // even if we don't use it all.
    claim(p.x, p.z, p.width, p.depth, usage_t::claim_building);

    // The roundy mod buildings look best on square plots.
    if (square && (p.width > 20)) {
        height = 45 + random_val(10);
        modern_count++;
        skyscrapers++;
        new Building(building_t::modern,
                     p.x,
                     p.z,
                     height,
                     p.width,
                     p.depth,
                     seed,
                     color);

        return;
    }

    // // Rectangular plots are a good place for Blocky style buildings
    // // to sprawl blockily
    // if ((p.width > (p.depth * 2))
    //    || ((p.depth > (p.width * 2)) && (area > 800))) {
    //     height = 20 + random_val(10);
    //     blocky_count++;
    //     skyscrapers++;
    //     new CBuilding(BUILDING_BLOCKY,
    //                   p.x,
    //                   p.z,
    //                   height,
    //                   p.width,
    //                   p.depth,
    //                   seed,
    //                   color);

    //     return;
    // }

    // tower_count = -1;

    // This spot isn't ideal for any particular building, but try to keep
    // a good mix
    if ((tower_count < modern_count) && (tower_count < blocky_count)) {
        type = building_t::tower;
        tower_count++;
    } else if (blocky_count < modern_count) {
        type = building_t::blocky;
        blocky_count++;
    } else {
        type = building_t::modern;
        modern_count++;
    }

    height = 45 + random_val(10);

    new Building(type, p.x, p.z, height, p.width, p.depth, seed, color);
    skyscrapers++;
}

static int build_light_strip(int x1, int z1, direction_t direction)
{
    Decoration *d;
    gl_rgba color;
    int dir_x;
    int dir_z;

    // We adjust the size of the lights with this.
    float size_adjust = 2.5f;
    gl_rgba temp;
    color = temp.from_hsl(0.09f, 0.99f, 0.85f);

    switch(direction) {
    case direction_t::north:
        dir_z = 1;
        dir_x = 0;
        break;
    case direction_t::south:
        dir_z = 1;
        dir_x = 0;
        break;
    case direction_t::east:
        dir_z = 0;
        dir_x = 1;
        break;
    case direction_t::west:
        dir_z = 0;
        dir_x = 1;
        break;
    }

    // So we know we're on the corner of an intersection
    // look in the given until we reach the end of
    // the sidewalk
    int x2 = x1;
    int z2 = z1;
    int length = 0;

    while ((x2 > 0) && (x2 < WORLD_SIZE) && (z2 > 0) && (z2 < WORLD_SIZE)) {
        if ((world[x2][z2] & usage_t::claim_road) != usage_t::none) {
            break;
        }

        length++;
        x2 += dir_x;
        z2 += dir_z;
    }

    if (length < 10) {
        return length;
    }

    int width = std::max(abs(x2 - x1), 1);
    int depth = std::max(abs(z2 - z1), 1);

    d = new Decoration();

    if (direction == direction_t::east) {
        d->create_light_strip(x1,
                            z1 - size_adjust,
                            width,
                            depth + size_adjust,
                            2,
                            color);
    } else if (direction == direction_t::west) {
        d->create_light_strip(x1,
                            z1,
                            width,
                            depth + size_adjust,
                            2,
                            color);
    } else if (direction == direction_t::north) {
        d->create_light_strip(x1,
                            z1,
                            width + size_adjust,
                            depth,
                            2,
                            color);
    } else {
        d->create_light_strip(x1 - size_adjust,
                            z1,
                            width - size_adjust,
                            depth,
                            2,
                            color);
    }

    return length;
}

static void do_reset()
{
    int width;
    int depth;
    int height;
    gl_rgba building_color;
    float west_street;
    float north_street;
    float east_street;
    float south_street;

    // Re-init Random to make the same city each time.
    // Helpful when running tests.
    random_init(6);
    reset_needed = false;
    bool broadway_done = false;
    skyscrapers = 0;
    logo_index = 0;
    scene_begin = 0;
    modern_count = 0;
    blocky_count = 0;
    tower_count = 0;
    hot_zone.clear();
    entity_clear();
    light_clear();
    car_clear();
    texture_reset();

    // Pink a tint for the bloom
    bloom_color = get_light_color(0.5f + ((float)random_val(10) / 20.0f), 0.75f);
    gl_rgba temp;
    // gl_rgba light_color = temp.from_hsl(0.11f, 1.0f, 0.65f);
    for (std::array<usage_t, WORLD_SIZE> &row : world) {
        row.fill(static_cast<usage_t>(0));
    }
    for (int y = WORLD_EDGE; y < (WORLD_SIZE - WORLD_EDGE); y += random_val(25) + 25) {
        if (!broadway_done && (y > (WORLD_HALF - 20))) {
            build_road(0, y, WORLD_SIZE, 19);
            y += 20;
            broadway_done = true;
        } else {
            depth = 6 + random_val(6);
            if (y < (WORLD_HALF / 2)) {
                north_street = (y + (depth / 2.0f));
            }

            if (y < (WORLD_SIZE - (WORLD_HALF / 2))) {
                south_street = (y + (depth / 2.0f));
            }

            build_road(0, y, WORLD_SIZE, depth);
        }
    }

    broadway_done = false;
    for (int x = WORLD_EDGE; x < (WORLD_SIZE - WORLD_EDGE); x += random_val(25) + 25) {
        if (!broadway_done && (x > (WORLD_HALF - 20))) {
            build_road(x, 0, 19, WORLD_SIZE);
            x += 20;
            broadway_done = true;
        } else {
            width = 6 + random_val(6);
            if (x <= (WORLD_HALF / 2)) {
                west_street = (x + (width / 2.0f));
            }
            if (x <= (WORLD_HALF + (WORLD_HALF / 2))) {
                east_street = (x + (width / 2.0f));
            }

            build_road(x, 0, width, WORLD_SIZE);
        }
    }

    // We kept track of the positions of streets that will outline the
    // high-detail hot zone in the middle of the world. Save this in a
    // bounding box so that late we can have the camera fly around without
    // clipping through buildings.
    hot_zone.contain_point(gl_vector3(west_street, 0.0f, north_street));

    hot_zone.contain_point(gl_vector3(east_street, 0.0f, south_street));

    // Scan for places to put runs of streetlights on the east and west
    // size of the road
    for (int x = 1; x < (WORLD_SIZE - 1); ++x) {
        for (int y = 0; y < WORLD_SIZE; ++y) {
            // If this isn't a bit of sidewalk, then keep looking
            if ((world[x][y] & usage_t::claim_walk) == usage_t::none) {
                continue;
            }

            // If it's used as a road, skip it.
            if ((world[x][y] & usage_t::claim_road) != usage_t::none) {
                continue;
            }

            bool road_left = ((world[x + 1][y] & usage_t::claim_road) != usage_t::none);
            bool road_right = ((world[x - 1][y] & usage_t::claim_road) != usage_t::none);

            // If the cells to our east and west are not road,
            // then we're not on a corner.
            if (!road_left && !road_right) {
                continue;
            }

            // If the cell to our east and west is road, then we're on
            // a median. skip it
            if (road_left && road_right) {
                continue;
            }

            y += build_light_strip(x, y, road_right ? direction_t::south : direction_t::north);
        }
    }

    // Scan for places to put runs of streetlights on the north
    // and south side of the road
    for (int y = 1; y < (WORLD_SIZE - 1); ++y) {
        for (int x = 1; x < (WORLD_SIZE - 1); ++x) {
            // If this isn't a bit of sidewalk, then keep looking
            if ((world[x][y] & usage_t::claim_walk) == usage_t::none) {
                continue;
            }

            // If it's used as a road, skip it.
            if ((world[x][y] & usage_t::claim_road) != usage_t::none) {
                continue;
            }

            bool road_left = ((world[x][y + 1] & usage_t::claim_road) != usage_t::none);
            bool road_right = ((world[x][y - 1] & usage_t::claim_road) != usage_t::none);

            // If the cell to our east and west is road, then we're on
            // a median. skip it
            if (road_left && road_right) {
                continue;
            }

            // If the cells to our north and source are not road,
            // then we're not on a corner.
            if (!road_left && !road_right) {
                continue;
            }

            x += build_light_strip(x, y, road_right ? direction_t::east : direction_t::west);
        }
    }

    // Scan over the center area of the map and place the big buildings
    int attempts = 0;
    while ((skyscrapers < 50) && (attempts < 350)) {
        int x = (WORLD_HALF / 2) + (random_val() % WORLD_HALF);
        int y = (WORLD_HALF / 2) + (random_val() % WORLD_HALF);
        if (!claimed(x, y, 1, 1)) {
            do_building(find_plot(x, y));
            skyscrapers++;
        }

        attempts++;
    }

    // Now blanket the rest of the world with lesser buildings
    for (int x = 0; x < WORLD_SIZE; ++x) {
        for (int y = 0; y < WORLD_SIZE; ++y) {
            if (world[std::clamp(x, 0, WORLD_SIZE)][std::clamp(y, 0, WORLD_SIZE)] != usage_t::none) {
                continue;
            }

            width = 12 + random_val(20);
            depth = 12 + random_val(20);
            height = std::min(width, depth);

            if ((x < 30)
               || (y < 30)
               || (x > (WORLD_SIZE - 30))
               || (y > (WORLD_SIZE - 30))) {
                height = random_val(15 + 20);
            } else if (x < (WORLD_HALF / 2)) {
                height /= 2;
            }

            while ((width > 8) && (depth > 8)) {
                if (!claimed(x, y, width, depth)) {
                    claim(x, y, width, depth, usage_t::claim_building);
                    building_color = world_light_color(random_val());

                    // If we're out of the host zone use simple builings
                    if ((x < hot_zone.get_min().get_x())
                       || (x > hot_zone.get_max().get_x())
                       || (y < hot_zone.get_min().get_z())
                       || (y > hot_zone.get_max().get_z())) {
                        height = 5 + random_val(height) + random_val(height);

                        new Building(building_t::simple,
                                     x + 1,
                                     y + 1,
                                     height,
                                     width - 2,
                                     depth - 2,
                                     random_val(),
                                     building_color);
                    } else {
                        // Use fancy buildings.
                        height = 15 + random_val(15);
                        width -= 2;
                        depth -= 2;
                        if (random_val(2) == 0) {
                            new Building(building_t::tower,
                                         x + 1,
                                         y + 1,
                                         height,
                                         width,
                                         depth,
                                         random_val(),
                                         building_color);
                        } else {
                            new Building(building_t::blocky,
                                         x + 1,
                                         y + 1,
                                         height,
                                         width - 2,
                                         depth - 2,
                                         random_val(),
                                         building_color);
                        }
                    }

                    break;
                }

                width--;
                depth--;
            }

            // Leave big gaps near the edge of the map,
            // no need to pack detail there.
            if ((y < WORLD_EDGE) || (y > (WORLD_SIZE - WORLD_EDGE))) {
                y += 32;
            }
        }

        // Leave big gaps near the edge of the map
        if ((x < WORLD_EDGE) || (x > (WORLD_SIZE - WORLD_EDGE))) {
            x += 28;
        }
    }
}

// This will return a random color which is suitable for light sources, taken
// from a narrow group of hues. (Yellows, oranges, blues.)
gl_rgba world_light_color(unsigned int index)
{
    index %= LIGHT_COLOR_COUNT;
    gl_rgba temp;
    return temp.from_hsl(light_colors[index].hue,
                         light_colors[index].sat,
                         light_colors[index].lum);
}

usage_t world_cell(int x, int y)
{
    int x_index = std::clamp(x, 0, WORLD_SIZE - 1);
    int y_index = std::clamp(y, 0, WORLD_SIZE - 1);

    return world[x_index][y_index];
}

gl_rgba world_bloom_color()
{
    return bloom_color;
}

int world_logo_index()
{
    return logo_index++;
}


gl_bbox world_hot_zone()
{
    return hot_zone;
}

void world_term()
{
}

void world_reset()
{
    // If we're already fading out, then this is the developer
    // hammering on the "rebuild" button. Let's hurry up for the
    // nice man...
    if (fade_state == fade_t::out) {
        do_reset();
    }

    // If reset is called but the world isn't ready, then don't
    // bother fading out. The program probably just started.
    fade_state = fade_t::out;
    fade_start = SDL_GetTicks();
}

void world_render()
{
    if (!SHOW_DEBUG_GROUND) {
        return;
    }

    // Render a single texture over the city that shows
    // traffic lances
    glDepthMask(false);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);

    glTexCoord2f(0, 1);
    glVertex3f(0, 0, 1024);

    glTexCoord2f(1, 1);
    glVertex3f(1024, 0, 1024);

    glTexCoord2f(1, 0);
    glVertex3f(1024, 0, 0);

    glEnd();

    glDepthMask(true);
}

float world_fade()
{
    return fade_current;
}

int world_scene_begin()
{
    return scene_begin;
}

// How long since this current iteration of the city went
// on display
int world_scene_elapsed()
{
    int elapsed;

    if (!entity_ready() || !world_scene_begin()) {
        elapsed = 1;
    } else {
        elapsed = SDL_GetTicks() - world_scene_begin();
    }

    elapsed = std::max(elapsed, 1);

    return elapsed;
}

void world_update()
{
    unsigned fade_delta;
    int now;

    now = SDL_GetTicks();
    if (reset_needed) {
        // Now we've faded out the scene, rebuild it
        do_reset();
    }

    if (fade_state != fade_t::idle) {
        if ((fade_state == fade_t::wait) && texture_ready() && entity_ready()) {
            fade_state = fade_t::in;
            fade_start = now;
            fade_current = 1.0f;
        }

        fade_delta = now - fade_start;

        // See if we're done fading in or out
        if ((fade_delta > FADE_TIME) && (fade_state != fade_t::wait)) {
            if (fade_state == fade_t::out) {
                reset_needed = true;
                fade_state = fade_t::wait;
                fade_current = 1.0f;
            } else {
                fade_state = fade_t::idle;
                fade_current = 0.0f;
                start_time = time(NULL);
                scene_begin = SDL_GetTicks();
            }
        } else {
            fade_current = (float)fade_delta / FADE_TIME;
            if (fade_state == fade_t::in) {
                fade_current = 1.0f - fade_current;
            }

            if (fade_state == fade_t::wait) {
                fade_current = 1.0f;
            }
        }

        if (!texture_ready()) {
            fade_current = 1.0f;
        }
    }

    if ((fade_state == fade_t::idle) && !texture_ready()) {
        fade_state = fade_t::in;
        fade_start = now;
    }

    if ((fade_state == fade_t::idle) && (world_scene_elapsed() > RESET_INTERVAL)) {
        world_reset();
    }
}

void world_init()
{
    last_update = SDL_GetTicks();
    for (int i = 0; i < CARS; ++i) {
        new Car();
    }

    sky = new Sky();
    world_reset();
    fade_state = fade_t::out;
    fade_start = 0;
}
