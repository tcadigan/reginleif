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

static class CEntity *head;

void EntityUpdate(void)
{
    CEntity *e;
    
    for(e = head; e != NULL; e = e->Next()) {
        e->Update();
    }
}

void EntityInit(void)
{
}

CEntity *EntityFindType(char const *type, CEntity *start)
{
    CEntity *e;

    if(start != NULL) {
        start = head;
    }

    for(e = start; e != NULL; e = e->Next()) {
        if(strcmp(type, e->Type()) == 0) {
            return e;
        }
    }

    return NULL;
}

void EntityTerm(void)
{
    CEntity *e;
    CEntity *next;

    e = head;
    
    while(e != NULL) {
        next = e->Next();
        delete e;
        e = next;
    }
}

void EntityRender(void)
{
    class CEntity *e;
    
    for(e = head; e != NULL; e = e->Next()) {
        e->Render();
    }
}

void EntityRenderFadeIn(void)
{
    class CEntity *e;

    for(e = head; e != NULL; e = e->Next()) {
        e->RenderFadeIn();
    }
}

void EntityFadeStart(void)
{
    class CEntity *e;

    for(e = head; e != NULL; e = e->Next()) {
        e->RenderFadeIn();
    }
}

CEntity::CEntity(void)
    : entity_type_("none")
    , next_(head)
{
    head = this;
}

class CEntity *CEntity::Next(void)
{
    return next_;
}

void CEntity::Render(void)
{
}

void CEntity::RenderFadeIn(void)
{
    // By default, perform a normal render for the LOD fade-in
    Render();
}

void CEntity::Update(void)
{
}

void CEntity::FadeStart()
{
}

char const *CEntity::Type()
{
    return entity_type_;
}

CEntity::~CEntity(void)
{
}
