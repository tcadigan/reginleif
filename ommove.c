/*
 * Omega copyright (c) 1987-1989 by Laurence Raphael Brothers
 *
 * ommove.c
 *
 * Monster move functions
 */

#include "oglob.h"

/* Like m_normal_move, but can open doors */
void m_smart_move(struct monster *m)
{
    m_simple_move(m);
}

/* Not very smart, but not altogether stupid movement */
void m_normal_move(struct monster *m)
{
    m_simple_move(m);
}

/* Used by both m_normal_move and m_smart_move */
void m_simple_move(struct monster *m)
{
    
