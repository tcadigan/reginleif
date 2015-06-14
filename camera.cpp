/*
 * camera.cpp
 *
 * 2009 Shamus Young
 *
 * This tracks the position and orientation of the camera. In screensaver
 * mode, it moves the camera around the world in order to create dramatic
 * views of the hot zone.
 *
 */

#include "camera.hpp"

#include <SDL.h>
#include <cmath>
#include <ctime>
#include <string>

#include "gl-bbox.hpp"
#include "ini.hpp"
#include "macro.hpp"
#include "math.hpp"
#include "win.hpp"
#include "world.hpp"

#define EYE_HEIGHT 2.0f
#define MAX_PITCH 85
#define FLYCAM_CIRCUIT 60000
#define FLYCAM_CIRCUIT_HALF (FLYCAM_CIRCUIT / 2)
#define FLYCAM_LEG (FLYCAM_CIRCUIT / 4)
#define ONE_SECOND 1000
#define CAMERA_CHANGE_INTERVAL 15
#define CAMERA_CYCLE_LENGTH (CAMERA_MODES * CAMERA_CHANGE_INTERVAL)

enum {
    CAMERA_FLYCAM1,
    CAMERA_ORBIT_INWARD,
    CAMERA_ORBIT_OUTWARD,
    CAMERA_ORBIT_ELLIPTICAL,
    CAMERA_FLYCAM2,
    CAMERA_SPEED,
    CAMERA_SPIN,
    CAMERA_FLYCAM3,
    CAMERA_MODES
};

static gl_vector3 angle;
static gl_vector3 position;
static gl_vector3 auto_angle;
static gl_vector3 auto_position;
static gl_vector3 movement;
static bool cam_auto;
static float tracker;
static unsigned int last_update;
static int camera_behavior;
static unsigned int last_move;

static gl_vector3 flycam_position(unsigned int t)
{
    unsigned int leg;
    float delta;
    gl_vector3 start;
    gl_vector3  end;
    gl_bbox hot_zone;

    hot_zone = WorldHotZone();
    t %= FLYCAM_CIRCUIT;
    leg = t / FLYCAM_LEG;
    delta = (float)(t % FLYCAM_LEG) / FLYCAM_LEG;
    
    switch(leg) {
    case 0:
        start = gl_vector3(hot_zone.get_min().get_x(),
                           25.0f,
                           hot_zone.get_min().get_z());

        end = gl_vector3(hot_zone.get_min().get_x(),
                         60.0f,
                         hot_zone.get_max().get_z());

        break;
    case 1:
        start = gl_vector3(hot_zone.get_min().get_x(),
                           60.0f,
                           hot_zone.get_max().get_z());

        end = gl_vector3(hot_zone.get_max().get_x(),
                         25.0f,
                         hot_zone.get_max().get_z());

        break;
    case 2:
        start = gl_vector3(hot_zone.get_max().get_x(), 
                           25.0f,
                           hot_zone.get_max().get_z());

        end = gl_vector3(hot_zone.get_max().get_x(), 
                         60.0f,
                         hot_zone.get_min().get_z());

        break;
    case 3:
        start = gl_vector3(hot_zone.get_max().get_x(),
                           60.0f,
                           hot_zone.get_min().get_z());

        end = gl_vector3(hot_zone.get_min().get_x(), 
                         25.0f,
                         hot_zone.get_min().get_z());

        break;
    }

    delta = MathScalarCurve(delta);

    return start.interpolate(end, delta);
}

static void do_auto_cam()
{
    float dist;
    unsigned int elapsed;
    unsigned int now;
    int behavior;
    gl_vector3 target;

    now = SDL_GetTicks();
    elapsed = now - last_update;
    elapsed = MIN(elapsed, 50); // Limit to 1/20th second worth of time
    if(elapsed == 0) {
        return;
    }

    last_update = now;

    behavior = camera_behavior;

    tracker += ((float)elapsed / 300.0f);
    // behavior = CAMERA_FLYCAM;

    switch(behavior) {
    case CAMERA_ORBIT_INWARD:
        auto_position.set_x(WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 150.0f));
        
        auto_position.set_y(60.0f);
        
        auto_position.set_z(WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 150.0f));

        target = gl_vector3(WORLD_HALF, 40.0f, WORLD_HALF);

        break;
    case CAMERA_ORBIT_OUTWARD:
        auto_position.set_x(WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 250.0f));
        
        auto_position.set_y(60.0f);

        auto_position.set_z(WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 250.0f));

        target = gl_vector3(WORLD_HALF, 30.0f, WORLD_HALF);

        break;
    case CAMERA_ORBIT_ELLIPTICAL:
        dist = 150.0f + (sinf((tracker * DEGREES_TO_RADIANS) / 1.1f) * 50);
        auto_position.set_x(WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * dist));

        auto_position.set_y(60.0f);
        
        auto_position.set_z(WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * dist));

        target = gl_vector3(WORLD_HALF, 50.0f, WORLD_HALF);

        break;
    case CAMERA_FLYCAM1:
    case CAMERA_FLYCAM2:
    case CAMERA_FLYCAM3:
        auto_position =
            (flycam_position(now) + flycam_position(now + 4000)) / 2.0f;

        target = flycam_position(now + FLYCAM_CIRCUIT_HALF - (ONE_SECOND * 3));

        break;
    case CAMERA_SPEED:
        auto_position = 
            (flycam_position(now) + flycam_position(now + 500)) / 2.0f;

        target = flycam_position(now + (ONE_SECOND * 5));
        auto_position.set_y(auto_position.get_y() / 2);

        break;
    default:
        target.set_x(WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 300.0f));
        target.set_y(30.0f);
        target.set_z(WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 300.0f));
        
        auto_position.set_x(WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 50.0f));
            
        auto_position.set_y(60.0f);
        
        auto_position.set_z(WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 50.0f));
    }

    dist = MathDistance(auto_position.get_x(),
                        auto_position.get_z(),
                        target.get_x(),
                        target.get_z());
    
    auto_angle.set_y(-MathAngle(auto_position.get_x(),
                                auto_position.get_y(),
                                target.get_x(),
                                target.get_y()));

    auto_angle.set_x(90.f + MathAngle(0, 
                                      auto_position.get_y(),
                                      dist,
                                      target.get_y()));
}

void CameraAutoToggle()
{
    cam_auto = !cam_auto;
}

void CameraNextBehavior()
{
    camera_behavior++;
    camera_behavior %= CAMERA_MODES;
}

void CameraYaw(float delta)
{
    angle.set_y(angle.get_y() - delta);
}

void CameraPitch(float delta)
{
    angle.set_x(angle.get_x() - delta);
}

void CameraPan(float delta)
{
    float move_x;
    float move_y;

    move_x = (float)sin(-angle.get_y() * DEGREES_TO_RADIANS) / 10.0f;
    move_y = (float)cos(-angle.get_y() * DEGREES_TO_RADIANS) / 10.0f;
    position.set_x(position.get_x() - (move_y * delta));
    position.set_z(position.get_z() - (-move_x * delta));
}

void CameraForward(float delta)
{
    float move_x;
    float move_y;

    move_y = (float)sin(-angle.get_y() * DEGREES_TO_RADIANS) / 10.0f;
    move_x = (float)cos(-angle.get_y() * DEGREES_TO_RADIANS) / 10.0f;
    position.set_x(position.get_x() - (move_y * delta));
    position.set_z(position.get_z() - (move_x * delta));
}

void CameraVertical(float val)
{
    movement.set_y(movement.get_y() + val);
    last_move = SDL_GetTicks();
}

void CameraLateral(float val)
{
    movement.set_x(movement.get_x() + val);
    last_move = SDL_GetTicks();
}

void CameraMedial(float val)
{
    movement.set_z(movement.get_z() + val);
    last_move = SDL_GetTicks();
}

gl_vector3 CameraPosition(void)
{
    if(cam_auto) {
        return auto_position;
    }

    return position;
}

void CameraReset()
{
    position.set_y(50.0f);
    position.set_x(WORLD_HALF);
    position.set_y(WORLD_HALF);
    angle.set_x(0.0f);
    angle.set_y(0.0f);
    angle.set_z(0.0f);
}

void CameraPositionSet(gl_vector3 new_pos)
{
    position = new_pos;
}

gl_vector3 CameraAngle(void)
{
    if(cam_auto) {
        return auto_angle;
    }

    return angle;
}

void CameraAngleSet(gl_vector3 new_angle)
{
    angle = new_angle;
    angle.set_x(CLAMP(angle.get_x(), -80.f, 80.f));
}

void CameraInit(void)
{
    std::string camera_angle("CameraAngle");
    std::string camera_position("CameraPosition");
    
    angle = IniVector(const_cast<char *>(camera_angle.c_str()));
    position = IniVector(const_cast<char *>(camera_position.c_str()));
}

void CameraUpdate(void)
{
    CameraPan(movement.get_x());
    CameraForward(movement.get_z());
    position.set_y(position.get_y() + (movement.get_y() / 10.0f));
    if((SDL_GetTicks() - last_move) > 1000) {
        movement *= 0.9f;
    }
    else {
        movement *= 0.99f;
    }
        
    if(cam_auto) {
        do_auto_cam();
    }

    if(angle.get_y() < 0.0f) {
        angle.set_y(360.0f - (float)fmod(fabs(angle.get_y()), 360.0f));
    }

    angle.set_y((float)fmod(angle.get_y(), 360.0f));
    angle.set_x(CLAMP(angle.get_x(), -MAX_PITCH, MAX_PITCH));
}

void CameraTerm(void)
{
    std::string camera_angle("CameraAngle");
    std::string camera_position("CameraPosition");
    
    // Just store our most recent position in the ini
    IniVectorSet(const_cast<char *>(camera_angle.c_str()), angle);
    IniVectorSet(const_cast<char *>(camera_position.c_str()), position);
}
