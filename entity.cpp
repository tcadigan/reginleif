/*
 * Entity.cpp
 * Copyright (c) 2005 Shamus Young
 * All Rights Reserved
 *
 * An entity is any renderable object in the worl. This is an abstract class.
 */

#include "entity.hpp"

#include <cstdlib>
#include <cstring>

void entity_update()
{
    // entity *e;
    
    // for(e = head; e != NULL; e = e->next()) {
    //     e->update();
    // }
}

void entity_init()
{
}

entity *entity_find_type(std::string const &type, entity *start)
{
    // entity *e;

    // if(start != NULL) {
    //     start = head;
    // }

    // for(e = start; e != NULL; e = e->next()) {
    //     if(type == e->type()) {
    //         return e;
    //     }
    // }

    return NULL;
}

void entity_term()
{
    // entity *e;
    // entity *next;

    // e = head;
    
    // while(e != NULL) {
    //     next = e->next();
    //     delete e;
    //     e = next;
    // }
}

void entity_render()
{
    // entity *e;
    
    // for(e = head; e != NULL; e = e->next()) {
    //     e->render();
    // }
}

void entity_render_fade_in()
{
    // entity *e;

    // for(e = head; e != NULL; e = e->next()) {
    //     e->render_fade_in();
    // }
}

void entity_fade_start()
{
    // entity *e;

    // for(e = head; e != NULL; e = e->next()) {
    //     e->render_fade_in();
    // }
}

entity::entity()
    : entity_type_("none")
    // , next_(head)
{
    // head = this;
}

void entity::render()
{
}

void entity::render_fade_in()
{
    // By default, perform a normal render for the LOD fade-in
    render();
}

void entity::update()
{
}

void entity::fade_start()
{
}

std::string entity::get_type() const
{
    return entity_type_;
}

void entity::set_type(std::string const &type) 
{
    entity_type_ = type;
}

entity::~entity()
{
}
