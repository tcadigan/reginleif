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

#include <cmath>
#include <ctime>
#include <string>

#include "primitives/types.hpp"

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

static GLvector angle;
static GLvector position;
static GLvector auto_angle;
static GLvector auto_position;
static float distance;
static GLvector movement;
static bool cam_auto;
static float tracker;
static unsigned int last_update;
static int camera_behavior;
static unsigned int last_move;

static GLvector flycam_position(unsigned int t)
{
    unsigned int leg;
    float delta;
    GLvector start;
    GLvector end;
    GLbbox hot_zone;

    hot_zone = WorldHotZone();
    t %= FLYCAM_CIRCUIT;
    leg = t / FLYCAM_LEG;
    delta = (float)(t % FLYCAM_LEG) / FLYCAM_LEG;
    
    switch(leg) {
    case 0:
        start = glVector(hot_zone.min.x, 25.0f, hot_zone.min.z);
        end = glVector(hot_zone.min.x, 60.0f, hot_zone.max.z);
        break;
    case 1:
        start = glVector(hot_zone.min.x, 60.0f, hot_zone.max.z);
        end = glVector(hot_zone.max.x, 25.0f, hot_zone.max.z);
        break;
    case 2:
        start = glVector(hot_zone.max.x, 25.0f, hot_zone.max.z);
        end = glVector(hot_zone.max.x, 60.0f, hot_zone.min.z);
        break;
    case 3:
        start = glVector(hot_zone.max.x, 60.0f, hot_zone.min.z);
        end = glVector(hot_zone.min.x, 25.0f, hot_zone.min.z);
        break;
    }

    delta = MathScalarCurve(delta);

    return glVectorInterpolate(start, end, delta);
}

static void do_auto_cam()
{
    float dist;
    unsigned int t;
    unsigned int elapsed;
    unsigned int now;
    int behavior;
    GLvector target;

    now = GetTickCount();
    elapsed = now - last_update;
    elapsed = MIN(elapsed, 50); // Limit to 1/20th second worth of time
    if(elapsed == 0) {
        return;
    }

    last_update = now;
    t = time(NULL) % CAMERA_CYCLE_LENGTH;

#if SCREENSAVER
    behavior = t / CAMERA_CHANGE_INTERVAL;
#else
    behavior = camera_behavior;
#endif

    tracker += ((float)elapsed / 300.0f);
    // behavior = CAMERA_FLYCAM;

    switch(behavior) {
    case CAMERA_ORBIT_INWARD:
        auto_position.x = 
            WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 150.0f);
        
        auto_position.y = 60.0f;
        
        auto_position.z =
            WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 150.0f);

        target = glVector(WORLD_HALF, 40.0f, WORLD_HALF);
        break;
    case CAMERA_ORBIT_OUTWARD:
        auto_position.x = 
            WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 250.0f);
        
        auto_position.y = 60.0f;

        auto_position.z =
            WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 250.0f);

        target = glVector(WORLD_HALF, 30.0f, WORLD_HALF);
        break;
    case CAMERA_ORBIT_ELLIPTICAL:
        dist = 150.0f + (sinf((tracker * DEGREES_TO_RADIANS) / 1.1f) * 50);
        auto_position.x = 
            WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * dist);

        auto_position.y = 60.0f;
        
        auto_position.z =
            WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * dist);

        target = glVector(WORLD_HALF, 50.0f, WORLD_HALF);
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
        auto_position.y /= 2;
        break;
    default:
        target.x = WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 300.0f);
        target.y = 30.0f;
        target.z = WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 300.0f);
        
        auto_position.x = 
            WORLD_HALF + (sinf(tracker * DEGREES_TO_RADIANS) * 50.0f);
            
        auto_position.y = 60.0f;
        
        auto_position.z = 
            WORLD_HALF + (cosf(tracker * DEGREES_TO_RADIANS) * 50.0f);
    }

    dist = MathDistance(auto_position.x, auto_position.z, target.x, target.z);
    auto_angle.y = MathAngle(
        -MathAngle(auto_position.x, auto_position.z, target.x, target.z));

    auto_angle.x = 90.f + MathAngle(0, auto_position.y, dist, target.y);
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
    angle.y -= delta;
}

void CameraPitch(float delta)
{
    angle.x -= delta;
}

void CameraPan(float delta)
{
    float move_x;
    float move_y;

    move_x = (float)sin(-angle.y * DEGREES_TO_RADIANS) / 10.0f;
    move_y = (float)cos(-angle.y * DEGREES_TO_RADIANS) / 10.0f;
    position.x -= (move_y * delta);
    position.z -= (-move_x * delta);
}

void CameraForward(float delta)
{
    float move_x;
    float move_y;

    move_y = (float)sin(-angle.y * DEGREES_TO_RADIANS) / 10.0f;
    move_x = (float)cos(-angle.y * DEGREES_TO_RADIANS) / 10.0f;
    position.x -= (move_y * delta);
    position.z -= (move_x * delta);
}

void CameraVertical(float val)
{
    movement.y += val;
    last_move = GetTickCount();
}

void CameraLateral(float val)
{
    movement.x += val;
    last_move = GetTickCount();
}

void CameraMedial(float val)
{
    movement.z += val;
    last_move = GetTickCount();
}

GLvector CameraPosition(void)
{
    if(cam_auto) {
        return auto_position;
    }

    return position;
}

void CameraReset()
{
    position.y = 50.0f;
    position.x = WORLD_HALF;
    position.y = WORLD_HALF;
    angle.x = 0.0f;
    angle.y = 0.0f;
    angle.z = 0.0f;
}

void CameraPositionSet(GLvector new_pos)
{
    position = new_pos;
}

GLvector CameraAngle(void)
{
    if(cam_auto) {
        return auto_angle;
    }

    return angle;
}

void CameraAngleSet(GLvector new_angle)
{
    angle = new_angle;
    angle.x = CLAMP(angle.x, -80.f, 80.f);
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
    CameraPan(movement.x);
    CameraForward(movement.z);
    position.y += (movement.y / 10.0f);
    if((GetTickCount() - last_move) > 1000) {
        movement *= 0.9f;
    }
    else {
        movement *= 0.99f;
    }
    
    if(SCREENSAVER) {
        cam_auto = true;
    }
    
    if(cam_auto) {
        do_auto_cam();
    }

    if(angle.y < 0.0f) {
        angle.y = 360.0f - (float)fmod(fabs(angle.y), 360.0f);
    }

    angle.y = (float)fmod(angle.y, 360.0f);
    angle.x = CLAMP(angle.x, -MAX_PITCH, MAX_PITCH);
}

void CameraTerm(void)
{
    std::string camera_angle("CameraAngle");
    std::string camera_position("CameraPosition");
    
    // Just store our most recent position in the ini
    IniVectorSet(const_cast<char *>(camera_angle.c_str()), angle);
    IniVectorSet(const_cast<char *>(camera_position.c_str()), position);
}
