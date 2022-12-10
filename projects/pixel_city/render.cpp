/*
 * render.cpp
 *
 * 2009 Shamus Young
 *
 * This is the core of the gl rendering functions. This contains the main
 * rendering function RenderUpdate(), which initiates the various other
 * renders in the other modules.
 *
 */

#include "render.hpp"

#include <SDL2/SDL.h>
#include <GL/glu.h>
#include <array>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "camera.hpp"
#include "car.hpp"
#include "entity.hpp"
#include "gl-font.hpp"
#include "ini.hpp"
#include "light.hpp"
#include "math.hpp"
#include "sky.hpp"
#include "texture.hpp"
#include "win.hpp"
#include "world.hpp"

static int constexpr RENDER_DISTANCE = 1280;
static int constexpr MAX_TEXT = 256;

static int constexpr COLOR_CYCLE_TIME = 10000; // Milliseconds
static int constexpr COLOR_CYCLE = COLOR_CYCLE_TIME / 4;
static int constexpr FONT_SIZE = LOGO_PIXELS - (LOGO_PIXELS / 8);
static float constexpr BLOOM_SCALING = 0.07f;

std::array<std::string, 9> help = {"ESC - Exit!",
    "F1  - Show this help screen",
    "R   - Rebuild city",
    "L   - Toggle 'letterbox' mode",
    "F   - Show Framecounter",
    "W   - Toggle Wireframe",
    "E   - Change full-scene effects",
    "T   - Toggle Textures",
    "G   - Toggle Fog"};

enum class effect_t {
  none = 0,
  bloom,
  count,
  debug_overbloom,
  debug,
  bloom_radial,
  color_cycle,
  glass_city,
};

effect_t operator++(effect_t &effect, int val)
{
    return static_cast<effect_t>((static_cast<int>(effect) + 1) % static_cast<int>(effect_t::count));
}

static std::string const render_section_ini("RENDER");
static std::string const set_defaults_ini("set defaults");
static std::string const effect_ini("effect");
static std::string const show_fog_ini("show fog");
static std::string const letterbox_ini("letterbox");
static std::string const wireframe_ini("wireframe");
static std::string const show_fps_ini("show fps");
static std::string const flat_ini("flat");

static float render_aspect;
static float fog_distance;
static int render_width;
static int render_height;
static bool letterbox;
static int letterbox_offset;
static effect_t effect;
static unsigned int current_fps;
static unsigned int frames;
static bool show_wireframe;
static bool flat;
static bool show_fps;
static bool show_fog;
static bool show_help;

// Draw a clock-ish progress...widget...thing. It's cute.
static void do_progress(float center_x,
                        float center_y,
                        float radius,
                        float opacity,
                        float progress)
{
    float angle;
    float s;
    float c;

    // Outer ring
    float gap = radius * 0.05f;
    float outer = radius;
    float inner = radius - (gap * 2);
    glColor4f(1, 1, 1, opacity);
    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= 360; i += 15) {
        angle = i * DEGREES_TO_RADIANS;
        s = sinf(angle);
        c = -cosf(angle);
        glVertex2f(center_x + (s * outer), center_y + (c * outer));
        glVertex2f(center_x + (s * inner), center_y + (c * inner));
    }

    glEnd();

    // Progress indicator
    glColor4f(1, 1, 1, opacity);
    int end_angle = (360 * progress);
    outer = radius - (gap * 3);
    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(center_x, center_y);
    for (int i = 0; i < end_angle; i += 3) {
        angle = i * DEGREES_TO_RADIANS;
        s = sinf(angle);
        c = -cosf(angle);
        glVertex2f(center_x + (s * outer), center_y + (c * outer));
    }

    glEnd();

    // Tick lines
    glLineWidth(2.0f);
    outer = radius - (gap * 1);
    inner = radius - (gap * 2);
    glColor4f(0, 0, 0, opacity);
    glBegin(GL_LINES);

    for (int i = 0; i <= 360; i += 15) {
        angle = i * DEGREES_TO_RADIANS;
        s = sinf(angle);
        c = -cosf(angle);
        glVertex2f(center_x + (s * outer), center_y + (c * outer));
        glVertex2f(center_x + (s * inner), center_y + (c * inner));
    }

    glEnd();
}

static void do_effects(effect_t type)
{
    float hue1;
    float hue2;
    float hue3;
    float hue4;
    gl_rgba color;
    gl_rgba temp;
    int radius;
    float offset;

    float fade = world_fade();
    int bloom_radius = 15;
    int bloom_step = bloom_radius / 3;

    if (!texture_ready()) {
        return;
    }

    // No change projection modes so we can render full-screen effects
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, render_width, render_height, 0, 0.1f, 2048);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, -1.0f);
    glDisable(GL_CULL_FACE);
    glDisable(GL_FOG);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render full-screen effects
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glBindTexture(GL_TEXTURE_2D, texture_id(texture_t::bloom));

    switch(type) {
    case effect_t::debug:
        glBindTexture(GL_TEXTURE_2D, texture_id(texture_t::logos));
        glDisable(GL_BLEND);
        glBegin(GL_QUADS);
        glColor3f(1, 1, 1);

        glTexCoord2f(0, 0);
        glVertex2i(0, render_height / 4);

        glTexCoord2f(0, 1);
        glVertex2i(0, 0);

        glTexCoord2f(1, 1);
        glVertex2i(render_width / 4, 0);

        glTexCoord2f(1, 0);
        glVertex2i(render_width / 4, render_height / 4);

        glTexCoord2f(0, 0);
        glVertex2i(0, 512);

        glTexCoord2f(0, 1);
        glVertex2i(0, 0);

        glTexCoord2f(1, 1);
        glVertex2i(512, 0);

        glTexCoord2f(1, 0);
        glVertex2i(512, 512);

        glEnd();
        break;
    case effect_t::bloom_radial:
        // Psychedelic bloom
        glEnable(GL_BLEND);
        glBegin(GL_QUADS);
        color = (world_bloom_color() * BLOOM_SCALING) * 2;
        glColor3fv(color.get_rgb().data());
        for (int i = 0; i <= 100; i += 10) {
            glTexCoord2f(0, 0);
            glVertex2i(-i, i + render_height);

            glTexCoord2f(0, 1);
            glVertex2i(-i, -i);

            glTexCoord2f(1, 1);
            glVertex2i(i + render_width, -i);

            glTexCoord2f(1, 0);
            glVertex2i(i + render_width, i + render_width);
        }

        glEnd();
        break;
    case effect_t::color_cycle:
        // Oooh. Pretty colors. Tint the scene according to the screenspace
        hue1 = (SDL_GetTicks() % COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;

        offset = SDL_GetTicks() + COLOR_CYCLE;
        hue2 = fmod(offset, COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;
        hue3 = fmod(offset * 2, COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;
        hue4 = fmod(offset * 3, COLOR_CYCLE_TIME) / COLOR_CYCLE_TIME;

        glBindTexture(GL_TEXTURE_2D, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
        glBegin(GL_QUADS);

        color = from_hsl(hue1, 1.0f, 0.6f);
        glColor3fv(color.get_rgb().data());
        glTexCoord2f(0, 0);
        glVertex2i(0, render_height);

        color = from_hsl(hue2, 1.0f, 0.6f);
        glColor3fv(color.get_rgb().data());
        glTexCoord2f(0, 1);
        glVertex2i(0, 0);

        color = from_hsl(hue3, 1.0f, 0.6f);
        glColor3fv(color.get_rgb().data());
        glTexCoord2f(1, 1);
        glVertex2i(render_width, 0);

        color = from_hsl(hue4, 1.0f, 0.6f);
        glColor3fv(color.get_rgb().data());
        glTexCoord2f(1, 0);
        glVertex2i(render_width, render_height);

        glEnd();

        break;
    case effect_t::bloom:
        // Simple bloom effect
        glBegin(GL_QUADS);

        color = world_bloom_color() * BLOOM_SCALING;
        glColor3fv(color.get_rgb().data());
        for (int x = -bloom_radius; x <= bloom_radius; x += bloom_step) {
            for (int y = -bloom_radius; y <= bloom_radius; y += bloom_step) {
                if ((abs(x) == abs(y)) && x) {
                    continue;
                }

                glTexCoord2f(0, 0);
                glVertex2i(x, y + render_height);

                glTexCoord2f(0, 1);
                glVertex2i(x, y);

                glTexCoord2f(1, 1);
                glVertex2i(x + render_width, y);

                glTexCoord2f(1, 0);
                glVertex2i(x + render_width, y + render_height);
            }
        }

        glEnd();
        break;
    case effect_t::debug_overbloom:
        // This will punish that uppity GPU. Good for testing
        // low frame rate behavior.
        glBegin(GL_QUADS);

        color = world_bloom_color() / 100;
        glColor3fv(color.get_rgb().data());
        for (int x = -50; x <= 50; x += 5) {
            for (int y = -50; y <= 50; y += 5) {
                glTexCoord2f(0, 0);
                glVertex2i(x, y + render_height);

                glTexCoord2f(0, 1);
                glVertex2i(x, y);

                glTexCoord2f(1, 1);
                glVertex2i(x + render_width, y);

                glTexCoord2f(1, 0);
                glVertex2i(x + render_width, y + render_height);
            }
        }

        glEnd();
        break;
    default:
        break;
    }

    // Do the fade to/from darkness used to hide screen transitions
    if (LOADING_SCREEN) {
        if (fade > 0.0f) {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
            glColor4f(0, 0, 0, fade);
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, render_height);
            glVertex2i(render_width, render_height);
            glVertex2i(render_width, 0);
            glEnd();
        }

        if (texture_ready() && !entity_ready() && (fade != 0.0f)) {
            radius = render_width / 16;
            do_progress(render_width / 2,
                        render_height / 2,
                        radius,
                        fade,
                        entity_progress());

            render_print((render_width / 2) - LOGO_PIXELS,
                        (render_height / 2) + LOGO_PIXELS,
                        0,
                        gl_rgba(127, 127, 127),
                        "%1.2f%%",
                        entity_progress() * 100.0f);

            render_print(1,
                        "%s v%d.%d.%03d",
                        APP_TITLE.c_str(),
                        VERSION_MAJOR,
                        VERSION_MINOR,
                        VERSION_REVISION);
        }
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

int render_max_texture_size()
{
    int mts;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mts);
    mts = std::min(mts, render_width);

    return std::min(mts, render_height);
}

void render_print(int x, int y, int font, gl_rgba color, const char *fmt, ...)
{
    char text[MAX_TEXT];
    va_list ap;

    text[0] = 0;

    if (fmt == NULL) {
        return;
    }

    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    std::string curier_new("Courier New");
    std::string arial("Arial");
    std::string times_new_roman("Times New Roman");
    std::string arial_black("Arial Black");
    std::string impact("Impact");
    std::string agency_fb("Agency FB");
    std::string book_antiqua("Book Antiqua");

    std::array<gl_font, 7> fonts = {
        gl_font{curier_new, 0},
        gl_font{arial, 0},
        gl_font{times_new_roman, 0},
        gl_font{arial_black, 0},
        gl_font{impact, 0},
        gl_font{agency_fb, 0},
        gl_font{book_antiqua, 0},
    };

    glPushAttrib(GL_LIST_BIT);
    glListBase(fonts[font % fonts.size()].get_base_char() - 32);
    glColor3fv(color.get_rgb().data());
    glRasterPos2i(x, y);
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
    glPopAttrib();

    /* FIXME */
    // char *ptr = text;
    // while(*ptr) {
    //     glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *ptr++);
    // }
}

void render_print(int line, const char *fmt, ...)
{
    char text[MAX_TEXT];
    va_list ap;

    text[0] = 0;

    if (fmt == NULL) {
        return;
    }

    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, render_width, render_height, 0, 0.1f, 2048);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, -1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    render_print(0, (line * FONT_SIZE) - 2, 0, gl_rgba(0, 0, 0), text);
    render_print(4, (line * FONT_SIZE) + 2, 0, gl_rgba(0, 0, 0), text);
    render_print(2, line * FONT_SIZE, 0, gl_rgba(255, 255, 255), text);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void static do_help()
{
    int line = 0;
    for (std::string const &text : help) {
        render_print(line + 2, text.c_str());
        line++;
    }
}

void do_fps()
{
    static long next_update;

    if (next_update > SDL_GetTicks()) {
        return;
    }

    next_update = SDL_GetTicks() + 1000;
    current_fps = frames;
    frames = 0;
}

void render_resize()
{
    float fovy = 60.0f;

    SDL_GetWindowSize(window, &render_width, &render_height);
    if (letterbox) {
        letterbox_offset = render_height / 6;
        render_height = render_height - (letterbox_offset * 2);
    } else {
        letterbox_offset = 0;
    }

    render_aspect = render_height / render_width;
    if (render_aspect > 1.0f) {
        fovy /= render_aspect;
    }

    glViewport(0, letterbox_offset, render_width, render_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, render_aspect, 0.1f, RENDER_DISTANCE);
    glMatrixMode(GL_MODELVIEW);
}

void render_term()
{
}

void render_init()
{
    // If the program is running for the first time, set the defaults.
    if (!ini_int(render_section_ini, set_defaults_ini)) {
        ini_int_set(render_section_ini, set_defaults_ini, 1);
        ini_int_set(render_section_ini, effect_ini, static_cast<int>(effect_t::bloom));
        ini_int_set(render_section_ini, show_fog_ini, 1);
    }

    // Load in our settings
    letterbox = (ini_int(render_section_ini, letterbox_ini) != 0);
    show_wireframe = (ini_int(render_section_ini, wireframe_ini) != 0);
    show_fps = (ini_int(render_section_ini, show_fps_ini) != 0);
    show_fog = (ini_int(render_section_ini, show_fog_ini) != 0);
    effect = static_cast<effect_t>(ini_int(render_section_ini, effect_ini));
    flat = (ini_int(render_section_ini, flat_ini) != 0);
    fog_distance = WORLD_HALF;

    // Clear the viewport so the user isn't looking at trash
    // while the program starts
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
    render_resize();
}

void render_fps_toggle()
{
    show_fps = !show_fps;
    ini_int_set(render_section_ini, show_fps_ini, show_fps ? 1 : 0);
}


bool render_fog()
{
    return show_fog;
}

void render_fog_toggle()
{
    show_fog = !show_fog;
    ini_int_set(render_section_ini, show_fog_ini, show_fog ? 1 : 0);
}

void RenderLetterBoxToggle()
{
    letterbox = !letterbox;
    ini_int_set(render_section_ini, letterbox_ini, letterbox ? 1: 0);
    render_resize();
}

void render_wireframe_toggle()
{
    show_wireframe = !show_wireframe;
    ini_int_set(render_section_ini, wireframe_ini, show_wireframe ? 1 : 0);
}

bool render_wireframe()
{
    return show_wireframe;
}

void render_effect_cycle()
{
    effect++;
    ini_int_set(render_section_ini, effect_ini, static_cast<int>(effect));
}

bool render_bloom()
{
    return ((effect == effect_t::bloom)
            || (effect == effect_t::bloom_radial)
            || (effect == effect_t::debug_overbloom)
            || (effect == effect_t::color_cycle));
}

bool render_flat()
{
    return flat;
}

void render_flat_toggle()
{
    flat = !flat;
    ini_int_set(render_section_ini, flat_ini, flat ? 1 : 0);
}

void render_help_toggle()
{
    show_help = !show_help;
}

float render_fog_distance()
{
    return fog_distance;
}

// This is used to set a gradient fog that goes from camera to some portion
// of the normal fog distance. This is used for making wireframe outlines and
// flat surfaces fade out after rebuild. Looks cool.
void render_fog_fx(float scalar)
{
    if (scalar >= 1.0f) {
        glDisable(GL_FOG);
        return;
    }

    glFogf(GL_FOG_START, 0.0f);
    glFogf(GL_FOG_END, (fog_distance * 2.0f) * scalar);
    glEnable(GL_FOG);
}

void render_update()
{
    gl_vector3 pos;
    gl_vector3 angle;
    gl_rgba color;
    int elapsed;

    frames++;
    do_fps();

    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glDepthMask(true);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (letterbox) {
        glViewport(0, letterbox_offset, render_width, render_height);
    }

    if (LOADING_SCREEN && texture_ready() && !entity_ready()) {
        do_effects(effect_t::none);
        SDL_GL_SwapWindow(window);

        return;
    }

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glLoadIdentity();
    glLineWidth(1.0f);
    pos = camera_position();
    angle = camera_angle();
    glRotatef(angle.get_x(), 1.0f, 0.0f, 0.0f);
    glRotatef(angle.get_y(), 0.0f, 1.0f, 0.0f);
    glRotatef(angle.get_z(), 0.0f, 0.0f, 1.0f);
    glTranslatef(-pos.get_x(), -pos.get_y(), -pos.get_z());
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render all the stuff in the whole entire world.
    glDisable(GL_FOG);
    sky_render();
    if (show_fog) {
        glEnable(GL_FOG);
        glFogf(GL_FOG_START, fog_distance - 100);
        glFogf(GL_FOG_END, fog_distance);
        color = gl_rgba(0, 0, 0);
        glFogfv(GL_FOG_COLOR, color.get_rgb().data());
    }

    world_render();

    if (effect == effect_t::glass_city) {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthFunc(false);
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_TEXTURE);
        glTranslatef((pos.get_x() + pos.get_z()) / SEGMENTS_PER_TEXTURE, 0, 0);
        glMatrixMode(GL_MODELVIEW);
    } else {
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }

    entity_render();

    if (!LOADING_SCREEN) {
        elapsed = 3000 - world_scene_elapsed();

        if ((elapsed >= 0) && (elapsed <= 3000)) {
            render_fog_fx((float)elapsed / 3000.0f);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);
            entity_render();
        }
    }

    if (entity_ready()) {
        light_render();
    }

    car_render();

    if (show_wireframe) {
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        entity_render();
    }

    do_effects(effect);

    // Framerate tracker
    if (show_fps) {
        render_print(1,
                    "FPS=%d : Entities=%d : polys=%d",
                    current_fps,
                    entity_count() + light_count() + car_count(),
                    entity_poly_count() + light_count() + car_count());
    }

    // Show the help overlay
    if (show_help) {
        do_help();
    }

    SDL_GL_SwapWindow(window);
}
