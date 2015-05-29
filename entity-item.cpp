/*
 * entity_item.cpp
 */

#include "entity-item.hpp"

entity_item::entity_item()
    : entity_type_("none")
{
}

entity_item::~entity_item()
{
}

std::string entity_item::get_type() const
{
    return entity_type_;
}

void entity_item::set_type(std::string const &type)
{
    entity_type_ = type;
}

void entity_item::render_fade_in()
{
    // By default, perform a normal render for the
    // Level Of Detail fade-in
    render();
}

void entity_item::fade_start()
{
}
