#ifndef SKY_HPP_
#define SKY_HPP_

#include "entity.hpp"
#include "gl-vector3.hpp"
#include "gl-rgba.hpp"

#define SKY_GRID 21
#define SKY_HALF (SKY_GRID / 2)

struct sky_point {
    GLrgba color;
    GLvector3 position;
};

class CSky : public CEntity {
public:
    CSky();
    
    void Render(void);
    void Update(void);

private:
    sky_point grid_[SKY_GRID][SKY_GRID];
};

#endif
