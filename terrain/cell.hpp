#ifndef CELL_HPP_
#define CELL_HPP_

#include "enums.hpp"
#include "gl-vector3.hpp"
#include "gl-rgba.hpp"

class cell {
public:
    cell();
    virtual ~cell();

    void set_shadow(GLboolean shadow);
    void set_distance(GLfloat distance);
    void set_layer(GLushort value, GLint index);
    void set_position(gl_vector3 const &position);
    void set_normal(gl_vector3 const &normal);
    void set_light(gl_rgba const &light);

    GLboolean get_shadow() const;
    GLfloat get_distance() const;
    GLushort get_layer(GLint index) const;
    gl_vector3 get_position() const;
    gl_vector3 get_normal() const;
    gl_rgba get_light() const;

private:
    GLboolean shadow_;
    // Move this to low-res sub-map?
    GLfloat distance_;
    GLushort layer_[terrain::enums::LAYER_COUNT - 1];
    gl_vector3 position_;
    gl_vector3 normal_;
    gl_rgba light_;
};

#endif
