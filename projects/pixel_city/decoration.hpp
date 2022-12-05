#ifndef DECORATION_HPP_
#define DECORATION_HPP_

#include "entity.hpp"

#include <array>

#include "gl-rgba.hpp"
#include "gl-vector2.hpp"
#include "gl-vector3.hpp"
#include "mesh.hpp"

class Decoration : Entity {
public:
    Decoration();
    ~Decoration();
    void CreateLogo(gl_vector2 start,
                    gl_vector2 end,
                    float base,
                    int seed,
                    gl_rgba color);

    void CreateLightStrip(float x,
                          float z,
                          float width,
                          float depth,
                          float height,
                          gl_rgba color);

    void CreateLightTrim(std::array<gl_vector3, MAX_VBUFFER> &chain,
                         int count,
                         float height,
                         int seed,
                         gl_rgba color);

    void CreateRadioTower(gl_vector3 pos, float height);
    void Render(void);
    void RenderFlat(bool colored);
    bool Alpha();
    int PolyCount();
    unsigned Texture();

private:
    gl_rgba color_;
    Mesh *mesh_;
    unsigned texture_;
    bool use_alpha_;
};

#endif /* DECORATION_HPP_ */
