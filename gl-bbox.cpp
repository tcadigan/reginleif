/*
 * gl-bbox.cpp
 * 2006 Shamus Young
 *
 * This module has a few functions useful for manipulating the
 * bounding-box structs.
 */

#include "gl-bbox.hpp"

#include "macro.hpp"

// Does the given point fall within the given Bbox?
bool glBboxTestPoint(GLbbox box, GLvector3 point)
{
    if((point.x > box.max.x) || (point.x < box.min.x)) {
        return false;
    }

    if((point.y > box.max.y) || (point.y < box.min.y)) {
        return false;
    }
     
    if((point.z > box.max.z) || (point.z < box.min.z)) {
        return false;
    }

    return true;
}

// Expand BBox (if needed) to contain given point
GLbbox glBboxContainPoint(GLbbox box, GLvector3 point)
{
    if(box.min.x >= point.x) {
        box.min.x = point.x;
    }

    if(box.min.y >= point.y) {
        box.min.y = point.y;
    }

    if(box.min.z >= point.z) {
        box.min.z = point.z;
    }

    if(box.max.x <= point.x) {
        box.max.x = point.x;
    }

    if(box.max.y <= point.y) {
        box.max.y = point.y;
    }

    if(box.max.z <= point.z) {
        box.max.z = point.z;
    }

    return box;
}

// This will invalidate the bbox.
GLbbox glBboxClear(void)
{
    GLbbox result;

    result.max = glVector(-MAX_VALUE, -MAX_VALUE, -MAX_VALUE);
    result.min = glVector(MAX_VALUE, MAX_VALUE, MAX_VALUE);

    return result;
}
