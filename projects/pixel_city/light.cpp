/*
 * light.cpp
 *
 * 2006 Shamus Young
 *
 * This tracks and renders the light sources. (Note that they do not really
 * CAST light in the OpenGL sense of the world, these are just simple panels.)
 * These are NOT subclassed to entities because these are dynamic. Some lights
 * blink, and thus they can't go into the fixed render lists managed by
 * Entity.cpp.
 *
 */

#include "light.hpp"

#include <SDL2/SDL.h>
#include <array>
#include <cmath>

#include "camera.hpp"
#include "entity.hpp"
#include "gl-vector2.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"

#define MAX_SIZE 5

static gl_vector2 angles[5][360];
static Light *head;
static bool angles_done;
static int count;

void LightClear()
{
    Light *l;

    while (head) {
        l = head;
        head = l->next_;
        delete l;
    }

    count = 0;
}

int LightCount()
{
    return count;
}

void LightRender()
{
    Light *l;

    if(!entity_ready()) {
        return;
    }

    if(!angles_done) {
        for(int size = 0; size < MAX_SIZE; ++size) {
            for(int i = 0; i < 360; ++i) {
                float radians = (float)i * DEGREES_TO_RADIANS;
                angles[size][i].set_x(cosf(radians) * ((float)size + 0.5f));
                angles[size][i].set_y(sinf(radians) * ((float)size + 0.5f));
            }
        }
    }

    glDepthMask(false);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, TextureId(TEXTURE_LIGHT));
    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);

    for(l = head; l; l = l->next_) {
        l->Render();
    }

    glEnd();
    glDepthMask(true);
}

Light::Light(gl_vector3 pos, gl_rgba color, int size)
{
    position_ = pos;
    color_ = color;
    size_ = CLAMP(size, 0, (MAX_SIZE - 1));
    vert_size_ = (float)size_ + 0.5f;
    flat_size_ = vert_size_ + 0.5f;
    blink_ = false;
    cell_x_ = pos.get_x() / GRID_RESOLUTION;
    cell_z_ = pos.get_z() / GRID_RESOLUTION;
    next_ = head;
    head = this;
    count++;
}

void Light::Blink()
{
    blink_ = true;

    // We don't want blinkers to by in sync, so have them blink at
    // slightly different rates. (Milliseconds)
    blink_interval_ = 1500 + random_val(500);
}

void Light::Render()
{
    int angle;
    gl_vector3 pos;
    gl_vector3 camera;
    gl_vector3 camera_pos;
    gl_vector2 offset;

    if(!visible(cell_x_, cell_z_)) {
        return;
    }

    camera = camera_angle();
    camera_pos = camera_position();

    if(fabs(camera_pos.get_x() - position_.get_x()) > RenderFogDistance()) {
        return;
    }
    if(fabs(camera_pos.get_x() - position_.get_z()) > RenderFogDistance()) {
        return;
    }
    if(blink_ && ((SDL_GetTicks() % blink_interval_) > 200)) {
        return;
    }

    angle = (int)MathAngle(camera.get_y());
    offset = angles[size_][angle];
    pos = position_;

    glColor4fv(color_.get_rgba().data());

    glTexCoord2f(0, 0);
    glVertex3f(pos.get_x() + offset.get_x(),
               pos.get_y() - vert_size_,
               pos.get_z() + offset.get_y());

    glTexCoord2f(0, 1);
    glVertex3f(pos.get_x() - offset.get_x(),
               pos.get_y() - vert_size_,
               pos.get_z() - offset.get_y());

    glTexCoord2f(1, 1);
    glVertex3f(pos.get_x() - offset.get_x(),
               pos.get_y() + vert_size_,
               pos.get_z() - offset.get_y());

    glTexCoord2f(1, 0);
    glVertex3f(pos.get_x() + offset.get_x(),
               pos.get_y() + vert_size_,
               pos.get_z() + offset.get_y());
}
