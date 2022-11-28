#ifndef SKY_POINT_HPP_
#define SKY_POINT_HPP_

#include "gl-rgba.hpp"
#include "gl-vector3.hpp"

class sky_point {
public:
    sky_point();
    virtual ~sky_point();

    void set_color(gl_rgba const &color);
    void set_position(gl_vector3 const &position);

    gl_rgba get_color() const;
    gl_vector3 get_position() const;

private:
    gl_rgba color_;
    gl_vector3 position_;
};

#endif
