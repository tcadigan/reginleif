#include "sno4.h"

#include "sno1.h"
#include "sno2.h"
#include "sno3.h"

#include <stdio.h>

struct node *and(struct node *ptr)
{
    struct node *a;
    struct node *p;

    p = ptr;
    a = p->p1;
    
    if(p->typ == 0) {
	switch(a->typ) {
	case 0:
	    a->typ = 1;
	case 1:
	    a = copy(a->p2);

	    return a;
	case 3:
	    fflush(stdout);
	    
	    return syspit();
	case 5:
	    a = a->p2->p1;
	    a = copy(a->p2);

	    return a;
	case 6:

	    return binstr(nfree());
	}

	writes("Attempt to take an illegal value");

	a->typ = 1;
	a = copy(a->p2);

	return a;
    }

    return a;
}

struct node *eval(struct node *e, int t)
{
    struct node *list;
    struct node *a2;
    struct node *a3;
    struct node *a4;
    struct node *a3base;
    struct node *a1;
    struct node *stack;
    struct node *op;

    int temp;

    if(rfail == 1) {
	return 0;
    }

    stack = 0;
    list = e;

    while(1) {
	temp = list->typ;
    
	switch(temp) {
	default:
	case 0:
	    if(t == 1) {
		a1 = and(stack);

		stack = pop(stack);

		if(stack) {
		    writes("Phase error");
		}

		return a1;
	    }
	
	    if(stack->typ == 1) {
		writes("Attempt to store in a value");
	    }

	    a1 = stack->p1;

	    stack = pop(stack);
	
	    if(stack) {
		writes("Phase error");
	    }

	    return a1;
	case 12:
	    a1 = and(stack);
	    stack->p1 = look(a1);
	    delete(a1);
	    stack->typ = 0;
	    list = list->p1;

	    break;
	case 13:
	    if(stack->typ) {
		writes("Illegal function");
	    }

	    a1 = stack->p1;
	
	    if(a1->typ != 5) {
		writes("Illegal function");
	    }

	    a1 = a1->p2;
	    op = a1->p1;
	    a3 = alloc();
	    a3base = a3;
	    a3->p2 = op->p2;
	    op->p2 = 0;
	    a1 = a1->p2;
	    a2 = list->p2;

	    while(1) {
		a4 = alloc();
		a3->p1 = a4;
		a3 = a4;
		a3->p2 = and(a1);
	    
		/* recursive */
		assign(a1->p1, eval(a2->p2, 1));
		a1 = a1->p2;
		a2 = a2->p1;
	    
		if((a1 != 0) && (a2 != 0)) {
		    continue;
		}

		break;
	    }

	    if(a1 != a2) {
		writes("Parameters do not match");
	    }

	    op = op->p1;

	    while(1) {
		/* recursive */
		op = execute(op);
	    
		if(op) {
		    continue;
		}

		break;
	    }

	    a1 = stack->p1->p2;
	    op = a1->p1;
	    a3 = a3base;
	    stack->p1 = op->p2;
	    stack->typ = 1;
	    op->p2 = a3->p2;

	    while(1) {
		a4 = a3->p1;
		sno_free(a3);
		a3 = a4;
		a1 = a1->p2;
	    
		if(a1 == 0) {
		    list = list->p1;
		
		    break;
		}
	    
		assign(a1->p1, a3->p2);
	    }

	    break;
	case 11:
	case 10:
	case 9:
	case 8:
	case 7:
	    a1 = and(stack);
	    stack = pop(stack);
	    a2 = and(stack);
	    a3 = doop(temp, a2, a1);
	    delete(a1);
	    delete(a2);
	    stack->p1 = a3;
	    stack->typ = 1;
	    list = list->p1;

	    break;
	case 15:
	    a1 = copy(list->p2);
	    temp = 1;

	    stack = push(stack);
	    stack->p1 = a1;
	    stack->typ = temp;
	    list = list->p1;

	    break;
	case 14:
	    a1 = list->p2;
	    temp = 0;
	    stack = push(stack);
	    stack->p1 = a1;
	    stack->typ = temp;
	    list = list->p1;

	    break;
	}
    }

    return 0;
}

struct node *doop(int op, struct node *arg1, struct node *arg2)
{
    struct node *a1;
    struct node *a2;
    
    a1 = arg1;
    a2 = arg2;

    switch(op) {
    case 11:

	return sno_div(a1, a2);
    case 10:

	return mult(a1, a2);
    case 8:
	
	return add(a1, a2);
    case 9:

	return sub(a1, a2);
    case 7:
	
	return cat(a1, a2);
    }

    return 0;
}

struct node *execute(struct node *e)
{
    struct node *r;
    struct node *b;
    struct node *c;
    struct node *m;
    struct node *ca;
    struct node *d;
    struct node *a;

    r = e->p2;
    lc = e->ch;

    switch(e->typ) {
    case 0:
	/* r g */
	a = r->p1;
	delete(eval(r->p2, 1));

	if(rfail) {
	    rfail = 0;
	    b = a->p2;

	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;
	}

	b = a->p1;
    
	if(b == 0) {
	    return e->p1;
	}

	b = eval(b, 0);

	if(b == lookret) {
	    return 0;
	}

	if(b == lookfret) {
	    rfail = 1;

	    return 0;
	}

	if(b->typ != 2) {
	    writes("Attempt to transfer to non-label");
	}

	return b->p2;

    case 1:
	/* r m g */
	m = r->p1;
	a = m->p1;
	b = eval(r->p2, 1);
	c = search(m, b);
	delete(b);

	if(c == 0) {
	    rfail = 0;
	    b = a->p2;

	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;
	}

	sno_free(c);

	if(rfail) {
	    rfail = 0;
	    b = a->p2;

	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;
	}

	b = a->p1;
    
	if(b == 0) {
	    return e->p1;
	}

	b = eval(b, 0);

	if(b == lookret) {
	    return 0;
	}

	if(b == lookfret) {
	    rfail = 1;

	    return 0;
	}

	if(b->typ != 2) {
	    writes("Attempt to transfer to non-label");
	}

	return b->p2;
    case 2:
	/* r a g */
	ca = r->p1;
	a = ca->p1;
	b = eval(r->p2, 0);
	assign(b, eval(ca->p2, 1));
	if(rfail) {
	    rfail = 0;
	    b = a->p2;

	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;
	}

	b = a->p1;
    
	if(b == 0) {
	    return e->p1;
	}

	b = eval(b, 0);

	if(b == lookret) {
	    return 0;
	}

	if(b == lookfret) {
	    rfail = 1;

	    return 0;
	}

	if(b->typ != 2) {
	    writes("Attempt to transfer to non-label");
	}

	return b->p2;
    case 3:
	/* r m a g */
	m = r->p1;
	ca = m->p1;
	a = ca->p1;
	b = eval(r->p2, 0);
	d = search(m, b->p2);

	if(d == 0) {
	    rfail = 0;
	    b = a->p2;

	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;
	}

	c = eval(ca->p2, 1);

	if(d->p1 == 0) {
	    sno_free(d);
	    assign(b, cat(c, b->p2));
	    delete(c);

	    if(rfail) {
		rfail = 0;
		b = a->p2;

		if(b == 0) {
		    return e->p1;
		}

		b = eval(b, 0);

		if(b == lookret) {
		    return 0;
		}

		if(b == lookfret) {
		    rfail = 1;

		    return 0;
		}

		if(b->typ != 2) {
		    writes("Attempt to transfer to non-label");
		}

		return b->p2;
	    }

	    b = a->p1;
    
	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;
	}

	if(d->p2 == b->p2->p2) {
	    assign(b, c);
	    sno_free(d);
	    if(rfail) {
		rfail = 0;
		b = a->p2;

		if(b == 0) {
		    return e->p1;
		}

		b = eval(b, 0);

		if(b == lookret) {
		    return 0;
		}

		if(b == lookfret) {
		    rfail = 1;

		    return 0;
		}

		if(b->typ != 2) {
		    writes("Attempt to transfer to non-label");
		}

		return b->p2;
	    }

	    b = a->p1;
    
	    if(b == 0) {
		return e->p1;
	    }

	    b = eval(b, 0);

	    if(b == lookret) {
		return 0;
	    }

	    if(b == lookfret) {
		rfail = 1;

		return 0;
	    }

	    if(b->typ != 2) {
		writes("Attempt to transfer to non-label");
	    }

	    return b->p2;

	}

	r = alloc();
	r->p1 = d->p2->p1;
	r->p2 = b->p2->p2;
	assign(b, cat(c, r));
	sno_free(d);
	sno_free(r);
	delete(c);

    }

    if(rfail) {
	rfail = 0;
	b = a->p2;

	if(b == 0) {
	    return e->p1;
	}

	b = eval(b, 0);

	if(b == lookret) {
	    return 0;
	}

	if(b == lookfret) {
	    rfail = 1;

	    return 0;
	}

	if(b->typ != 2) {
	    writes("Attempt to transfer to non-label");
	}

	return b->p2;
    }

    b = a->p1;
    
    if(b == 0) {
	return e->p1;
    }

    b = eval(b, 0);

    if(b == lookret) {
	return 0;
    }

    if(b == lookfret) {
	rfail = 1;

	return 0;
    }

    if(b->typ != 2) {
	writes("Attempt to transfer to non-label");
    }

    return b->p2;
}

int assign(struct node *adr, struct node *val)
{
    struct node *a;
    struct node *addr;
    struct node *value;

    addr = adr;
    value = val;

    if(rfail == 1) {
	delete(value);

	return 0;
    }

    switch(addr->typ) {
    default:
	writes("Attempt to make an illegal assignment");
    case 0:
	addr->typ = 1;
    case 1:
	delete(addr->p2);
	addr->p2 = value;

	return 0;
    case 4:
	sysput(value);

	return 0;

    case 5:
	a = addr->p2->p1;
	delete(a->p2);
	a->p2 = value;

	return 0;
    }
}
