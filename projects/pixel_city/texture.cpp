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

#include "building.hpp"
#include "camera.hpp"
#include "car.hpp"
#include "light.hpp"
#include "macro.hpp"
#include "random.hpp"
#include "render.hpp"
#include "sky.hpp"
#include "texture.hpp"
#include "win.hpp"
#include "world.hpp"

#define RANDOM_COLOR_SHIFT ((float)(random_val(10)) / 50.0f)
#define RANDOM_COLOR_VAL ((float)(random_val(256)) / 256.0f)
#define RANDOM_COLOR_LIGHT ((float)(200 + random_val(56)) / 256.0f)
#define SKY_BANDS (sizeof(sky_pos) / sizeof(int))
#define PREFIX_COUNT (sizeof(prefix) / sizeof(char *))
#define SUFFIX_COUNT (sizeof(suffix) / sizeof(char *))
#define NAME_COUNT (sizeof(name) / sizeof(char *))

static char const *prefix[] = {
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

static char const *name[] = {
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

static char const *suffix[] = {
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

class CTexture {
public:
    int my_id_;
    unsigned int glid_;
    int desired_size_;
    int size_;
    int half_;
    int segment_size_;
    bool ready_;
    bool masked_;
    bool mipmap_;
    bool clamp_;
    CTexture *next_;

    CTexture(int id, int size, bool mipmap, bool clamp, bool masked);
    void Clear();
    void Rebuild();
    void DrawWindows();
    void DrawSky();
    void DrawHeadlight();
};

void CTexture::Clear()
{
    ready_ = false;
}

static CTexture *head;
static bool textures_done;
static bool prefix_used[PREFIX_COUNT];
static bool name_used[NAME_COUNT];
static bool suffix_used[SUFFIX_COUNT];
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
    float average;
    float hue;
    int potential;
    int height;
    int i;
    int j;
    bool bright;
    gl_rgba color_noise;

    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glLineWidth(1.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3fv(color.get_rgb().data());

    // In low resolution, a "rect" might be 1 pixel wide
    if(left == right) {
        glBegin(GL_LINES);
        glVertex2i(left, top);
        glVertex2i(left, bottom);
        glEnd();
    }

    // In low resolution, a "rect" might be 1 pixel wide
    if(top == bottom) {
        glBegin(GL_LINES);
        glVertex2i(left, top);
        glVertex2i(right, top);
        glEnd();
    }
    else {
        // Draw one of those fancy 2-dimensional rectangles
        glBegin(GL_QUADS);
        glVertex2i(left, top);
        glVertex2i(right, top);
        glVertex2i(right, bottom);
        glVertex2i(left, bottom);
        glEnd();

        std::array<float, 3> vals = color.get_rgb();
        average = (vals.at(0) + vals.at(1) + vals.at(2)) / 3.0f;
        bright = (average > 0.5f);
        potential = (int)(average * 255.0f);

        if(bright) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_POINTS);
            for(i = left + 1; i < right - 1; ++i) {
                for(j = top + 1; j < bottom - 1; ++j) {
                    glColor4i(255, 0, random_val(potential), 255);
                    hue = 0.2f
                        + ((float)random_val(100) / 300.0f)
                        + ((float)random_val(100) / 300.0f)
                        + ((float)random_val(100) / 300.0f);

                    gl_rgba temp;
                    color_noise = temp.from_hsl(hue, 0.3f, 0.5f);
                    color_noise.set_alpha(((float)random_val(potential) / 144.0f) * 255);
                    glColor4f(RANDOM_COLOR_VAL,
                              RANDOM_COLOR_VAL,
                              RANDOM_COLOR_VAL,
                              (float)random_val(potential) / 144.0f);
                    glColor4fv(color_noise.get_rgba().data());
                    glVertex2i(i, j);
                }
            }
            glEnd();
        }

        height = (bottom - top) + (random_val(3) - 1) + (random_val(3) - 1);

        for(i = left; i < right; ++i) {
            if(random_val(6) == 0) {
                height = bottom - top;
                height = random_val(height);
                height = random_val(height);
                height = random_val(height);
                height = ((bottom - top) + height) / 2;
            }

            for(j = 0; j < 1; ++j) {
                glBegin(GL_LINES);
                glColor4f(0, 0, 0, (float)random_val(256) / 256.0f);
                glVertex2i(i, bottom - height);
                glColor4f(0, 0, 0, (float)random_val(256) / 256.0f);
                glVertex2i(i, bottom);
                glEnd();
            }
        }
    }
}

static void buildingWindow(int x, int y, int size, int id, gl_rgba color)
{
    int margin;
    int half;
    int i;

    margin = size / 3;
    half = size / 2;

    switch(id) {
    case TEXTURE_BUILDING1:
        // Filled, 1-pixel frame
        drawrect(x + 1, y + 1, x + size - 1, y + size - 1, color);
        break;
    case TEXTURE_BUILDING2:
        // Vertical
        drawrect(x + margin, y + 1, x + size - margin, y + size - 1, color);
        break;
    case TEXTURE_BUILDING3:
        // Side-by-side pair
        drawrect(x + 1, y + 1, x + half - 1, y + size - margin, color);
        drawrect(x + half + 1, y + 1, x + size - 1, y + size - margin, color);
        break;
    case TEXTURE_BUILDING4:
        // Windows with blinds
        drawrect(x + 1, y + 1, x + size - 1, y + size - 1, color);
        i = random_val(size - 2);
        drawrect(x + 1, y + 1, x + size - 1, y + i + 1, color * 0.3f);
        break;
    case TEXTURE_BUILDING5:
        // Vert stripes
        drawrect(x + 1, y + 1, x + size - 1, y + size - 1, color);
        drawrect(x + margin, y + 1, x + margin, y + size - 1, color * 0.7f);
        drawrect(x + size - margin - 1,
                 y + 1,
                 x + size - margin - 1,
                 y + size - 1,
                 color * 0.3f);
        break;
    case TEXTURE_BUILDING6:
        // Wide horz line
        drawrect(x + 1, y + 1, x + size - 1, y + size - margin, color);
        break;
    case TEXTURE_BUILDING7:
        // 4-pane
        drawrect(x + 2, y + 1, x + size - 1, y + size - 1, color);
        drawrect(x + 2, y + half, x + size - 1, y + half, color * 0.2f);
        drawrect(x + half, y + 1, x + half, y + size - 1, color * 0.2f);
        break;
    case TEXTURE_BUILDING8:
        // Single narrow window
        drawrect(x + half - 1, y + 1, x + half + 1, y + size - margin, color);
        break;
    case TEXTURE_BUILDING9:
        // Horizontal
        drawrect(x + 1, y + margin, x + size - 1, y + size - margin - 1, color);
        break;
    }
}

static void do_bloom(CTexture *t)
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, t->size_, t->size_);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FOG);
    glFogf(GL_FOG_START, RenderFogDistance() / 2);
    glFogf(GL_FOG_END, RenderFogDistance());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    entity_render();
    car_render();
    LightRender();
    glBindTexture(GL_TEXTURE_2D, t->glid_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, t->size_, t->size_, 0);
}

CTexture::CTexture(int id, int size, bool mipmap, bool clamp, bool masked)
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
    next_ = head;
    head = this;
}

// This draws all of the windows on a building texture. lit_density controls
// how many lights are on. (1 in n chance that the light is on. Higher values
// mean less lit windows). run_length controls how often it will consider
// changing the lit/unlit status. 1 produces a complete scatter, higher
// numbers make long strings of lights
void CTexture::DrawWindows()
{
    int x;
    int y;
    int run;
    int run_length;
    int lit_density;
    gl_rgba color;
    bool lit;

    // color = glRgbaUnique(_my_id);
    for(y = 0; y < SEGMENTS_PER_TEXTURE; ++y) {
        // Every few floors we change the behavior
        if(!(y % 8)) {
            run = 0;
            run_length = random_val(9) + 2;
            lit_density = 2 + random_val(2) + random_val(2);
            lit = false;
        }

        for(x = 0; x < SEGMENTS_PER_TEXTURE; ++x) {
            // If this run is over reroll lit and start a new one
            if(run < 1) {
                run = random_val(run_length);
                lit = (random_val(lit_density) == 0);
                // if(lit) {
                //     color = glRgba(0.5f + (float)(random_val() % 128) / 256.0f)
                //         + glRgba(RANDOM_COLOR_SHIFT,
                //                  RANDOM_COLOR_SHIFT,
                //                  RANDOM_COLOR_SHIFT);
                // }
            }

            if(lit) {
                int rand_val = random_val() % 128;
                color = gl_rgba(127 + rand_val, 127 + rand_val, 127 + rand_val)
                    + gl_rgba(RANDOM_COLOR_SHIFT,
                             RANDOM_COLOR_SHIFT,
                             RANDOM_COLOR_SHIFT);
            }
            else {
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

void CTexture::DrawSky()
{
    gl_rgba color;
    float scale;
    float inv_scale;
    int i;
    int x;
    int y;
    int width;
    int height;
    int offset;
    int width_adjust;
    int height_adjust;

    color = WorldBloomColor();

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
    for(i = 0; i < size_; i += 5) {
        drawrect(i,
                 size_ - random_val(8) - random_val(8) - random_val(8),
                 i + random_val(9),
                 size_,
                 gl_rgba(0, 0, 0));
    }

    // Draw the clouds
    for(i = size_ - 30; i > 5; i -= 2) {
        x = random_val(size_);
        y = i;

        scale = 1.0f - ((float)y / (float)size_);
        width = random_val(half_ / 2) + (int)((float)half_ * scale) / 2;
        scale = 1.0f - ((float)y / (float)size_);
        height = (int)((float)width * scale);
        height = MAX(height, 4);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureId(TEXTURE_SOFT_CIRCLE));
        glDepthMask(false);
        glBegin(GL_QUADS);
        for(offset = -size_; offset <= size_; offset += size_) {
            for(scale = 1.0f; scale > 0.0f; scale -= 0.25f) {
                inv_scale = 1.0f - scale;
                if(scale < 0.4f) {
                    color = WorldBloomColor() * 0.1f;
                }
                else {
                    color = gl_rgba(0, 0, 0);
                }

                color.set_alpha(51);
                glColor4fv(color.get_rgba().data());
                width_adjust =
                    (int)(((float)width / 2.0f)
                          + (int)(inv_scale * ((float)width / 2.0f)));

                height_adjust = height + (int)(scale * (float)height * 0.99f);

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

void CTexture::DrawHeadlight()
{
    float radius;
    int i;
    int x;
    int y;
    gl_vector2 pos;

    // Make a simple circle of light, bright in the center and fading out
    radius = ((float)half_) - 20;
    x = half_ - 20;
    y = half_;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.8f, 0.8f, 0.8f, 0.6f);
    glVertex2i(half_ - 5, y);
    glColor4f(0, 0, 0, 0);
    for(i = 0; i <= 360; i += 36) {
        pos.set_x(sinf((float)(i % 360) * DEGREES_TO_RADIANS) * radius);
        pos.set_y(cosf((float)(i % 360) * DEGREES_TO_RADIANS) * radius);
        glVertex2i(x + (int)pos.get_x(), half_ + (int)pos.get_y());
    }
    glEnd();

    x = half_ + 20;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.8f, 0.8f, 0.8f, 0.6f);
    glVertex2i(half_ + 5, y);
    glColor4f(0, 0, 0, 0);
    for(i = 0; i <= 360; i += 36) {
        pos.set_x(sinf((float)(i % 360) * DEGREES_TO_RADIANS) * radius);
        pos.set_y(cosf((float)(i % 360) * DEGREES_TO_RADIANS) * radius);
        glVertex2i(x + (int)pos.get_x(), half_ + (int)pos.get_y());
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
void CTexture::Rebuild()
{
    int i;
    int j;
    int x;
    int y;
    int name_num;
    int prefix_num;
    int suffix_num;
    int max_size;
    float radius;
    gl_vector2 pos;
    bool use_framebuffer;
    unsigned char *bits;
    unsigned int start;
    int lapsed;

    start = SDL_GetTicks();

    // Since we make textures by drawing into the viewport, we can't make
    // them bigger than the current view.
    size_ = desired_size_;
    max_size = RenderMaxTextureSize();
    while(size_ > max_size) {
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
    if(clamp_) {
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
    use_framebuffer = true;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    switch(my_id_) {
    case TEXTURE_LATTICE:
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
        for(i = 0; i < size_; i += 9) {
            if(i % 2) {
                glVertex2i(0, i);
            }
            else {
                glVertex2i(i, i);
            }
        }

        for(i = 0; i < size_; i += 9) {
            if(i % 2) {
                glVertex2i(i, 0);
            }
            else {
                glVertex2i(i, i);
            }
        }
        glEnd();
        break;
    case TEXTURE_SOFT_CIRCLE:
        // Make a simple circle of light, bright in the center and fading out
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        radius = (float)half_ - 3;
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1, 1, 1, 1);
        glVertex2i(half_, half_);
        glColor4f(0, 0, 0, 0);
        for(i = 0; i <= 360; ++i) {
            pos.set_x(sinf((float)i * DEGREES_TO_RADIANS) * radius);
            pos.set_y(cosf((float)i * DEGREES_TO_RADIANS) * radius);
            glVertex2i(half_ + (int)pos.get_x(), half_ + (int)pos.get_y());
        }
        glEnd();
        break;
    case TEXTURE_LIGHT:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        radius = (float)half_ - 3;
        for(j = 0; j < 2; ++j) {
            glBegin(GL_TRIANGLE_FAN);
            glColor4f(1, 1, 1, 1);
            glVertex2i(half_, half_);
            if(!j) {
                radius = (float)half_ / 2;
            }
            else {
                radius = 8;
            }

            glColor4f(1, 1, 1, 0);
            for(i = 0; i <= 360; ++i) {
                pos.set_x(sinf((float)i * DEGREES_TO_RADIANS) * radius);
                pos.set_y(cosf((float)i * DEGREES_TO_RADIANS) * radius);
                glVertex2i(half_ + (int)pos.get_x(), half_ + (int)pos.get_y());
            }
            glEnd();
        }
        break;
    case TEXTURE_HEADLIGHT:
        DrawHeadlight();
        break;
    case TEXTURE_LOGOS:
        i = 0;
        glDepthMask(false);
        glDisable(GL_BLEND);
        name_num = random_val(NAME_COUNT);
        prefix_num = random_val(PREFIX_COUNT);
        suffix_num = random_val(SUFFIX_COUNT);
        glColor3f(1, 1, 1);

        while(i < size_) {
            // Randomly use a prefix OR suffix, but not both. Too verbose.
            if(COIN_FLIP) {
                RenderPrint(2,
                            size_ - i - (LOGO_PIXELS / 4),
                            random_val(),
                            gl_rgba(255, 255, 255),
                            "%s%s",
                            prefix[prefix_num],
                            name[name_num]);
            }
            else {
                RenderPrint(2,
                            size_ - i - (LOGO_PIXELS / 4),
                            random_val(),
                            gl_rgba(255, 255, 255),
                            "%s%s",
                            name[name_num],
                            suffix[suffix_num]);
            }

            name_num = (name_num + 1) % NAME_COUNT;
            prefix_num = (prefix_num + 1) % PREFIX_COUNT;
            suffix_num = (suffix_num + 1) % SUFFIX_COUNT;
        }
        break;
    case TEXTURE_TRIM:
        int margin;
        y = 0;
        margin = MAX(TRIM_PIXELS / 4, 1);

        for(x = 0; x < size_; x += TRIM_PIXELS) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }

        y += TRIM_PIXELS;
        for(x = 0; x < size_; x += TRIM_PIXELS * 2) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }

        y += TRIM_PIXELS;
        for(x = 0; x < size_; x += TRIM_PIXELS * 3) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }

        y += TRIM_PIXELS;
        for(x = 0; x < size_; x += TRIM_PIXELS) {
            drawrect_simple(x + margin,
                            y + margin,
                            x + TRIM_PIXELS - margin,
                            y + TRIM_PIXELS - margin,
                            gl_rgba(255, 255, 255),
                            gl_rgba(127, 127, 127));
        }
        break;
    case TEXTURE_SKY:
        DrawSky();
        break;
    default:
        // Building textures
        DrawWindows();
        break;
    }

    glPopMatrix();

    // Now blit the finished image into our texture
    if(use_framebuffer) {
        glBindTexture(GL_TEXTURE_2D, glid_);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, size_, size_, 0);
    }

    if(mipmap_) {
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
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // Cleanup and restore the viewport
    RenderResize();
    ready_ = true;
    lapsed = SDL_GetTicks() - start;
    build_time += lapsed;
}

unsigned int TextureId(int id)
{
    for(CTexture *t = head; t; t = t->next_) {
        if(t->my_id_ == id) {
            return t->glid_;
        }
    }

    return 0;
}

unsigned int TextureRandomBuilding(int index)
{
    index = abs(index) % BUILDING_COUNT;
    return TextureId(TEXTURE_BUILDING1 + index);
}

void TextureReset()
{
    textures_done = false;
    build_time = 0;
    for(CTexture *t = head; t; t = t->next_) {
        t->Clear();
    }

    memset(prefix_used, '0', sizeof(prefix_used));
    memset(name_used, '0', sizeof(name_used));
    memset(suffix_used, '0', sizeof(suffix_used));
}

bool TextureReady()
{
    return textures_done;
}

void TextureUpdate()
{
    if(textures_done) {
        if(!RenderBloom()) {
            return;
        }

        CTexture *t;
        for(t = head; t; t = t->next_) {
            if(t->my_id_ != TEXTURE_BLOOM) {
                continue;
            }

            do_bloom(t);
            return;
        }
    }

    for(CTexture *t = head; t; t = t->next_) {
        if(!t->ready_) {
            t->Rebuild();
            return;
        }
    }

    textures_done = true;
}

void TextureTerm()
{
    CTexture *t;

    while(head) {
        t = head->next_;
        free(head);
        head = t;
    }
}

void TextureInit()
{
    new CTexture(TEXTURE_SKY, 512, true, false, false);
    new CTexture(TEXTURE_LATTICE, 128, true, true, true);
    new CTexture(TEXTURE_LIGHT, 128, false, false, true);
    new CTexture(TEXTURE_SOFT_CIRCLE, 128, false, false, true);
    new CTexture(TEXTURE_HEADLIGHT, 128, false, false, true);
    new CTexture(TEXTURE_TRIM, TRIM_RESOLUTION, true, false, false);
    new CTexture(TEXTURE_LOGOS, LOGO_RESOLUTION, true, false, true);
    for(int i = TEXTURE_BUILDING1; i < TEXTURE_BUILDING9; ++i) {
        new CTexture(i, 512, true, false, false);
    }
    new CTexture(TEXTURE_BLOOM, 512, true, false, false);
}
