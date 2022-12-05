#ifndef MESH_HPP_
#define MESH_HPP_

#include "gl-vertex.hpp"

#include <vector>

class Cube {
public:
    std::vector<int> index_list; // Probably always size() == 10...
};

class QuadStrip {
public:
    std::vector<int> index_list;
};

class Fan {
public:
    std::vector<int> index_list;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void VertexAdd(gl_vertex const &v);
    int VertexCount();
    int PolyCount();
    void CubeAdd(Cube const &c);
    void QuadStripAdd(QuadStrip const &qs);
    void FanAdd(Fan const &f);
    void Render();
    void Compile();

    unsigned int list_;
    int polycount_;
    std::vector<gl_vertex> vertex_;
    std::vector<Cube> cube_;
    std::vector<QuadStrip> quad_strip_;
    std::vector<Fan> fan_;
    bool compiled_;
};

#endif /* MESH_HPP_ */
