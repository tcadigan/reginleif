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

#include "math.hpp"
#include "mouse-pointer.hpp"

camera::camera(world const &world_object)
    : world_(world_object)
{
}

camera::~camera()
{
}

void camera::init(terrain_map const &terrain_map,
                  ini_manager const &ini_mgr)
{
    terrain_map_ = &terrain_map;
    ini_mgr_ = &ini_mgr;

    angle_ = ini_mgr_->get_vector("Camera Settings", "CameraAngle");
    position_ = ini_mgr_->get_vector("Camera Settings", "CameraPosition");
    eye_height_ = ini_mgr_->get_float("Camera Settings", "eye_height");
    max_pitch_ = ini_mgr_->get_int("Camera Settings", "max_pitch");
}

void camera::term(void)
{
    ini_manager ini_mgr;

    // Just store our most recent position in the ini
    ini_mgr.set_vector("Settings", "CameraAngle", angle_);
    ini_mgr.set_vector("Settings", "CameraPosition", position_);
}

void camera::pitch(GLfloat delta)
{
    moving_ = true;
    angle_.set_x(angle_.get_x() - delta);
}

void camera::yaw(GLfloat delta)
{
    moving_ = true;
    angle_.set_y(angle_.get_y() - delta);
}

void camera::pan(GLfloat delta)
{
    GLfloat move_x;
    GLfloat move_y;

    moving_ = true;
    move_x = (GLfloat)sin(-angle_.get_y() * (GLfloat)(acos(-1) / 180)) / 10.0f;
    move_y = (GLfloat)cos(-angle_.get_y() * (GLfloat)(acos(-1) / 180)) / 10.0f;
    position_.set_x(position_.get_x() - (move_y * delta));
    position_.set_z(position_.get_z() - (-move_x * delta));
}

void camera::forward(GLfloat delta)
{
    GLfloat move_x;
    GLfloat move_y;

    moving_ = true;
    move_y = (float)sin(-angle_.get_y() * (GLfloat)(acos(-1) / 180)) / 10.0f;
    move_x = (float)cos(-angle_.get_y() * (GLfloat)(acos(-1) / 180)) / 10.0f;
    position_.set_x(position_.get_x() - (move_y * delta));
    position_.set_z(position_.get_z() - (move_x * delta));
}

void camera::selection_pitch(GLfloat delta)
{
    gl_vector3 center;
    GLfloat pitch_to;
    GLfloat yaw_to;
    GLfloat horz_dist;
    GLfloat total_dist;
    GLfloat vert_dist;
    mouse_pointer *ptr;
    point selected_cell;

    moving_ = true;
    ptr = (mouse_pointer *)entity_find_type("pointer", NULL);
    selected_cell = ptr->get_selected();
    delta *= movement_;

    if((selected_cell.get_x() == -1) || (selected_cell.get_y() == -1)) {
        angle_.set_x(angle_.get_x() - delta);

        return;
    }

    center = terrain_map_->get_position(selected_cell.get_x(), selected_cell.get_y());
    vert_dist = position_.get_y() - center.get_y();
    yaw_to = math_angle(center.get_x(),
                        center.get_z(),
                        position_.get_x(), 
                        position_.get_z());

    horz_dist = math_distance(center.get_x(),
                              center.get_z(),
                              position_.get_x(),
                              position_.get_z());

    total_dist = math_distance(0.0f, 0.0f, horz_dist, vert_dist);
    pitch_to = math_angle(vert_dist, 0.0f, 0.0f, -horz_dist);
    angle_.set_x(angle_.get_x() + delta);
    pitch_to += delta;
    angle_.set_x(angle_.get_x() - math_angle_difference(angle_.get_x(), pitch_to) / 15.0f);
    vert_dist = (GLfloat)sin(pitch_to * (GLfloat)(acos(-1) / 180)) * total_dist;
    horz_dist = (GLfloat)cos(pitch_to * (GLfloat)(acos(-1) / 180)) * total_dist;
    position_.set_x(center.get_x() - (GLfloat)sin(yaw_to * (GLfloat)(acos(-1) / 180)) * horz_dist);
    position_.set_y(center.get_y() + vert_dist);
    position_.set_x(center.get_z() - (GLfloat)cos(yaw_to * (GLfloat)(acos(-1) / 180)) * horz_dist);
}

void camera::selection_yaw(GLfloat delta)
{
    gl_vector3 center;
    GLfloat yaw_to;
    GLfloat horz_dist;
    GLfloat vert_dist;
    mouse_pointer *ptr;
    point selected_cell;

    moving_ = true;
    ptr = (mouse_pointer *)entity_find_type("pointer", NULL);
    selected_cell = ptr->get_selected();
    delta *= movement_;

    if((selected_cell.get_x() == -1) || (selected_cell.get_y() == -1)) {
        angle_.set_y(angle_.get_y() - delta);

        return;
    }

    center = terrain_map_->get_position(selected_cell.get_x(), selected_cell.get_y());
    vert_dist = position_.get_y() - center.get_y();
    yaw_to = math_angle(center.get_x(),
                        center.get_z(),
                        position_.get_x(), 
                        position_.get_z());

    horz_dist = math_distance(center.get_x(),
                              center.get_z(),
                              position_.get_x(),
                              position_.get_z());

    angle_.set_y(angle_.get_y() - math_angle_difference(angle_.get_y(), -yaw_to + 180.0f) / 15.0f);
    yaw_to += delta;
    angle_.set_y(angle_.get_y() - delta);

    position_.set_x(center.get_x() - (GLfloat)sin(yaw_to * (GLfloat)(acos(-1) / 180)) * horz_dist);
    position_.set_y(center.get_y() + vert_dist);
    position_.set_z(center.get_z() - (GLfloat)cos(yaw_to * (GLfloat)(acos(-1) / 180)) * horz_dist);
}

void camera::selection_zoom(GLfloat delta)
{
    gl_vector3 center;
    gl_vector3 offset;
    GLfloat total_dist;
    mouse_pointer *ptr;
    point selected_cell;

    moving_ = true;
    ptr = (mouse_pointer *)entity_find_type("pointer", NULL);
    selected_cell = ptr->get_selected();
    delta *= movement_;

    if((selected_cell.get_x() == -1) || (selected_cell.get_y() == -1)) {
        angle_.set_x(angle_.get_x() - delta);
        
        return;
    }

    center = terrain_map_->get_position(selected_cell.get_x(), selected_cell.get_y());
    offset = position_ - center;
    total_dist = offset.length();
    offset.normalize();
    total_dist += delta;
    offset *= total_dist;
    position_ = center + offset;
}

gl_vector3 camera::get_position() const
{
    return position_;
}

gl_vector3 camera::get_angle() const
{
    return angle_;
}

void camera::set_position(gl_vector3 new_pos)
{
    GLfloat limit;
    GLfloat elevation;
    
    limit = (GLfloat)terrain_map_->get_size();
    position_ = new_pos;

    if(position_.get_x() < -limit) {
        position_.set_x(-limit);
    }
    else if(position_.get_x() > limit) {
        position_.set_x(limit);
    }

    if(position_.get_y() < -512.0f) {
        position_.set_y(-512.0f);
    }
    else if(position_.get_y() > 512.0f) {
        position_.set_y(512.0f);
    }

    if(position_.get_z() < -limit) {
        position_.set_z(-limit);
    }
    else if(position_.get_z() > limit) {
        position_.set_z(limit);
    }

    elevation = 
        terrain_map_->get_elevation(position_.get_x(), position_.get_z()) + eye_height_;

    if(elevation > position_.get_y()) {
        position_.set_y(elevation);
    }
}

void camera::set_angle(gl_vector3 new_angle)
{
    angle_ = new_angle;

    if(angle_.get_x() < -80.f) {
        angle_.set_x(-80.0f);
    }
    else if(angle_.get_x() > 80.0f) {
        angle_.set_x(80.0f);
    }
}

void camera::update()
{
    GLfloat limit;
    GLfloat elevation;

    if(moving_) {
        movement_ *= 1.1f;
    }
    else {
        movement_ = 0.0f;
    }

    if(movement_ < 0.01f) {
        movement_ = 0.01f;
    }
    else if(movement_ > 1.0f) {
        movement_ = 1.0f;
    }

    limit = (GLfloat)terrain_map_->get_size() * 1.5f;

    if(position_.get_x() < -limit) {
        position_.set_x(-limit);
    }
    else if(position_.get_x() > limit) {
        position_.set_x(limit);
    }

    if(position_.get_y() < -512.0f) {
        position_.set_y(-512.0f);
    }
    else if(position_.get_y() > 512.0f) {
        position_.set_y(512.0f);
    }

    if(position_.get_z() < -limit) {
        position_.set_z(-limit);
    }
    else if(position_.get_z() > limit) {
        position_.set_z(limit);
    }

    elevation = 
        terrain_map_->get_elevation(position_.get_x(), position_.get_z()) + eye_height_;

    if(elevation > position_.get_y()) {
        position_.set_y(elevation);
    }

    if(angle_.get_x() < -max_pitch_) {
        angle_.set_x(-max_pitch_);
    }
    else if(angle_.get_x() > max_pitch_) {
        angle_.set_x(max_pitch_);
    }

    moving_ = false;
}

void camera::render()
{
    // no-op
}
