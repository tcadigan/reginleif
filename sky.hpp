#ifndef SKY_HPP_
#define SKY_HPP_

#include "entity.hpp"
#include "gl-vector3.hpp"
#include "gl-rgba.hpp"

struct sky_point {
    GLrgba color;
    GLvector3 position;
};

class CSky : public CEntity {
public:
    CSky();
    virtual ~CSky();
    
    void Render(void);
    void Update(void);

private:
    int sky_grid_;
    sky_point **grid_;
};

#endif
