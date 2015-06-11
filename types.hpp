#ifndef TYPES_HPP_
#define TYPES_HPP_

#define GL_CLAMP_TO_EDGE 0x812F

#define OPERATORS(type)                         \
    type operator+(type const &c);              \
    type operator+(float const &c);             \
    void operator+=(type const &c);             \
    void operator+=(float const &c);            \
    type operator-(type const &c);              \
    type operator-(float const &c);             \
    void operator-=(type const &c);             \
    void operator-=(float const &c);            \
    type operator*(type const &c);              \
    type operator*(float const &c);             \
    void operator*=(type const &c);             \
    void operator*=(float const &c);            \
    type operator/(type const &c);              \
    type operator/(float const &c);             \
    void operator/=(type const &c);             \
    void operator/=(float const &c);            \
    bool operator==(type const &c);

#define JOINT_MAX_CHILDREN 8

struct GLquat {
    float x;
    float y;
    float z;
    float w;
};

struct GLvector2 {
    float x;
    float y;
    OPERATORS(GLvector2);
};

struct GLrgba {
    float red;
    float green;
    float blue;
    float alpha;
    OPERATORS(GLrgba);
};

struct GLmatrix {
    float elements[4][4];
};

struct GLvertex {
    GLvector3 position;
    GLvector2 uv;
    GLrgba color;
    int bone;
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

GLrgba glRgba(char *string);
GLrgba glRgba(float red, float green, float blue);
GLrgba glRgba(float luminance);
GLrgba glRgba(float red, float green, float blue, float alpha);
GLrgba glRgba(long c);
GLrgba glRgba(int red, int green, int blue);
GLrgba glRgbaAdd(GLrgba c1, GLrgba c2);
GLrgba glRgbaSubtract(GLrgba c1, GLrgba c2);
GLrgba glRgbaInterpolate(GLrgba c1, GLrgba c2, float delta);
GLrgba glRgbaScale(GLrgba c, float scale);
GLrgba glRgbaMultiply(GLrgba c1, GLrgba c2);
GLrgba glRgbaUnique(int i);
GLrgba glRgbaFromHsl(float h, float s, float l);

GLmatrix glMatrixIdentity(void);
void glMatrixElementsSet(GLmatrix *m, float *in);
GLmatrix glMatrixMultiply(GLmatrix a, GLmatrix b);
GLvector glMatrixTransformPoint(GLmatrix m, GLvector in);
GLmatrix glMatrixTranslate(GLmatrix m, GLvector in);
GLmatrix glMatrixRotate(GLmatrix m, float theta, float x, float y, float z);
GLvector glMatrixToEuler(GLmatrix mat, int order);

GLquat glQuat(float x, float y, float z, float w);
GLvector glQuatToEuler(GLquat q, int order);

GLvector2 glVector(float x, float y);
GLvector2 glVectorAdd(GLvector2 val1, GLvector2 val2);
GLvector2 glVectorSubtract(GLvector val1, GLvector2 val2);
GLvector2 glVectoreNormalize(GLvector2 v);
GLvector2 glVectorInterpolate(GLvector2 v1, GLvector v2, float scalar);
GLvector2 glVectorSinCos(float angle);
float geVectorLength(GLvector2 v);

#ifndef NULL
#define NULL 0
#endif

#endif /* TYPES_HPP_ */
