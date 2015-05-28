#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>

class entity {
public:
    entity();
    virtual ~entity();

    std::string get_type() const;
    void  set_type(std::string const &type);

    virtual void update() = 0;

    virtual void render() = 0;

    // Unused
    virtual void render_fade_in();
    virtual void fade_start();

private:
    std::string entity_type_;
};

void entity_update();
void entity_init();
entity *entity_find_type(std::string const &type, entity *start);
void entity_term();
void entity_render();
void entity_render_fade_in();
void entity_fade_start();

#endif
