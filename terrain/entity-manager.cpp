/*
 * entity-manager.cpp
 * Copyright (c) 2005 Shamus Young
 * All Rights Reserved
 *
 * An entity is any renderable object in the worl. This is an abstract class.
 */

#include "entity-manager.hpp"

entity_manager::entity_manager()
{
}

entity_manager::~entity_manager()
{
}

void entity_manager::init()
{
}

void entity_manager::update()
{
    std::vector<entity_item *>::reverse_iterator itr;

    for(itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->update();
    }
}


void entity_manager::term()
{
    std::vector<entity_item *>::reverse_iterator itr;

    for(itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        delete *itr;
    }
}

entity_item *entity_manager::find_type(std::string const &type) const
{
    std::vector<entity_item *>::const_reverse_iterator itr;

    for(itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        if((*itr)->get_type() == type) {
            break;
        }
    }

    return *itr;
}

void entity_manager::render()
{
    std::vector<entity_item *>::reverse_iterator itr;

    for(itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->render();
    }
}

void entity_manager::fade_in()
{
    std::vector<entity_item *>::reverse_iterator itr;

    for(itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->render_fade_in();
    }
}

void entity_manager::fade_start()
{
    std::vector<entity_item *>::reverse_iterator itr;

    for(itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->render_fade_in();
    }
}
