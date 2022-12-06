#ifndef CAR_HPP_
#define CAR_HPP_

#include "gl-vector3.hpp"

#include "win.hpp"

class Car {
public:
    Car();
    bool test_position(int row, int col);
    void render();
    void update();
    void park();

private:
    gl_vector3 position_;
    gl_vector3 drive_position_;
    bool ready_;
    bool front_;
    int drive_angle_;
    int row_;
    int col_;
    direction_t direction_;
    int change_;
    int stuck_;
    float speed_;
    float max_speed_;
};

void car_clear();
int car_count();
void car_render();
void car_update();

#endif /* CAR_HPP_ */
