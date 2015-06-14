#ifndef TYPES_HPP_
#define TYPES_HPP_

#define GL_CLAMP_TO_EDGE 0x812F

#define JOINT_MAX_CHILDREN 8

struct GLvertex {
    GLvector3 position;
    GLvector2 uv;
    GLrgba color;
};

struct GLrect {
    float left;
    float top;
    float right;
    float bottom;
};

struct GLtriangle {
    int v1;
    int v2;
    int v3;
    int normal1;
    int normal2;
    int normal3;
};

// class GLmodel {
// public:
//     unsigned int vertex_count;
//     unsigned int triangle_count;
//     unsigned int normal_count;
//     GLvertex *vertex;
//     GLVector *normal;
//     GLtriangle triangle;

//     GLModel();
//     ~GLModel();
    
//     void TriangleRender(unsigned int n);
//     GLtriangle *TriangleAdd(unsigned int v1, unsigned int, unsigned int);
//     GLtriangle *TriangleAdd(GLtriangle c);
//     void NormalAdd(GLvector n);
//     void VertexAdd(GLvertex v);
//     void Render();
//     GLbbox BBox();

// private:
//     GLbbox m_bbox;
// };

// struct GLKeyframe {
//     float time;
//     GLvector offset;
//     GLvector rotation;
// };

// struct GLsegment {
//     int index;
//     GLvector rotation;
//     GLvector offset;
//     GLkeyFrame keyframes[255];
//     int frame_count;
// };

// class GLanimate {
// public:
//     GLanimate();
    
//     void KeyFrameAdd(int joint, float time, GLquat q);
//     void TimeSet(float time);
//     void PositionSet(float pos);
//     GLvector Rotation(int);
//     GLvector offset(int);

// private:
//     GLsegment *m_segments;
//     int m_segment_count;
//     float m_length;
// };

#endif /* TYPES_HPP_ */
