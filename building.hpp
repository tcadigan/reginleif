#ifndef BUILDING_HPP_
#define BUILDING_HPP_

#include "entity.hpp"

#include "gl-rgba.hpp"
#include "mesh.hpp"

enum {
    BUILDING_SIMPLE,
    BUILDING_BLOCKY,
    BUILDING_MODERN,
    BUILDING_TOWER
};

class Building: public Entity {
public:
    Building(int type, 
             int x, 
             int y,
             int height,
             int width,
             int depth,
             int seed,
             gl_rgba color);
    ~Building();

    void Render(void);
    int PolyCount();
    void RenderFlat(bool colored);
    unsigned int Texture();

private:
    int x_;
    int y_;
    int width_;
    int depth_;
    int height_;
    int texture_type_;
    int seed_;
    int roof_tiers_;
    gl_rgba color_;
    gl_rgba trim_color_;
    Mesh *mesh_;
    Mesh *mesh_flat_;
    bool have_lights_;
    bool have_trim_;
    bool have_logo_;

    void CreateSimple();
    void CreateBlocky();
    void CreateModern();
    void CreateTower();

    float ConstructWall(int start_x,
                        int start_y,
                        int start_z,
                        int direction,
                        int length,
                        int height,
                        int window_groups,
                        float uv_start,
                        bool blank_corners);

    void ConstructSpike(int left,
                        int right,
                        int front,
                        int back,
                        int bottom,
                        int top);

    void ConstructCube(int left,
                       int right,
                       int front,
                       int back,
                       int bottom,
                       int top);

    void ConstructCube(float left,
                       float right,
                       float front,
                       float back,
                       float bottom,
                       float top);

    void ConstructRoof(float left,
                       float right,
                       float front,
                       float back,
                       float bottom);
};

#endif /* BUILDING_HPP_ */
