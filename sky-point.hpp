#ifndef SKY_POINT_HPP_
#define SKY_POINT_HPP_

#include "gl-rgba.hpp"
#include "gl-vector-3d.hpp"

class sky_point {
public:
    sky_point();
    virtual ~sky_point();

    gl_rgba color_;
    gl_vector_3d position_;
};

#endif
