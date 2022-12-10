/*
 * texture.cpp
 *
 * 2009 Shamus Young
 *
 * This procedurally builds all of the textures.
 *
 * I apologize in advance for the apalling state of this module. It's the victim
 * of iterative and experimental development. It has cruft, poorly named
 * functions, obscure code, poorly named variables, and is badly organized. Even
 * the formatting sucks in places. Its only saving grace is that it works.
 *
 */

#include "texture.hpp"

#include <SDL2/SDL.h>
#include <GL/glu.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <list>

#include "building.hpp"
#include "camera.hpp"
#include "car.hpp"
#include "light.hpp"
#include "math.hpp"
#include "random.hpp"
#include "render.hpp"
#include "sky.hpp"
#include "texture.hpp"
#include "win.hpp"
#include "world.hpp"

static std::array<std::string, 23> prefix = {
    "i",
    "Green ",
    "Mega",
    "Super ",
    "Omni",
    "e",
    "Hyper",
    "GLobal ",
    "Vital ",
    "Next ",
    "Pacific ",
    "Metro",
    "Unity ",
    "G-",
    "Trans",
    "Infinity ",
    "Superior ",
    "Monolith ",
    "Best ",
    "Atlantic ",
    "First ",
    "Union ",
    "National "
};

std::array<std::string, 33> name = {
    "Biotic",
    "Info",
    "Data",
    "Solar",
    "Aerospace",
    "Motors",
    "Nano",
    "Online",
    "Circuits",
    "Energy",
    "Med",
    "Robotic",
    "Exports",
    "Security",
    "Systems",
    "Financial",
    "Industrial",
    "Media",
    "Materials",
    "Foods",
    "Networks",
    "Shipping",
    "Tools",
    "Medical",
    "Publishing",
    "Enterprises",
    "Audio",
    "Health",
    "Bank",
    "Imports",
    "Apparel",
    "Petroleum",
    "Studios"
};

std::array<std::string, 15> suffix = {
    "Corp.",
    " Inc.",
    "Co",
    "World",
    ".Com",
    " USA",
    " Ltd.",
    "Net",
    " Tech",
    " Labs",
    " Mfg.",
    " UK",
    " Unlimited",
    " One",
    " LLC"
};


void Texture::clear()
{
    ready_ = false;
}

static std::list<Texture> textures;
static bool textures_done;
static std::array<bool, prefix.size()> prefix_used;
static std::array<bool, name.size()> name_used;
static std::array<bool, suffix.size()> suffix_used;
static int build_time;

void drawrect_simple(int left, int top, int right, int bottom, gl_rgba color)
{
    glColor3fv(color.get_rgb().data());
    glBegin(GL_QUADS);
    glVertex2i(left, top);
    glVertex2i(right, top);
    glVertex2i(right, bottom);
    glVertex2i(left, bottom);
    glEnd();
}

void drawrect_simple(int left,
                     int top,
                     int right,
                     int bottom,
                     gl_rgba color1,
                     gl_rgba color2)
{
    glColor3fv(color1.get_rgb().data());
    glBegin(GL_TRIANGLE_FAN);
    glVertex2i((left + right) / 2, (top + bottom) / 2);
    glColor3fv(color2.get_rgb().data());
    glVertex2i(left, top);
    glVertex2i(right, top);
    glVertex2i(right, bottom);
    glVertex2i(left, bottom);
    glVertex2i(left, top);
    glEnd();
}

void drawrect(int left, int top, int right, int bottom, gl_rgba color)
{
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3fv(color.get_rgb().data());

    // In low resolution, a "rect" might be 1 pixel wide
    if (left == right) {
        glBegin(GL_LINES);
        glVertex2i(left, top);
        glVertex2i(left, bottom);
        glEnd();
    }

    // In low resolution, a "rect" might be 1 pixel wide
    if (top == bottom) {
        glBegin(GL_LINES);
        glVertex2i(left, top);
        glVertex2i(right, top);
        glEnd();
    } else {
        // Draw one of those fancy 2-dimensional rectangles
        glBegin(GL_QUADS);
        glVertex2i(left, top);
        glVertex2i(right, top);
        glVertex2i(right, bottom);
        glVertex2i(left, bottom);
        glEnd();

        std::array<float, 3> vals = color.get_rgb();
        float average = (vals.at(0) + vals.at(1) + vals.at(2)) / 3.0f;
        bool bright = (average > 0.5f);
        int potential = (average * 255.0f);

        if (bright) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_POINTS);
            for (int i = left + 1; i < right - 1; ++i) {
                for (int j = top + 1; j < bottom - 1; ++j) {
                    glColor4i(255, 0, random_val(potential), 255);
                    float hue = 0.2f
                        + (random_val(100) / 300.0f)
                        + (random_val(100) / 300.0f)
                        + (random_val(100) / 300.0f);

                    gl_rgba color_noise = from_hsl(hue, 0.3f, 0.5f);
                    color_noise.set_alpha((random_val(potential) / 144.0f) * 255);
                    glColor4f(random_val(256) / 256.0f,
                              random_val(256) / 256.0f,
                              random_val(256) / 256.0f,
                              random_val(potential) / 144.0f);
                    glColor4fv(color_noise.get_rgba().data());
                    glVertex2i(i, j);
                }
            }
            glEnd();
        }

        int height = (bottom - top) + (random_val(3) - 1) + (random_val(3) - 1);

        for (int i = left; i < right; ++i) {
            if (random_val(6) == 0) {
                height = bottom - top;
                height = random_val(height);
                height = random_val(height);
                height = random_val(height);
                height = ((bottom - top) + height) / 2;
            }

            for (int j = 0; j < 1; ++j) {
                glBegin(GL_LINES);
                glColor4f(0, 0, 0, random_val(256) / 256.0f);
                glVertex2i(i, bottom - height);
                glColor4f(0, 0, 0, random_val(256) / 256.0f);
                glVertex2i(i, bottom);
                glEnd();
            }
        }
    }
}

static void buildingWindow(int x, int y, int size, texture_t id, gl_rgba color)
{
    int i;

    int margin = size / 3;
    int half = size / 2;

    switch(id) {
    case texture_t::building1:
        // Filled, 1-pixel frame
        drawrect(x + 1, y + 1, x + size - 1, y + size - 1, color);
        break;
    case texture_t::building2:
        // Vertical
        drawrect(x + margin, y + 1, x + size - margin, y + size - 1, color);
        break;
    case texture_t::building3:
        // Side-by-side pair
        drawrect(x + 1, y + 1, x + half - 1, y + size - margin, color);
        drawrect(x + half + 1, y + 1, x + size - 1, y + size - margin, color);
        break;
    case texture_t::building4:
        // Windows with blinds
        drawrect(x + 1, y + 1, x + size - 1, y + size - 1, color);
        i = random_val(size - 2);
        drawrect(x + 1, y + 1, x + size - 1, y + i + 1, color * 0.3f);
        break;
    case texture_t::building5:
        // Vert stripes
        drawrect(x + 1, y + 1, x + size - 1, y + size - 1, color);
        drawrect(x + margin, y + 1, x + margin, y + size - 1, color * 0.7f);
        drawrect(x + size - margin - 1,
                 y + 1,
                 x + size - margin - 1,
                 y + size - 1,
                 color * 0.3f);
        break;
    case texture_t::building6:
        // Wide horz line
        drawrect(x + 1, y + 1, x + size - 1, y + size - margin, color);
        break;
    case texture_t::building7:
        // 4-pane
        drawrect(x + 2, y + 1, x + size - 1, y + size - 1, color);
        drawrect(x + 2, y + half, x + size - 1, y + half, color * 0.2f);
        drawrect(x + half, y + 1, x + half, y + size - 1, color * 0.2f);
        break;
    case texture_t::building8:
        // Single narrow window
        drawrect(x + half - 1, y + 1, x + half + 1, y + size - margin, color);
        break;
    case texture_t::building9:
        // Horizontal
        drawrect(x + 1, y + margin, x + size - 1, y + size - margin - 1, color);
        break;
    default:
        break;
    }
}

static void do_bloom(Texture const &t)
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, t.size_, t.size_);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FOG);
    glFogf(GL_FOG_START, render_fog_distance() / 2);
    glFogf(GL_FOG_END, render_fog_distance());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    entity_render();
    car_render();
    light_render();
    glBindTexture(GL_TEXTURE_2D, t.glid_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, t.size_, t.size_, 0);
}

Texture::Texture(texture_t id, int size, bool mipmap, bool clamp, bool masked)
{
    glGenTextures(1, &glid_);
    my_id_ = id;
    mipmap_ = mipmap;
    clamp_ = clamp;
    masked_ = masked;
    desired_size_ = size;
    size_ = size;
    half_ = size / 2;
    segment_size_ = size / SEGMENTS_PER_TEXTURE;
    ready_ = false;
    textures.push_back(*this);
}

// This draws all of the windows on a building texture. lit_density controls
// how many lights are on. (1 in n chance that the light is on. Higher values
// mean less lit windows). run_length controls how often it will consider
// changing the lit/unlit status. 1 produces a complete scatter, higher
// numbers make long strings of lights
void Texture::draw_windows()
{
    int run;
    int run_length;
    int lit_density;
    gl_rgba color;
    bool lit;

    // color = glRgbaUnique(_my_id);
    for (int y = 0; y < SEGMENTS_PER_TEXTURE; ++y) {
        // Every few floors we change the behavior
        if (!(y % 8)) {
            run = 0;
            run_length = random_val(9) + 2;
            lit_density = 2 + random_val(2) + random_val(2);
            lit = false;
        }

        for (int x = 0; x < SEGMENTS_PER_TEXTURE; ++x) {
            // If this run is over reroll lit and start a new one
            if (run < 1) {
                run = random_val(run_length);
                lit = (random_val(lit_density) == 0);
                // if (lit) {
                //     color = glRgba(0.5f + (float)(random_val() % 128) / 256.0f)
                //         + glRgba(random_val(10) / 50.0f,
                //                  random_val(10) / 50.0f,
                //                  random_val(10) / 50.0f);
                // }
            }

            if (lit) {
                int rand_val = random_val() % 128;
                color = gl_rgba(127 + rand_val, 127 + rand_val, 127 + rand_val)
                    + gl_rgba(random_val(10) / 50.0f,
                              random_val(10) / 50.0f,
                              random_val(10) / 50.0f);
            } else {
                int rand_val = random_val() % 40;
                color = gl_rgba(rand_val, rand_val, rand_val);
            }

            buildingWindow(x * segment_size_,
                           y * segment_size_,
                           segment_size_,
                           my_id_,
                           color);

            run--;
        }
    }
}

void Texture::draw_sky()
{
    float inv_scale;
    int height;
    int width_adjust;
    int height_adjust;

    gl_rgba color = world_bloom_color();

    // Desaturate, slightly dim
    color = (color / 15.0f) + (color / 45.0f) + (color / 45.0f);
    glDisable(GL_BLEND);
    glBegin(GL_QUAD_STRIP);
    glColor3f(0, 0, 0);
    glVertex2i(0, half_);
    glVertex2i(size_, half_);
    glColor3fv(color.get_rgb().data());
    glVertex2i(0, size_ - 2);
    glVertex2i(size_, size_ - 2);
    glEnd();

    // Draw a bunch of little faux-buildings on the horizon
    for (int i = 0; i < size_; i += 5) {
        drawrect(i,
                 size_ - random_val(8) - random_val(8) - random_val(8),
                 i + random_val(9),
                 size_,
                 gl_rgba(0, 0, 0));
    }

    // Draw the clouds
    for (int i = size_ - 30; i > 5; i -= 2) {
        int x = random_val(size_);
        int y = i;

        float scale = 1.0f - (static_cast<float>(y) / size_);
        int width = random_val(half_ / 2) + (half_ * scale) / 2;
        scale = 1.0f - (static_cast<float>(y) / size_);
        height = (width * scale);
        height = std::max(height, 4);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_id(texture_t::soft_circle));
        glDepthMask(false);
        glBegin(GL_QUADS);
        for (int offset = -size_; offset <= size_; offset += size_) {
            for (float scale = 1.0f; scale > 0.0f; scale -= 0.25f) {
                inv_scale = 1.0f - scale;
                if (scale < 0.4f) {
                    color = world_bloom_color() * 0.1f;
                } else {
                    color = gl_rgba(0, 0, 0);
                }

                color.set_alpha(51);
                glColor4fv(color.get_rgba().data());
                width_adjust =
                    ((width / 2.0f) + (inv_scale * (width / 2.0f)));

                height_adjust = height + (scale * height * 0.99f);

                glTexCoord2f(0, 0);
                glVertex2i(offset + x - width_adjust,
                           y + height - height_adjust);

                glTexCoord2f(0, 1);
                glVertex2i(offset + x - width_adjust, y + height);

                glTexCoord2f(1, 1);
                glVertex2i(offset + x + width_adjust, y + height);

                glTexCoord2f(1, 0);
                glVertex2i(offset + x + width_adjust,
                           y + height - height_adjust);
            }
        }
    }

    glEnd();
}

void Texture::draw_headlight()
{
    gl_vector2 pos;

    // Make a simple circle of light, bright in the center and fading out
    float radius = (half_) - 20;
    int x = half_ - 20;
    int y = half_;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.8f, 0.8f, 0.8f, 0.6f);
    glVertex2i(half_ - 5, y);
    glColor4f(0, 0, 0, 0);
    for (int i = 0; i <= 360; i += 36) {
        pos.set_x(sinf((i % 360) * DEGREES_TO_RADIANS) * radius);
        pos.set_y(cosf((i % 360) * DEGREES_TO_RADIANS) * radius);
        glVertex2i(x + pos.get_x(), half_ + pos.get_y());
    }
    glEnd();

    x = half_ + 20;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.8f, 0.8f, 0.8f, 0.6f);
    glVertex2i(half_ + 5, y);
    glColor4f(0, 0, 0, 0);
    for (int i = 0; i <= 360; i += 36) {
        pos.set_x(sinf((i % 360) * DEGREES_TO_RADIANS) * radius);
        pos.set_y(cosf((i % 360) * DEGREES_TO_RADIANS) * radius);
        glVertex2i(x + pos.get_x(), half_ + pos.get_y());
    }
    glEnd();

    x = half_ - 6;
    drawrect_simple(x - 3, y - 2, x + 2, y + 2, gl_rgba(255, 255, 255));
    x = half_ + 6;
    drawrect_simple(x - 2, y - 2, x + 3, y + 2, gl_rgba(255, 255, 255));
};

// Here is where ALL of the procedural textures are created. It's filled with
// obscure logic, magic numbers, and message code. Part of this is because
// there is a lot of "art" being done here, and lots of numbers that could be
// endlessly tweaked. Also because I'm lazy.
void Texture::rebuild()
{
    int i;
    int y;
    int name_num;
    int prefix_num;
    int suffix_num;
    float radius;
    gl_vector2 pos;
    unsigned char *bits;
    int lapsed;

    unsigned int start = SDL_GetTicks();

    // Since we make textures by drawing into the viewport, we can't make
    // them bigger than the current view.
    size_ = desired_size_;
    int max_size = render_max_texture_size();
    while (size_ > max_size) {
        size_ /= 2;
    }

    glBindTexture(GL_TEXTURE_2D, glid_);
    // Set up the texutre
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 size_,
                 size_,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (clamp_) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Set up our viewport so that drawing into our texture will be as
    // easy as possible. We make the viewport and projection simply
    // match the given texture size.
    glViewport(0, 0, size_, size_);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, size_, size_, 0, 0.1f, 2048);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    glDisable(GL_FOG);
    glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(0, 0, -10.0f);
    glClearColor(0, 0, 0, masked_ ? 0.0f : 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bool use_framebuffer = true;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    switch(my_id_) {
    case texture_t::lattice:
        glLineWidth(2.0f);

        glColor3f(0, 0, 0);
        glBegin(GL_LINES);

        // Diagonal
        glVertex2i(0, 0);
        glVertex2i(size_, size_);

        // Vertical
        glVertex2i(0, 0);
        glVertex2i(0, size_);

        // Vertical
        glVertex2i(0, 0);
        glVertex2i(size_, 0);

        glEnd();

        glBegin(GL_LINE_STRIP);
        glVertex2i(0, 0);
        for (int i = 0; i < size_; i += 9) {
            if (i % 2) {
                glVertex2i(0, i);
            } else {
                glVertex2i(i, i);
            }
        }

        for (int i = 0; i < size_; i += 9) {
            if (i % 2) {
                glVertex2i(i, 0);
            } else {
                glVertex2i(i, i);
            }
        }
        glEnd();
        break;
    case texture_t::soft_circle:
        // Make a simple circle of light, bright in the center and fading out
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        radius = (float)half_ - 3;
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1, 1, 1, 1);
        glVertex2i(half_, half_);
        glColor4f(0, 0, 0, 0);
        for (int i = 0; i <= 360; ++i) {
            pos.set_x(sinf((float)i * DEGREES_TO_RADIANS) * radius);
            pos.set_y(cosf((float)i * DEGREES_TO_RADIANS) * radius);
            glVertex2i(half_ + (int)pos.get_x(), half_ + (int)pos.get_y());
        }
        glEnd();
        break;
    case texture_t::light:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        radius = (float)half_ - 3;
        for (int j = 0; j < 2; ++j) {
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(1, 1, 1, 1);
            glVertex2i(half_, half_);
            if (!j) {
                radius = (float)half_ / 2;
            } else {
                radius = 8;
            }

            glColor4f(1, 1, 1, 0);
            for (int i = 0; i <= 360; ++i) {
                pos.set_x(sinf((float)i * DEGREES_TO_RADIANS) * radius);
                pos.set_y(cosf((float)i * DEGREES_TO_RADIANS) * radius);
                glVertex2i(half_ + (int)pos.get_x(), half_ + (int)pos.get_y());
            }
            glEnd();
        }
        break;
    case texture_t::headlight:
        draw_headlight();
        break;
    case texture_t::logos:
        i = 0;
        glDepthMask(false);
        glDisable(GL_BLEND);
        name_num = random_val(name.size());
        prefix_num = random_val(prefix.size());
        suffix_num = random_val(suffix.size());
        glColor3f(1, 1, 1);

        while (i < size_) {
            // Randomly use a prefix OR suffix, but not both. Too verbose.
            if (random_val(2) == 0) {
                render_print(2,
                             size_ - i - (LOGO_PIXELS / 4),
                             random_val(),
                             gl_rgba(255, 255, 255),
                             "%s%s",
                             prefix[prefix_num].c_str(),
                             name[name_num].c_str());
            } else {
                render_print(2,
                             size_ - i - (LOGO_PIXELS / 4),
                             random_val(),
                             gl_rgba(255, 255, 255),
                             "%s%s",
                             name[name_num].c_str(),
                             suffix[suffix_num].c_str());
            }

            name_num = (name_num + 1) % name.size();
            prefix_num = (prefix_num + 1) % prefix.size();
            suffix_num = (suffix_num + 1) % suffix.size();
        }
        break;
    case texture_t::trim:
        int margin;
        y = 0;
        margin = std::max(TRIM_PIXELS / 4, 1);

        for (int x = 0; x < size_; x += TRIM_PIXELS) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }

        y += TRIM_PIXELS;
        for (int x = 0; x < size_; x += TRIM_PIXELS * 2) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }

        y += TRIM_PIXELS;
        for (int x = 0; x < size_; x += TRIM_PIXELS * 3) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }

        y += TRIM_PIXELS;
        for (int x = 0; x < size_; x += TRIM_PIXELS) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }
        break;
    case texture_t::sky:
        draw_sky();
        break;
    default:
        // Building textures
        draw_windows();
        break;
    }

    glPopMatrix();

    // Now blit the finished image into our texture
    if (use_framebuffer) {
        glBindTexture(GL_TEXTURE_2D, glid_);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, size_, size_, 0);
    }

    if (mipmap_) {
        bits = (unsigned char *)malloc(size_ * size_ * 4);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
        gluBuild2DMipmaps(GL_TEXTURE_2D,
                          GL_RGBA,
                          size_,
                          size_,
                          GL_RGBA,
                          GL_UNSIGNED_BYTE,
                          bits);

        free(bits);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // Cleanup and restore the viewport
    render_resize();
    ready_ = true;
    lapsed = SDL_GetTicks() - start;
    build_time += lapsed;
}

unsigned int texture_id(texture_t id)
{
    for (Texture const &texture : textures) {
        if (texture.my_id_ == id) {
            return texture.glid_;
        }
    }

    return 0;
}

unsigned int texture_random_building(int index)
{
    index = abs(index) % BUILDING_COUNT;
    return texture_id(static_cast<texture_t>(static_cast<int>(texture_t::building1) + index));
}

void texture_reset()
{
    textures_done = false;
    build_time = 0;

    for (Texture &texture: textures) {
        texture.clear();
    }

    prefix_used.fill(false);
    name_used.fill(false);
    suffix_used.fill(false);
}

bool texture_ready()
{
    return textures_done;
}

void texture_update()
{
    if (textures_done) {
        if (!render_bloom()) {
            return;
        }

        for (Texture const &texture : textures) {
            if (texture.my_id_ == texture_t::bloom) {
                continue;
            }

            do_bloom(texture);
            return;
        }
    }

    for (Texture &texture : textures) {
        if (!texture.ready_) {
            texture.rebuild();

            return;
        }
    }

    textures_done = true;
}

void texture_term()
{
    textures.clear();
}

void texture_init()
{
    new Texture(texture_t::sky, 512, true, false, false);
    new Texture(texture_t::lattice, 128, true, true, true);
    new Texture(texture_t::light, 128, false, false, true);
    new Texture(texture_t::soft_circle, 128, false, false, true);
    new Texture(texture_t::headlight, 128, false, false, true);
    new Texture(texture_t::trim, TRIM_RESOLUTION, true, false, false);
    new Texture(texture_t::logos, LOGO_RESOLUTION, true, false, true);
    for (int i = static_cast<int>(texture_t::building1); i < static_cast<int>(texture_t::building9); ++i) {
        new Texture(static_cast<texture_t>(i), 512, true, false, false);
    }
    new Texture(texture_t::bloom, 512, true, false, false);
}
