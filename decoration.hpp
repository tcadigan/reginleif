#ifndef DECORATION_HPP_
#define DECORATION_HPP_

#include "entity.hpp"
#include "mesh.hpp"

class Decoration : Entity {
public:
    Decoration();
    ~Decoration();
    void CreateLogo(GLvector2 start,
                    GLvector2 end,
                    float base,
                    int seed, 
                    GLrgba color);

    void CreateLightStrip(float x,
                          float z,
                          float width,
                          float depth,
                          float height,
                          GLrgba color);

    void CreateLightTrim(GLvector *chain,
                         int count,
                         float height,
                         int seed,
                         GLrgba color);

    void CreateRadioTower(GLvector pos, float height);
    void Render(void);
    void RenderFlat(bool colored);
    bool Alpha();
    int PolyCount();
    unsigned Texture();

private:
    GLrgba color_;
    Mesh *mesh_;
    int type_;
    unsigned texture_;
    bool use_alpha_;
};

#endif /* DECORATION_HPP_ */
