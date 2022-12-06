/*
 * car.cpp
 *
 * 2009 Shamus Young
 *
 * This creates the little two-triangle cars and moves them around the map.
 *
 */

#include "car.hpp"

#include <SDL2/SDL.h>
#include <cmath>
#include <list>

#include "building.hpp"
#include "camera.hpp"
#include "gl-vector2.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "random.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "visible.hpp"
#include "win.hpp"
#include "world.hpp"

static int const DEAD_ZONE = 200;
static int const STUCK_TIME = 230;
static int const UPDATE_INTERVAL = 30; // Milliseconds
static float const MOVEMENT_SPEED = 0.61f;
static float const CAR_SIZE = 3.0f;

static std::array<gl_vector3, 4> direction = {
    gl_vector3(0.0f, 0.0f, -1.0f),
    gl_vector3(1.0f, 0.0f,  0.0f),
    gl_vector3(0.0f, 0.0f,  1.0f),
    gl_vector3(-1.0f, 0.0f,  0.0f)
};

static std::array<int, 4> dangles = { 0, 90, 180, 270 };

static std::array<gl_vector2, 360> angles;
static bool angles_done;
static std::array<std::array<unsigned char, WORLD_SIZE>, WORLD_SIZE> carmap;
static std::list<Car> cars;
static unsigned next_update;
static int count;

int car_count()
{
    return count;
}

void car_clear()
{
    for (Car &car : cars) {
        car.park();
    }

    for (std::array<unsigned char, WORLD_SIZE> &row : carmap) {
        for (unsigned char &car : row) {
            car = '\0';
        }
    }

    count = 0;
}

void car_render()
{
    if (!angles_done) {
        for (int i = 0; i < angles.size(); ++i) {
            angles.at(i).set_x(cos(i * DEGREES_TO_RADIANS) * CAR_SIZE);
            angles.at(i).set_y(sin(i * DEGREES_TO_RADIANS) * CAR_SIZE);
        }
    }

    glDepthMask(false);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_ONE, GL_ONE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, TextureId(TEXTURE_HEADLIGHT));

    for (Car &car : cars) {
        car.render();
    }

    glDepthMask(true);
}

void car_update()
{
    if (!TextureReady() || !entity_ready()) {
        return;
    }

    unsigned int now = SDL_GetTicks();

    if (next_update > now) {
        return;
    }

    next_update = now + UPDATE_INTERVAL;

    for (Car &car : cars) {
        car.update();
    }
}

Car::Car()
{
    ready_ = false;
    cars.push_back(*this);
    count++;
}

bool Car::test_position(int row, int col)
{
    // Test the given position and see if it's already occupied
    if (carmap.at(row).at(col)) {
        return false;
    }

    // Now make sure that the lane is going the right direction
    if (WorldCell(row, col) != WorldCell(row_, col_)) {
        return false;
    }

    return true;
}

void Car::update(void)
{
    gl_vector3 old_pos;

    // If the car isn't ready, place it on the map and get it moving
    gl_vector3 camera = camera_position();
    if (!ready_) {
        // If the car isn't ready, we need to place it somewhere on the map
        row_ = DEAD_ZONE + random_val(WORLD_SIZE - (DEAD_ZONE * 2));
        col_ = DEAD_ZONE + random_val(WORLD_SIZE - (DEAD_ZONE * 2));

        // If there is already a car here, forget it.
        if (carmap.at(row_).at(col_) > 0) {
            return;
        }

        // If this spot is not a road forget it
        if ((WorldCell(row_, col_) & usage_t::claim_road) == usage_t::none) {
            return;
        }

        if (!Visible(gl_vector3(row_, 0.0f, col_))) {
            return;
        }

        // Good spot. Place the car
        position_ = gl_vector3(row_, 0.1f, col_);
        drive_position_ = position_;
        ready_ = true;

        if ((WorldCell(row_, col_) & usage_t::map_road_north) != usage_t::none) {
            direction_ = direction_t::north;
            drive_angle_ = dangles.at(0);
        }
        if ((WorldCell(row_, col_) & usage_t::map_road_east) != usage_t::none) {
            direction_ = direction_t::east;
            drive_angle_ = dangles.at(1);
        }
        if ((WorldCell(row_, col_) & usage_t::map_road_south) != usage_t::none) {
            direction_ = direction_t::south;
            drive_angle_ = dangles.at(2);
        }
        if ((WorldCell(row_, col_) & usage_t::map_road_west) != usage_t::none) {
            direction_ = direction_t::west;
            drive_angle_ = dangles.at(3);
        }

        max_speed_ = (4 + random_val(6)) / 10.0f;
        speed_ = 0.0f;
        change_ = 3;
        stuck_ = 0;

        carmap.at(row_).at(col_)++;
    }

    // Take the car off the map and move it
    carmap.at(row_).at(col_)--;
    old_pos = position_;
    speed_ += max_speed_ * 0.05f;
    speed_ =  MIN(speed_, max_speed_);

    switch (direction_) {
    case direction_t::north:
        position_ += (direction.at(0) * (MOVEMENT_SPEED * speed_));
        break;
    case direction_t::east:
        position_ += (direction.at(1) * (MOVEMENT_SPEED * speed_));
        break;
    case direction_t::south:
        position_ += (direction.at(2) * (MOVEMENT_SPEED * speed_));
        break;
    case direction_t::west:
        position_ += (direction.at(3) * (MOVEMENT_SPEED * speed_));
    }

    // If the car has moved out of view, there's no need to keep simulating it
    if (!Visible(gl_vector3(row_, 0.0f, col_))) {
        ready_ = false;
    }

    // If the car is far away, remove it. We use Manhattan units because
    // buildings almost always block views of cars on the diagonal.
    float x_dist = fabs(camera.get_x() - position_.get_x());
    float z_dist = fabs(camera.get_z() - position_.get_z());
    if ((x_dist + z_dist) > RenderFogDistance()) {
        ready_ = false;
    }

    // If the car gets too close to the edge of the map, take it out of play
    if ((position_.get_x() < DEAD_ZONE)
       || (position_.get_x() > (WORLD_SIZE - DEAD_ZONE))) {
        ready_ = false;
    }
    if ((position_.get_z() < DEAD_ZONE)
       || (position_.get_z() > (WORLD_SIZE - DEAD_ZONE))) {
        ready_ = false;
    }

    if (stuck_ >= STUCK_TIME) {
        ready_ = false;
    }

    if (!ready_) {
        return;
    }

    // Check the new position and make sure its not in another car
    int new_row = position_.get_x();
    int new_col = position_.get_z();
    if ((new_row != row_) || (new_col != col_)) {
        // See if the new position places us on top of another car
        if (carmap.at(new_row).at(new_col)) {
            position_ = old_pos;
            speed_ = 0.0f;
            stuck_++;
        }
    } else {
        // Look at the new position and decide if we're heading towards
        // or away from the camera
        row_ = new_row;
        col_ = new_col;
        change_--;
        stuck_ = 0;
        if (direction_ == direction_t::north) {
            front_ = (camera.get_z() < position_.get_z());
        } else if (direction_ == direction_t::south) {
            front_ = (camera.get_z() > position_.get_z());
        } else if (direction_ == direction_t::east) {
            front_ = (camera.get_z() > position_.get_x());
        } else {
            front_ = (camera.get_x() < position_.get_x());
        }
    }

    drive_position_ = (drive_position_ + position_) / 2.0f;

    // Place the car back on the map
    carmap.at(row_).at(col_)++;
}

void Car::render()
{
    int angle;
    float top;

    if (!ready_) {
        return;
    }

    if (!Visible(drive_position_)) {
        return;
    }

    if (front_) {
        glColor3f(1, 1, 0.8f);
        top = CAR_SIZE;
    } else {
        glColor3f(0.5, 0.2f, 0);
        top = 0.0f;
    }

    glBegin(GL_QUADS);

    switch (direction_) {
    case direction_t::north:
        angle = dangles.at(0);
        break;
    case direction_t::east:
        angle = dangles.at(1);
        break;
    case direction_t::south:
        angle = dangles.at(2);
        break;
    case direction_t::west:
        angle = dangles.at(3);
        break;
    }
    gl_vector3 pos = drive_position_;
    angle = 360 - MathAngle(position_.get_x(),
                                 position_.get_z(),
                                 pos.get_x(),
                                 pos.get_z());

    angle %= 360;
    int turn = MathAngleDifference(drive_angle_, angle);
    drive_angle_ += SIGN(turn);
    pos += gl_vector3(0.5f, 0.0f, 0.5f);

    glTexCoord2f(0, 0);
    glVertex3f(pos.get_x() + angles.at(angle).get_x(),
               -CAR_SIZE,
               pos.get_z() + angles.at(angle).get_y());

    glTexCoord2f(1, 0);
    glVertex3f(pos.get_x() - angles.at(angle).get_x(),
               -CAR_SIZE,
               pos.get_z() - angles.at(angle).get_y());

    glTexCoord2f(1, 1);
    glVertex3f(pos.get_x() - angles.at(angle).get_x(),
               top,
               pos.get_z() - angles.at(angle).get_y());

    glTexCoord2f(0, 1);
    glVertex3f(pos.get_x() + angles.at(angle).get_x(),
               top,
               pos.get_z() + angles.at(angle).get_y());

    glEnd();
}

void Car::park()
{
    ready_ = false;
}
