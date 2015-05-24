/*
 * gl-bbox.cpp
 * 2006 Shamus Young
 *
 * This module has a few functions useful for manipulating the
 * bounding-box structs.
 */

#include "gl-bbox.hpp"

#include <cfloat>

// Does the given point fall within the given Bbox?
bool gl_bbox_test_point(gl_bbox box, gl_vector_3d point)
{
    if((point.x_ > box.max_.x_) || (point.x_ < box.min_.x_)) {
        return false;
    }

    if((point.y_ > box.max_.y_) || (point.y_ < box.min_.y_)) {
        return false;
    }
     
    if((point.z_ > box.max_.z_) || (point.z_ < box.min_.z_)) {
        return false;
    }

    return true;
}

// Expand BBox (if needed) to contain given point
gl_bbox gl_bbox_contain_point(gl_bbox box, gl_vector_3d point)
{
    if(box.min_.x_ >= point.x_) {
        box.min_.x_ = point.x_;
    }

    if(box.min_.y_ >= point.y_) {
        box.min_.y_ = point.y_;
    }

    if(box.min_.z_ >= point.z_) {
        box.min_.z_ = point.z_;
    }

    if(box.max_.x_ <= point.x_) {
        box.max_.x_ = point.x_;
    }

    if(box.max_.y_ <= point.y_) {
        box.max_.y_ = point.y_;
    }

    if(box.max_.z_ <= point.z_) {
        box.max_.z_ = point.z_;
    }

    return box;
}

// This will invalidate the bbox.
gl_bbox gl_bbox_clear(void)
{
    gl_bbox result;

    result.max_ = gl_vector_3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    result.min_ = gl_vector_3d(FLT_MAX, FLT_MAX, FLT_MAX);

    return result;
}

gl_bbox::gl_bbox()
{
}

gl_bbox::~gl_bbox()
{
}
