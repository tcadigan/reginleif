/*
 * entity-manager.cpp
 * Copyright (c) 2005 Shamus Young
 * All Rights Reserved
 *
 * An entity is any renderable object in the world. This is an abstract class.
 */

#include "entity-manager.hpp"

void entity_manager::update()
{
    for(auto itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->update();
    }
}


void entity_manager::term()
{
    for(auto itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        delete *itr;
    }
}

entity_item *entity_manager::find_type(std::string const &type) const
{
    entity_item *result = nullptr;

    for(auto itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        if((*itr)->get_type() == type) {
            result = *itr;
            break;
        }
    }

    return result;
}

void entity_manager::render()
{
    for(auto itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->render();
    }
}

void entity_manager::fade_in()
{
    for(auto itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->render_fade_in();
    }
}

void entity_manager::fade_start()
{
    for(auto itr = entities_.rbegin(); itr != entities_.rend(); ++itr) {
        (*itr)->render_fade_in();
    }
}
