#ifndef MESH_HPP_
#define MESH_HPP_

#include <vector>

struct cube {
    std::vector<int> index_list; // Probably always size() == 10...
};

struct quad_strip {
    std::vector<int> index_list;
};

struct fan {
    std::vector<int> index_list;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void VertexAdd(const GLvertex &v);
    int VertexCount();
    int PolyCount();
    void CubeAdd(const cube &c);
    void QuadStripAdd(const quad_strip &qs);
    void FanAdd(const fan &f);
    void Render();
    void Compile();

    unsigned int list_;
    int polycount_;
    std::vector<GLvertex> vertex_;
    std::vector<cube> cube_;
    std::vector<quad_strip> quad_strip_;
    std::vector<fan> fan_;
    bool compiled_;
};

#endif /* MESH_HPP_ */
