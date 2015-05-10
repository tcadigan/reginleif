#ifndef SKY_HPP_
#define SKY_HPP_

#define SKY_GRID 21
#define SKY_HALF (SKY_GRID / 2)

struct sky_point {
    GLrgba color;
    GLvector position;
};

class Sky {
public:
    Sky();
    void Render(void);

private:
    int list_;
    int stars_list_;
    sky_point grid_[SKY_GRID][SKY_GRID];
};

void SkyRender();
void SkyClear();

#endif /* SKY_HPP_ */
