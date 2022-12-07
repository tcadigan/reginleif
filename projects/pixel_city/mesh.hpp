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

    void vertex_add(gl_vertex const &v);
    int vertex_count();
    int poly_count() const;
    void cube_add(Cube const &c);
    void quad_strip_add(QuadStrip const &qs);
    void fan_add(Fan const &f);
    void render() const;
    void compile();

    unsigned int list_;
    int poly_count_;
    std::vector<gl_vertex> vertex_;
    std::vector<Cube> cube_;
    std::vector<QuadStrip> quad_strip_;
    std::vector<Fan> fan_;
    bool compiled_;
};

#endif /* MESH_HPP_ */
