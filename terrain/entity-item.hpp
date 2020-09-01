#ifndef ENTITY_ITEM_HPP_
#define ENTITY_ITEM_HPP_

#include <string>

class entity_item {
public:
    entity_item();
    virtual ~entity_item();

    std::string get_type() const;
    void set_type(std::string const &type);

    virtual void update() = 0;

    virtual void render() = 0;

    // Unused
    void render_fade_in();
    void fade_start();

private:
    std::string entity_type_;
};

#endif
