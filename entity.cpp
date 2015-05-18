/*
 * Entity.cpp
 * Copyright (c) 2005 Shamus Young
 * All Rights Reserved
 *
 * An entity is any renderable object in the worl. This is an abstract class.
 */

#include <math.h>
#include <cstring>
#include <GL/gl.h>

#include "camera.hpp"
#include "entity.hpp"
#include "map.hpp"
#include "sky.hpp"
#include "terrain.hpp"

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
{
    next = head;
    head = this;
    m_entity_type = "none";
}

class CEntity *CEntity::Next(void)
{
    return next;
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
    return m_entity_type;
}

CEntity::~CEntity(void)
{
}
