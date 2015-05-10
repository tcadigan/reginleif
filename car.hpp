#ifndef CAR_HPP_
#define CAR_HPP_

#include "primitives/types.hpp"

class Car {
public:
    Car();
    bool TestPosition(int row, int col);
    void Render();
    void Update();
    void Park();
    
    Car *next_;

private:
    GLvector position_;
    GLvector drive_position_;
    bool ready_;
    bool front_;
    int drive_angle_;
    int row_;
    int col_;
    int direction_;
    int change_;
    int stuck_;
    float speed_;
    float max_speed_;
};

void CarClear();
int CarCount();
void CarRender();
void CarUpdate();

#endif /* CAR_HPP_ */
