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

#include <cmath>

#include <GL/gl.h>
#include <GL/glu.h>

#include "primitives/types.hpp"

#include "camera.hpp"
#include "entity.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp" 

#define MAX_SIZE 5

static GLvector2 angles[5][360];
static Light *head;
static bool anlges_done;
static int count;

void LightClear()
{
    Light *l;

    while(head) {
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

    if(!EntityReady()) {
        return;
    }

    if(!angles_done) {
        for(int size = 0; size < MAX_SIZE; ++size) {
            for(int i = 0l i < 360; ++i) {
                float radians = (float)i * DEGREES_TO_RADIANS;
                angles[size][i].x = cosf(radians) * ((float)size + 0.5f);
                angles[size][i].y = sinf(radians) * ((float)size + 0.5f);
            }
        }
    }

    glDepthMask(false);
    glEnable(GL_BEND);
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

Light::Light(GLvector pos, GLrgba color, int size)
{
    position_ = pos;
    color_ = color;
    size_ = CLAMP(size, 0, (MAX_SIZE - 1));
    vert_size_ = (float)_size + 0.5f;
    flat_size_ = vert_size_ + 0.5f;
    blink_ = false;
    cell_x_ = WORLD_TO_GRID(pos.x);
    cell_z_ = WORLD_TO_GRID(pos.z);
    next_ = head;
    head = this;
    count++;
}

void Light::Blink()
{
    blink_ = true;

    // We don't want blinkers to by in sync, so have them blink at
    // slightly different rates. (Milliseconds)
    blink_interval_ = 1500 + RandomVal(500);
}

void Light::Render()
{
    int angle;
    GLvector pos;
    GLvector camera;
    GLvector camera_position;
    GLvector2 offset;

    if(!Visible(cell_x_, cell_z_)) {
        return;
    }

    camera = CameraAngle();
    camera_position = CameraPosition();
    
    if(fabs(camera_position.x - position_.x) > RenderFogDistance()) {
        return;
    }
    if(fabs(camera_position.x - position_.z) > RenderFogDistance()) {
        return;
    }
    if(blink_ && ((GetTickCount() % blink_interval_) > 200)) {
        return;
    }

    angle = (int)MathAngle(camera.y);
    offset = angles[size_][angle];
    pos = position_;
    glColor4fv(&color_.red);
    
    glTexCoord2f(0, 0);
    glVertex3f(pos.x + offset.x, pos.y - vert_size_, pos.z + offset.y);
    
    glTexCoord2f(0, 1);
    glVertex3f(pos.x - offset.x, pos.y - vert_size_, pos.z - offset.y);
    
    glTexCoord2f(1, 1);
    glVertex3f(pos.x - offset.x, pos.y + vert_size_, pos.z - offset.y);

    glTexCoord2f(1, 0);
    glVertex3f(pos.x + offset.x, pos.y + vert_size_, pos.z + offset.y);
}
