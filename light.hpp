#ifndef LIGHT_HPP_
#define LIGHT_HPP_

class Light {
public:
    Light(GLvector pos, GLrgba color, int size);
    Light *next_;
    void Render();
    void BLink();

private:
    GLvector position_;
    GLrgba color_;
    int size_;
    float vert_size_;
    float flat_size_;
    bool blink_;
    unsigned blink_interval_;
    int cell_x_;
    int cell_z_;
};

void LightRender();
void LightClear();
int LightCount();

#endif /* LIGHT_HPP_ */
