#ifndef SKY_HPP_
#define SKY_HPP_

#include "entity.hpp"
#include "sky-point.hpp"

class sky : public entity {
public:
    sky();
    virtual ~sky();
    
    void render(void);
    void update(void);

private:
    int sky_grid_;
    sky_point **grid_;
};

#endif
