#ifndef ENTITY_MANAGER_HPP_
#define ENTITY_MANAGER_HPP_

#include <string>
#include <vector>

#include "entity-item.hpp"

class entity_manager {
public:
    void update();
    void term();

    entity_item *find_type(std::string const &type) const;
    void render();
    void fade_in();
    void fade_start();

private:
    std::vector<entity_item *> entities_;
};

#endif
