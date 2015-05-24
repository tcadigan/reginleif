/*
 * camera.cpp
 * 2006 Shamus Young
 *
 * This module keeps tabs on the camera position, keeps it in bounds, an
 * handles some of the trickery it has to do when moving around a given
 * point on the terrain.
 */

#include "camera.hpp"

#include <cstdlib>
#include <cmath>

#include "ini.hpp"
#include "map.hpp"
#include "math.hpp"
#include "mouse-pointer.hpp"

static gl_vector_3d angle;
static gl_vector_3d position;
static float movement;
static bool moving;
static float eye_height;
static int max_pitch;

void camera_view(float delta)
{
    moving = true;
    angle.y_ -= delta;
}

void camera_pitch(float delta)
{
    moving = true;
    angle.x_ -= delta;
}

void camera_pan(float delta)
{
    float move_x;
    float move_y;

    moving = true;
    move_x = (float)sin(-angle.y_ * (float)(acos(-1) / 180)) / 10.0f;
    move_y = (float)cos(-angle.y_ * (float)(acos(-1) / 180)) / 10.0f;
    position.x_ -= (move_y * delta);
    position.z_ -= (-move_x * delta);
}

void camera_forward(float delta)
{
    float move_x;
    float move_y;

    moving = true;
    move_y = (float)sin(-angle.y_ * (float)(acos(-1) / 180)) / 10.0f;
    move_x = (float)cos(-angle.y_ * (float)(acos(-1) / 180)) / 10.0f;
    position.x_ -= (move_y * delta);
    position.z_ -= (move_x * delta);
}

void camera_selection_pitch(float delta)
{
    gl_vector_3d center;
    float pitch_to;
    float yaw_to;
    float horz_dist;
    float total_dist;
    float vert_dist;
    mouse_pointer *ptr;
    point selected_cell;

    moving = true;
    ptr = (mouse_pointer *)entity_find_type("pointer", NULL);
    selected_cell = ptr->selected();
    delta *= movement;

    if((selected_cell.x_ == -1) || (selected_cell.y_ == -1)) {
        angle.x_ -= delta;

        return;
    }

    center = map_position(selected_cell.x_, selected_cell.y_);
    vert_dist = position.y_ - center.y_;
    yaw_to = math_angle(center.x_, center.z_, position.x_, position.z_);
    horz_dist = math_distance(center.x_, center.z_, position.x_, position.z_);
    total_dist = math_distance(0.0f, 0.0f, horz_dist, vert_dist);
    pitch_to = math_angle(vert_dist, 0.0f, 0.0f, -horz_dist);
    angle.x_ += delta;
    pitch_to += delta;
    angle.x_ -= math_angle_difference(angle.x_, pitch_to) / 15.0f;
    vert_dist = (float)sin(pitch_to * (float)(acos(-1) / 180)) * total_dist;
    horz_dist = (float)cos(pitch_to * (float)(acos(-1) / 180)) * total_dist;
    position.x_ = center.x_ - (float)sin(yaw_to * (float)(acos(-1) / 180)) * horz_dist;
    position.y_ = center.y_ + vert_dist;
    position.z_ = center.z_ - (float)cos(yaw_to * (float)(acos(-1) / 180)) * horz_dist;
}

void camera_selection_zoom(float delta)
{
    gl_vector_3d center;
    gl_vector_3d offset;
    float total_dist;
    mouse_pointer *ptr;
    point selected_cell;

    moving = true;
    ptr = (mouse_pointer *)entity_find_type("pointer", NULL);
    selected_cell = ptr->selected();
    delta *= movement;

    if((selected_cell.x_ == -1) || (selected_cell.y_ == -1)) {
        angle.x_ -= delta;
        
        return;
    }

    center = map_position(selected_cell.x_, selected_cell.y_);
    offset = gl_vector_subtract(position, center);
    total_dist = gl_vector_length(offset);
    offset = gl_vector_normalize(offset);
    total_dist += delta;
    offset = gl_vector_scale(offset, total_dist);
    position = gl_vector_add(center, offset);
}

void camera_selection_yaw(float delta)
{
    gl_vector_3d center;
    float yaw_to;
    float horz_dist;
    float vert_dist;
    mouse_pointer *ptr;
    point selected_cell;

    moving = true;
    ptr = (mouse_pointer *)entity_find_type("pointer", NULL);
    selected_cell = ptr->selected();
    delta *= movement;

    if((selected_cell.x_ == -1) || (selected_cell.y_ == -1)) {
        angle.y_ -= delta;

        return;
    }

    center = map_position(selected_cell.x_, selected_cell.y_);
    vert_dist = position.y_ - center.y_;
    yaw_to = math_angle(center.x_, center.z_, position.x_, position.z_);
    horz_dist = math_distance(center.x_, center.z_, position.x_, position.z_);
    angle.y_ -= math_angle_difference(angle.y_, -yaw_to + 180.0f) / 15.0f;
    yaw_to += delta;
    angle.y_ -= delta;

    position.x_ = center.x_ - (float)sin(yaw_to * (float)(acos(-1) / 180)) * horz_dist;
    position.y_ = center.y_ + vert_dist;
    position.z_ = center.z_ - (float)cos(yaw_to * (float)(acos(-1) / 180)) * horz_dist;
}

gl_vector_3d camera_position(void)
{
    return position;
}

void camera_position_set(gl_vector_3d new_pos)
{
    float limit;
    float elevation;
    
    limit = (float)map_size();
    position = new_pos;

    if(position.x_ < -limit) {
        position.x_ = -limit;
    }
    else if(position.x_ > limit) {
        position.x_ = limit;
    }

    if(position.y_ < -512.0f) {
        position.y_ = -512.0f;
    }
    else if(position.y_ > 512.0f) {
        position.y_ = 512.0f;
    }

    if(position.z_ < -limit) {
        position.z_ = -limit;
    }
    else if(position.z_ > limit) {
        position.z_ = limit;
    }

    elevation = map_elevation(position.x_, position.z_) + eye_height;

    if(elevation > position.y_) {
        position.y_ = elevation;
    }
}

gl_vector_3d camera_angle(void)
{
    return angle;
}

void camera_angle_set(gl_vector_3d new_angle)
{
    angle = new_angle;

    if(angle.x_ < -80.f) {
        angle.x_ = -80.0f;
    }
    else if(angle.x_ > 80.0f) {
        angle.x_ = 80.0f;
    }
}

void camera_init(void)
{
    ini_manager ini_mgr;

    angle = ini_mgr.get_vector("Camera Settings", "CameraAngle");
    position = ini_mgr.get_vector("Camera Settings", "CameraPosition");

    eye_height = ini_mgr.get_float("Camera Settings", "eye_height");
    max_pitch = ini_mgr.get_int("Camera Settings", "max_pitch");
}

void camera_update(void)
{
    float limit;
    float elevation;

    if(moving) {
        movement *= 1.1f;
    }
    else {
        movement = 0.0f;
    }

    if(movement < 0.01f) {
        movement = 0.01f;
    }
    else if(movement > 1.0f) {
        movement = 1.0f;
    }

    limit = (float)map_size() * 1.5f;

    if(position.x_ < -limit) {
        position.x_ = -limit;
    }
    else if(position.x_ > limit) {
        position.x_ = limit;
    }

    if(position.y_ < -512.0f) {
        position.y_ = -512.0f;
    }
    else if(position.y_ > 512.0f) {
        position.y_ = 512.0f;
    }

    if(position.z_ < -limit) {
        position.z_ = -limit;
    }
    else if(position.z_ > limit) {
        position.z_ = limit;
    }

    elevation = map_elevation(position.x_, position.z_) + eye_height;

    if(elevation > position.y_) {
        position.y_ = elevation;
    }

    if(angle.x_ < -max_pitch) {
        angle.x_ = -max_pitch;
    }
    else if(angle.x_ > max_pitch) {
        angle.x_ = max_pitch;
    }

    moving = false;
}

void camera_term(void)
{
    ini_manager ini_mgr;

    // Just store our most recent position in the ini
    ini_mgr.set_vector("Settings", "CameraAngle", angle);
    ini_mgr.set_vector("Settings", "CameraPosition", position);
}
