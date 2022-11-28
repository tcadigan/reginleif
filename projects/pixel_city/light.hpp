#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include "gl-rgba.hpp"
#include "gl-vector3.hpp"

class Light {
public:
    Light(gl_vector3 pos, gl_rgba color, int size);
    Light *next_;
    void Render();
    void Blink();

private:
    gl_vector3 position_;
    gl_rgba color_;
    int size_;
    float vert_size_;
    float flat_size_;
    bool blink_;
    unsigned blink_interval_;
    int cell_x_;
    int cell_z_;
};

void LightRender();
void LightClear();
int LightCount();

#endif /* LIGHT_HPP_ */
