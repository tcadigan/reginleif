/*
 * car.cpp
 *
 * 2009 Shamus Young
 *
 * This creates the little two-triangle cars and moves them around the map.
 *
 */

#include "car.hpp"

#include <SDL.h>
#include <cmath>

#include <GL/gl.h>
#include <GL/glu.h>

#include "types.hpp"

#include "building.hpp"
#include "camera.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

#define DEAD_ZONE 200
#define STUCK_TIME 230
#define UPDATE_INTERVAL 50 // milliseconds
#define MOVEMENT_SPEED 0.61f
#define CAR_SIZE 3.0f

static GLvector direction[] = {
    {  0.0f, 0.0f, -1.0f },
    {  1.0f, 0.0f,  0.0f },
    {  0.0f, 0.0f,  1.0f },
    { -1.0f, 0.0f,  0.0f },
};

static int dangles[] = { 0, 90, 180, 270 };

static GLvector2 angles[360];
static bool angles_done;
static unsigned char carmap[WORLD_SIZE][WORLD_SIZE];
static Car *head;
static unsigned next_update;
static int count;

int CarCount()
{
    return count;
}

void CarClear()
{
    Car *c;
    
    for(c = head; c; c = c->next_) {
        c->Park();
    }

    memset(carmap, '\0', sizeof(carmap));
    count = 0;
}

void CarRender()
{
    Car *c;

    if(!angles_done) {
        for(int i = 0; i < 360; ++i) {
            angles[i].x = (float)cos((float)i * DEGREES_TO_RADIANS) * CAR_SIZE;
            angles[i].y = (float)sin((float)i * DEGREES_TO_RADIANS) * CAR_SIZE;
        }
    }

    glDepthMask(false);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, TextureId(TEXTURE_HEADLIGHT));
    for(c = head; c; c = c->next_) {
        c->Render();
    }

    glDepthMask(true);
}

void CarUpdate()
{
    Car *c;
    unsigned int now;

    if(!TextureReady() || !EntityReady()) {
        return;
    }

    now = SDL_GetTicks();
    
    if(next_update > now) {
        return;
    }

    next_update = now + UPDATE_INTERVAL;
    
    for(c = head; c; c = c->next_) {
        c->Update();
    }
}

Car::Car()
{
    ready_ = false;
    next_ = head;
    head = this;
    count++;
}

bool Car::TestPosition(int row, int col)
{
    // Test the given position and see if it's already occupied
    if(carmap[row][col]) {
        return false;
    }

    // Now make sure that the lane is going the right direction
    if(WorldCell(row, col) != WorldCell(row_, col_)) {
        return false;
    }

    return true;
}

void Car::Update(void)
{
    int new_row;
    int new_col;
    GLvector old_pos;
    GLvector camera;

    // If the car isn't ready, place it on the map and get it moving
    camera = CameraPosition();
    if(!ready_) {
        // If the car isn't ready, we need to place it somewhere on the map
        row_ = DEAD_ZONE + RandomVal(WORLD_SIZE - (DEAD_ZONE * 2));
        col_ = DEAD_ZONE + RandomVal(WORLD_SIZE - (DEAD_ZONE * 2));

        // If there is already a car here, forget it.
        if(carmap[row_][col_] > 0) {
            return;
        }

        // If this sport is not a road forget it
        if(!(WorldCell(row_, col_) & CLAIM_ROAD)) {
            return;
        }

        if(!Visible(glVector((float)row_, 0.0f, (float)col_))) {
            return;
        }

        // Good spot. Place the car
        position_ = glVector((float)row_, 0.1f, (float)col_);
        drive_position_ = position_;
        ready_ = true;

        if(WorldCell(row_, col_) & MAP_ROAD_NORTH) {
            direction_ = NORTH;
        }
        if(WorldCell(row_, col_) & MAP_ROAD_EAST) {
            direction_ = EAST;
        }
        if(WorldCell(row_, col_) & MAP_ROAD_SOUTH) {
            direction_ = SOUTH;
        }
        if(WorldCell(row_, col_) & MAP_ROAD_WEST) {
            direction_ = WEST;
        }

        drive_angle_ = dangles[direction_];
        max_speed_ = (float)(4 + RandomVal(6)) / 10.0f;
        speed_ = 0.0f;
        change_ = 3;
        stuck_ = 0;

        carmap[row_][col_]++;
    }

    // Take the car off the map and move it
    carmap[row_][col_]--;
    old_pos = position_;
    speed_ += max_speed_ * 0.05f;
    speed_ =  MIN(speed_, max_speed_);
    position_ += direction[direction_] * MOVEMENT_SPEED * speed_;
    
    // If the car has moved out of view, there's no need to keep simulating it
    if(!Visible(glVector((float)row_, 0.0f, (float)col_))) {
        ready_ = false;
    }
    
    // If the car is far away, remove it. We use Manhattan units because
    // buildings almost always block views of cars on the diagonal.
    float x_dist = fabs(camera.x - position_.x);
    float z_dist = fabs(camera.z - position_.z);
    if((x_dist + z_dist) > RenderFogDistance()) {
        ready_ = false;
    }

    // If the car gets too close to the edge of the map, take it out of play
    if((position_.x < DEAD_ZONE)
       || (position_.x > (WORLD_SIZE - DEAD_ZONE))) {
        ready_ = false;
    }
    if((position_.z < DEAD_ZONE)
       || (position_.z > (WORLD_SIZE - DEAD_ZONE))) {
        ready_ = false;
    }

    if(stuck_ >= STUCK_TIME) {
        ready_ = false;
    }
    
    if(!ready_) {
        return;
    }

    // Check the new position and make sure its not in another car
    new_row = (int)position_.x;
    new_col = (int)position_.z;
    if((new_row != row_) || (new_col != col_)) {
        // See if the new position places us on top of another car
        if(carmap[new_row][new_col]) {
            position_ = old_pos;
            speed_ = 0.0f;
            stuck_++;
        }
    }
    else {
        // Look at the new position and decide if we're heading towards
        // or away from the camera
        row_ = new_row;
        col_ = new_col;
        change_--;
        stuck_ = 0;
        if(direction_ == NORTH) {
            front_ = (camera.z < position_.z);
        }
        else if(direction_ == SOUTH) {
            front_ = (camera.z > position_.z);
        }
        else if(direction_ == EAST) {
            front_ = (camera.z > position_.x);
        }
        else {
            front_ = (camera.x < position_.x);
        }
    }

    drive_position_ = (drive_position_ + position_) / 2.0f;

    // Place the car back on the map
    carmap[row_][col_]++;
}

void Car::Render()
{
    GLvector pos;
    int angle;
    int turn;
    float top;

    if(!ready_) {
        return;
    }

    if(!Visible(drive_position_)) {
        return;
    }

    if(front_) {
        glColor3f(1, 1, 0.8f);
        top = CAR_SIZE;
    }
    else {
        glColor3f(0.5, 0.2f, 0);
        top = 0.0f;
    }

    glBegin(GL_QUADS);
    
    angle = dangles[direction_];
    pos = drive_position_;
    angle = 360 - (int)MathAngle(position_.x, position_.z, pos.x, pos.z);
    angle %= 360;
    turn = (int)MathAngleDifference((float)drive_angle_, (float)angle);
    drive_angle_ += SIGN(turn);
    pos += glVector(0.5f, 0.0f, 0.5f);

    glTexCoord2f(0, 0);
    glVertex3f(pos.x + angles[angle].x, -CAR_SIZE, pos.z + angles[angle].y);
    glTexCoord2f(1, 0);
    glVertex3f(pos.x - angles[angle].x, -CAR_SIZE, pos.z - angles[angle].y);
    glTexCoord2f(1, 1);
    glVertex3f(pos.x - angles[angle].x, top, pos.z - angles[angle].y);
    glTexCoord2f(0, 1);
    glVertex3f(pos.x + angles[angle].x, top, pos.z + angles[angle].y);

    glEnd();
}

void Car::Park()
{
    ready_ = false;
}
