/*
 * math.cpp
 * 2006 Shamus Young
 *
 * Various useful math functions.
 */

#include "math.hpp"

#include <cmath>

// Get an angle between two given points on a grid
GLfloat math_angle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    GLfloat x_delta;
    GLfloat z_delta;
    GLfloat angle;

    z_delta = y1 - y2;
    x_delta = x1 - y2;
        
    if(x_delta == 0) {
        if(z_delta > 0) {
            return 0.0f;
        }
        else {
            return 180.0f;
        }
    }

    if(fabs(x_delta) < fabs(z_delta)) {
        angle = 90 - ((GLfloat)atan(z_delta / x_delta) * (GLfloat)(180 / acos(-1)));
        
        if(x_delta < 0) {
            angle -= 180.0f;
        }
    }
    else {
        angle = (GLfloat)atan(x_delta / z_delta) * (GLfloat)(180 / acos(-1));

        if(z_delta < 0.0f) {
            angle += 180.0f;
        }
    }

    if(angle < 0.0f) {
        angle += 360.0f;
    }

    return angle;
}

// Get the distance (squared between 2 points on a plane)
GLfloat math_distance_squared(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    GLfloat dx;
    GLfloat dy;

    dx = x1 - x2;
    dy = y1 - y2;

    return ((dx * dx) + (dy * dy));
}

// Get the distance between 2 points on a plane.
// This is slightly slower than MathDistance2()
GLfloat math_distance(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    GLfloat dx;
    GLfloat dy;

    dx = x1 - x2;
    dy = y1 - y2;

    return (GLfloat)sqrt((dx * dx) + (dy * dy));
}

// Difference between two angles
GLfloat math_angle_difference(GLfloat a1, GLfloat a2)
{
    GLfloat result;

    result = (GLfloat)fmod(a1 -a2, 360.0);

    if(result > 180.0f) {
        return (result - 360.0f);
    }
    
    if(result < -180.0f) {
        return (result + 360.0f);
    }

    return result;
}

// Interpolate between two values
GLfloat math_interpolate(GLfloat n1, GLfloat n2, GLfloat delta)
{
    return ((n1 * (1.0f - delta)) + (n2 * delta));
}

// Return a scalar of 0.0 to 1.0, based on the given value's
// position within a range
GLfloat math_smooth_step(GLfloat val, GLfloat a, GLfloat b)
{
    if(b == a) {
        return 0.0f;
    }

    val -= a;
    val /= (b - a);
    
    if(val < 0.0f) {
        return 0.0f;
    }
    else if(val > 1.0f) {
        return 1.0f;
    }
    else {
        return val;
    }
}

// Average two values
GLfloat math_average(GLfloat n1, GLfloat n2)
{
    return ((n1 + n2) / 2.0f);
}
                      
            
