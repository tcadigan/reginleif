#ifndef TERRAIN_HPP_
#define TERRAIN_HPP_

#include "gl-vector3.hpp"
#include "gl-vector2.hpp"
#include "entity.hpp"

enum build_stage {
    // Check for the need to begin building
    STAGE_IDLE, 
    // Reset the mesh
    STAGE_CLEAR,
    // Run the quad tree and figure out what points will be in the final mesh
    STAGE_QUADTREE, 
    STAGE_TEXTURES,
    // Build the final glList
    STAGE_COMPILE,
    STAGE_WAIT_FOR_FADE,
    STAGE_DONE
};

class CTerrain : public CEntity {
public:
    CTerrain(int size);
    ~CTerrain();

    void Render(void);
    void RenderFadeIn(void);
    void Update(void);
    void FadeStart(void);
    
private:
    char stage_;
    int map_size_;
    int map_half_;
    int zone_size_;
    GLvector3 viewpoint_;
    short *boundary_;
    bool *point_;

    float tolerance_;
    int update_time_;
    int do_wireframe_;
    int do_solid_;
    int zone_grid_;

    unsigned int list_front_;
    unsigned int list_back_;

    GLvector2 *zone_uv_;
    int x_;
    int y_;
    int layer_;
    int zone_;
    int zone_origin_x_;
    int zone_origin_y_;
    bool compile_back_;
    bool use_color_;
    bool fade_;
    long triangles_;
    long vertices_;
    long build_start_;
    long build_time_;
    long compile_time_;

    void Compile(void);
    void CompileBlock(int x, int y, int size);
    void CompileTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
    void CompileVertex(int x, int y);
    void CompileStrip(int x1,
                      int y1,
                      int x2,
                      int y2,
                      int x3,
                      int y3,
                      int x4,
                      int y4);

    void CompileFan(int x1,
                    int y1,
                    int x2,
                    int y2,
                    int x3,
                    int y3,
                    int x4,
                    int y4,
                    int x5,
                    int y5);

    void CompileFan(int x1,
                    int y1,
                    int x2,
                    int y2,
                    int x3,
                    int y3,
                    int x4,
                    int y4,
                    int x5,
                    int y5,
                    int x6,
                    int y6);

    void GridStep(void);
    void DoQuad(int x, int y, int size);
    void PointActivate(int x, int y);
};
    
#endif
