#ifndef SUN_HPP_
#define SUN_HPP_

#include "gl-quat.hpp"
#include "gl-rgba.hpp"
#include "gl-vector3.hpp"

class sun {
public:
    sun();
    virtual ~sun();

    gl_vector3 get_position() const;
    gl_quat get_position_quat() const;
    gl_rgba get_color() const;

    void set_position(gl_vector3 const &position);
    void set_color(gl_rgba const &color);

private:
    gl_vector3 position_;
    gl_rgba color_;
};

#endif
