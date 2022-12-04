#ifndef SKY_HPP_
#define SKY_HPP_

#include "gl-rgba.hpp"
#include "gl-vector3.hpp"

#define SKY_GRID 21
#define SKY_HALF (SKY_GRID / 2)

struct sky_point {
    gl_rgba color;
    gl_vector3 position;
};

class Sky {
public:
    Sky();
    void Render(void);

private:
    int list_;
    sky_point grid_[SKY_GRID][SKY_GRID];
};

void SkyRender();
void SkyClear();

#endif /* SKY_HPP_ */
