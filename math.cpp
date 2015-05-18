/*
 * math.cpp
 * 2006 Shamus Young
 *
 * Various useful math functions.
 */

#include <math.h>

#include "macro.hpp"
#include "math.hpp"

// Get an angle between two given points on a grid
float MathAngle(float x1, float y1, float x2, float y2)
{
    float x_delta;
    float z_delta;
    float angle;

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
        angle = 90 - ((float)atan(z_delta / x_delta) * RADIANS_TO_DEGREES);
        
        if(x_delta < 0) {
            angle -= 180.0f;
        }
    }
    else {
        angle = (float)atan(x_delta / z_delta) * RADIANS_TO_DEGREES;

        if(z_delta < 0.0f) {
            angle += 180.0f;
        }
    }

    if(angle < 0.0f) {
        angle += 360.0f;
    }

    return angle;
}

// Get the distance (squared between 2 points on a plane
float MathDistance2(float x1, float y1, float x2, float y2)
{
    float dx;
    float dy;

    dx = x1 - x2;
    dy = y1 - y2;

    return ((dx * dx) + (dy * dy));
}

// Get the distance between 2 points on a plane.
// This is slightly slower than MathDistance2()
float MathDistance(float x1, float y1, float x2, float y2)
{
    float dx;
    float dy;

    dx = x1 - x2;
    dy = y1 - y2;

    return (float)sqrt((dx * dx) + (dy * dy));
}

// Difference between two angles
float MathAngleDifference(float a1, float a2)
{
    float result;

    result = (float)fmod(a1 -a2, 360.0);

    if(result > 180.0f) {
        return (result - 360.0f);
    }
    
    if(result < -180.0f) {
        return (result + 360.0f);
    }

    return result;
}

// Interpolate between two values
float MathInterpolate(float n1, float n2, float delta)
{
    return ((n1 * (1.0f - delta)) + (n2 * delta));
}

// Return a scalar of 0.0 to 1.0, based on the given value's
// position within a range
float MathSmoothStep(float val, float a, float b)
{
    if(b == a) {
        return 0.0f;
    }

    val -= a;
    val /= (b - a);
    
    return CLAMP(val, 0.0f, 1.0f);
}

// Average two values
float MathAverage(float n1, float n2)
{
    return ((n1 + n2) / 2.0f);
}
                      
            
