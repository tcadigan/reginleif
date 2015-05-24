#ifndef ENTITY_HPP_
#define ENTITY_HPP_

#include <string>

using namespace std;

class entity {
public:
    entity(void);
    virtual ~entity(void);

    entity *next(void);
    virtual void render(void);
    virtual void render_fade_in(void);
    virtual void fade_start(void);
    virtual void update(void);
    virtual string type(void);

protected:
    string entity_type_;

private:
    entity *next_;
};

void entity_update(void);
void entity_init(void);
entity *entity_find_type(string const &type, entity *start);
void entity_term(void);
void entity_render(void);
void entity_render_fade_in(void);
void entity_fade_start(void);

#endif
