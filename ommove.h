#ifndef OMMOVE_H_
#define OMMOVE_H_

#include "odefs.h"

void m_teleport(struct monster *m);
void m_vanish(struct monster *m);
void m_random_move(struct monster *m);
void m_simple_move(struct monster *m);
void m_scaredy_move(struct monster *m);
void m_move_animal(struct monster *m);
void m_confused_move(struct monster *m);
void m_spirit_move(struct monster *m);
void m_smart_move(struct monster *m);
void m_follow_move(struct monster *m);
void m_flutter_move(struct monster *m);
void m_normal_move(struct monster *m);
void m_teleport(struct monster *m);

#endif
