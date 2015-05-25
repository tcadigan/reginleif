/*
 * gl-bbox.cpp
 * 2006 Shamus Young
 *
 * This module has a few functions useful for manipulating the
 * bounding-box structs.
 */

#include "gl-bbox.hpp"

#include <cfloat>

gl_bbox::gl_bbox()
{
}

gl_bbox::~gl_bbox()
{
}

// Does the given point fall within the given Bbox?
bool gl_bbox::test_point(gl_vector3 const &point)
{
    if((point.get_x() > max_.get_x()) || (point.get_x() < min_.get_x())) {
        return false;
    }

    if((point.get_y() > max_.get_y()) || (point.get_y() < min_.get_y())) {
        return false;
    }

    if((point.get_z() > max_.get_z()) || (point.get_z() < min_.get_z())) {
        return false;
    }

    return true;
}

// Expand BBox (if needed) to contain given point
void gl_bbox::contain_point(gl_vector3 const &point)
{
    if(min_.get_x() >= point.get_x()) {
        min_.set_x(point.get_x());
    }

    if(min_.get_y() >= point.get_y()) {
        min_.set_y(point.get_y());
    }
                                                
    if(min_.get_z() >= point.get_z()) {
        min_.set_z(point.get_z());
    }
    
    if(max_.get_x() <= point.get_x()) {
        max_.set_x(point.get_x());
    }
                                                
    if(max_.get_y() <= point.get_y()) {
        max_.set_y(point.get_y());
    }

    if(max_.get_z() <= point.get_z()) {
        max_.set_z(point.get_z());
    }
}

// This will invalidate the bbox.
void gl_bbox::clear()
{
    max_.set_x(-FLT_MAX);
    max_.set_y(-FLT_MAX);
    max_.set_z(-FLT_MAX);

    min_.set_x(FLT_MAX);
    min_.set_y(FLT_MAX);
    min_.set_z(FLT_MAX);
}

void gl_bbox::set_min(gl_vector3 const &min)
{
    min_ = min;
}

void gl_bbox::set_max(gl_vector3 const &max)
{
    max_ = max;
}

gl_vector3 gl_bbox::get_min() const
{
    return min_;
}

gl_vector3 gl_bbox::get_max() const
{
    return max_;
}
