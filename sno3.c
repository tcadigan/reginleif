#include "sno3.h"

#include "sno1.h"
#include "sno4.h"

int bextend(struct node *str, struct node *last)
{
    struct node *a;
    struct node *s;
    int b;
    int c;
    int d;
    int f;

    f = 1;
    s = str;
    
    if(s->p1 == NULL) {
	return 0;
    }

    d = 0;
    b = 0;
    a = s->p2;

    if(a == NULL) {
	a = s->p1;
	
	f = 0;
    }

    while(1) {
	if(f) {
	    if(a == last) {
		return 0;
	    }

	    a = a->p1;

	    f = 0;
	}

	++d;
	c = class(a->ch);

	/* '(' */
	if(c == 1) {
	    if(b == 0) {
		return 0;
	    }

	    --b;

	    if(b == 0) {
		s->p2 = a;

		return d;
	    }
	    
	    f = 1;
	    continue;
	}

	/* ')' */
	if(c == 2) {
	    ++b;

	    f = 1;

	    continue;
	}

	if(b == 0) {
	    s->p2 = a;

	    return d;
	}

	f = 1;
    }

    return 0;
}

int ubextend(struct node *str, struct node *last)
{
    struct node *a;
    struct node *b;
    struct node *s;

    s = str;
    a = s->p1;
    
    if(a == NULL) {
	return 0;
    }

    b = s->p2;

    if(b == NULL) {
	s->p2 = a;

	return 1;
    }

    if(b == last) {
	return 0;
    }

    a = b->p1;
    s->p2 = a;
    
    return 1;
}

struct node *search(struct node *arg, struct node *r)
{
    struct node *list;
    struct node *back;
    struct node *str;
    struct node *etc;
    struct node *next;
    struct node *last;
    struct node *base;
    struct node *e;
    struct node *a;
    struct node *b;
    struct node *var;
    int c;
    int d;

    a = arg->p2;
    base = alloc();
    list = base;
    next = NULL;
    last = NULL;

    while(1) {
	back = alloc();
	list->p2 = back;
	back->p1 = last;
	b = a->p2;
	c = a->typ;
	list->typ = c;

	if(c < 2) {
	    back->p2 = eval(b, 1);

	    a = a->p1;

	    if(a->typ == 0) {
		list->p1 = 0;
	
		if(rfail == 1) {
		    a = 0;
	    
		    list = base;

		    while(1) {
			back = list->p2;
			var = back->p2;
	
			if(list->typ < 2) {
			    delete(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	    
			    if(b == NULL) {
				return a;
			    }
	    
			    list = b;

			    continue;
			}
	
			str = var->p1;
			etc = var->p2;
	
			if((a != NULL) && (etc->p1 != NULL)) {
			    if(str->p2 == NULL) {
				sno_free(str);
				str = NULL;
			    }

			    assign(etc->p1, copy(str));
			}
	
			if(str) {
			    sno_free(str);
			}
	
			sno_free(etc);
			sno_free(var);
			sno_free(back);
			b = list->p1;
			sno_free(list);
	
			if(b == NULL) {
			    return a;
			}
	
			list = b;
		    }

		}

		list = base;

		if(r == NULL) {
		    last = NULL;
		    next = NULL;
		}
		else {
		    next = r->p1;
		    last = r->p2;
		}

		back = list->p2;
		var = back->p2;
		d = list->typ;

		if(d < 2) {
		    if(var == NULL) {
			goto advanc;
		    }

		    if(next == NULL) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == NULL) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }

		    a = next;
		    b = var->p1;
		    e = var->p2;

		    while(1) {
			if(a->ch != b->ch) {
			    while(1) {
				a = back->p1;
    
				if(a == NULL) {
				    rfail = 1;
				    list = base;

				    while(1) {
					back = list->p2;
					var = back->p2;
	
					if(list->typ < 2) {
					    delete(var);
					    sno_free(back);
					    b = list->p1;
					    sno_free(list);
	    
					    if(b == NULL) {
						return a;
					    }
	    
					    list = b;

					    continue;
					}
	
					str = var->p1;
					etc = var->p2;
	
					if((a != NULL) && (etc->p1 != NULL)) {
					    if(str->p2 == NULL) {
						sno_free(str);
						str = NULL;
					    }

					    assign(etc->p1, copy(str));
					}
	
					if(str) {
					    sno_free(str);
					}
	
					sno_free(etc);
					sno_free(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	
					if(b == NULL) {
					    return a;
					}
	
					list = b;
				    }
				}

				list = a;
				back = list->p2;
				var = back->p2;
				str = var->p1;
				etc = var->p2;

				if(etc->p2) {
				    continue;
				}

				if(var->typ == 1) {
				    if(bextend(str, last) == 0) {
					continue;
				    }

				    a = str->p2;

				    if(a == last) {
					next = NULL;
				    }
				    else {
					next = a->p1;
				    }

				    goto advanc;
				}

				if(ubextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;
    
				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}
			    }

			    goto advanc;
			}

			if(b == e) {
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }

			    goto advanc;
			}

			if(a == last) {
			    while(1) {
				a = back->p1;
    
				if(a == NULL) {
				    rfail = 1;
				    list = base;

				    while(1) {
					back = list->p2;
					var = back->p2;
	
					if(list->typ < 2) {
					    delete(var);
					    sno_free(back);
					    b = list->p1;
					    sno_free(list);
	    
					    if(b == NULL) {
						return a;
					    }
	    
					    list = b;

					    continue;
					}
	
					str = var->p1;
					etc = var->p2;
	
					if((a != NULL) && (etc->p1 != NULL)) {
					    if(str->p2 == NULL) {
						sno_free(str);
						str = NULL;
					    }

					    assign(etc->p1, copy(str));
					}
	
					if(str) {
					    sno_free(str);
					}
	
					sno_free(etc);
					sno_free(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	
					if(b == NULL) {
					    return a;
					}
	
					list = b;
				    }
				}

				list = a;
				back = list->p2;
				var = back->p2;
				str = var->p1;
				etc = var->p2;

				if(etc->p2) {
				    continue;
				}

				if(var->typ == 1) {
				    if(bextend(str, last) == 0) {
					continue;
				    }

				    a = str->p2;

				    if(a == last) {
					next = NULL;
				    }
				    else {
					next = a->p1;
				    }

				    goto advanc;
				}

				if(ubextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;
    
				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}
			    }

			    goto advanc;
			}

			a = a->p1;
			b = b->p1;
		    }
		}

		str = var->p1;
		etc = var->p2;
		str->p1 = next;
		str->p1 = NULL;

		if(var->typ == 1) {
		    d = bextend(str, last);

		    if(d == 0) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == NULL) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }

		    if(etc->p2 == NULL) {
			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}
	    
			goto advanc;
		    }

		    while(1) {
			c -= d;

			if(c == d) {
			    a = str->p2;

			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }

			    goto advanc;
			}

			if(c < 0) {
			    while(1) {
				a = back->p1;
    
				if(a == NULL) {
				    rfail = 1;
				    list = base;

				    while(1) {
					back = list->p2;
					var = back->p2;
	
					if(list->typ < 2) {
					    delete(var);
					    sno_free(back);
					    b = list->p1;
					    sno_free(list);
	    
					    if(b == NULL) {
						return a;
					    }
	    
					    list = b;

					    continue;
					}
	
					str = var->p1;
					etc = var->p2;
	
					if((a != NULL) && (etc->p1 != NULL)) {
					    if(str->p2 == NULL) {
						sno_free(str);
						str = NULL;
					    }

					    assign(etc->p1, copy(str));
					}
	
					if(str) {
					    sno_free(str);
					}
	
					sno_free(etc);
					sno_free(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	
					if(b == NULL) {
					    return a;
					}
	
					list = b;
				    }
				}

				list = a;
				back = list->p2;
				var = back->p2;
				str = var->p1;
				etc = var->p2;

				if(etc->p2) {
				    continue;
				}

				if(var->typ == 1) {
				    if(bextend(str, last) == 0) {
					continue;
				    }

				    a = str->p2;

				    if(a == last) {
					next = NULL;
				    }
				    else {
					next = a->p1;
				    }

				    goto advanc;
				}

				if(ubextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;
    
				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}
			    }

			    goto advanc;
			}

			d = bextend(str, last);
       
			if(d == 0) {
			    while(1) {
				a = back->p1;
    
				if(a == NULL) {
				    rfail = 1;
				    list = base;

				    while(1) {
					back = list->p2;
					var = back->p2;
	
					if(list->typ < 2) {
					    delete(var);
					    sno_free(back);
					    b = list->p1;
					    sno_free(list);
	    
					    if(b == NULL) {
						return a;
					    }
	    
					    list = b;

					    continue;
					}
	
					str = var->p1;
					etc = var->p2;
	
					if((a != NULL) && (etc->p1 != NULL)) {
					    if(str->p2 == NULL) {
						sno_free(str);
						str = NULL;
					    }

					    assign(etc->p1, copy(str));
					}
	
					if(str) {
					    sno_free(str);
					}
	
					sno_free(etc);
					sno_free(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	
					if(b == NULL) {
					    return a;
					}
	
					list = b;
				    }
				}

				list = a;
				back = list->p2;
				var = back->p2;
				str = var->p1;
				etc = var->p2;

				if(etc->p2) {
				    continue;
				}

				if(var->typ == 1) {
				    if(bextend(str, last) == 0) {
					continue;
				    }

				    a = str->p2;

				    if(a == last) {
					next = NULL;
				    }
				    else {
					next = a->p1;
				    }

				    goto advanc;
				}

				if(ubextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;
    
				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}
			    }

			    goto advanc;
			}
		    }
		}

		if(c == 0) {
		    if((d == 3) && (next != NULL)) {
			str->p2 = last;
			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}
		    }

		    goto advanc;
		}

	    	while(c) {
		    --c;

		    if(ubextend(str, last) == 0) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == 0) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = 0;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }
		}

		a = str->p2;

		if(a == last) {
		    next = NULL;
		}
		else {
		    next = a->p1;
		}

		goto advanc;
	    }

	    b = alloc();
	    list->p1 = b;
	    list = b;

	    continue;
	}

	last = list;
	str = alloc();
	etc = alloc();
	var = alloc();
	back->p2 = var;
	var->typ = b->typ;
	var->p1 = str;
	var->p2 = etc;
	e = b->p1;

	if(e == NULL) {
	    etc->p1 = NULL;
	}
	else {
	    etc->p1 = eval(e, 0);
	}

	e = b->p2;
    
	if(e == 0) {
	    etc->p2 = NULL;
	}
	else {
	    e = eval(e, 1);
	    etc->p2 = e;
	    delete(e);
	}

	a = a->p1;

	if(a->typ == 0) {
	    list->p1 = NULL;
	
	    if(rfail == 1) {
		a = NULL;
	    
		list = base;

		while(1) {
		    back = list->p2;
		    var = back->p2;
	
		    if(list->typ < 2) {
			delete(var);
			sno_free(back);
			b = list->p1;
			sno_free(list);
	    
			if(b == NULL) {
			    return a;
			}
	    
			list = b;

			continue;
		    }
	
		    str = var->p1;
		    etc = var->p2;
	
		    if((a != NULL) && (etc->p1 != NULL)) {
			if(str->p2 == 0) {
			    sno_free(str);
			    str = NULL;
			}

			assign(etc->p1, copy(str));
		    }
	
		    if(str) {
			sno_free(str);
		    }
	
		    sno_free(etc);
		    sno_free(var);
		    sno_free(back);
		    b = list->p1;
		    sno_free(list);
	
		    if(b == NULL) {
			return a;
		    }
	
		    list = b;
		}

	    }

	    list = base;

	    if(r == NULL) {
		last = NULL;
		next = NULL;
	    }
	    else {
		next = r->p1;
		last = r->p2;
	    }

	    back = list->p2;
	    var = back->p2;
	    d = list->typ;

	    if(d < 2) {
		if(var == NULL) {
		    goto advanc;
		}

		if(next == NULL) {
		    while(1) {
			a = back->p1;
    
			if(a == NULL) {
			    rfail = 1;
			    list = base;

			    while(1) {
				back = list->p2;
				var = back->p2;
	
				if(list->typ < 2) {
				    delete(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	    
				    if(b == NULL) {
					return a;
				    }
	    
				    list = b;

				    continue;
				}
	
				str = var->p1;
				etc = var->p2;
	
				if((a != NULL) && (etc->p1 != NULL)) {
				    if(str->p2 == NULL) {
					sno_free(str);
					str = NULL;
				    }

				    assign(etc->p1, copy(str));
				}
	
				if(str) {
				    sno_free(str);
				}
	
				sno_free(etc);
				sno_free(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	
				if(b == NULL) {
				    return a;
				}
	
				list = b;
			    }
			}

			list = a;
			back = list->p2;
			var = back->p2;
			str = var->p1;
			etc = var->p2;

			if(etc->p2) {
			    continue;
			}

			if(var->typ == 1) {
			    if(bextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;

			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }

			    goto advanc;
			}

			if(ubextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;
    
			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}
		    }

		    goto advanc;
		}

		a = next;
		b = var->p1;
		e = var->p2;

		while(1) {
		    if(a->ch != b->ch) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == NULL) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }

		    if(b == e) {
			if(a == last) {
			    next = 0;
			}
			else {
			    next = a->p1;
			}

			goto advanc;
		    }

		    if(a == last) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == NULL) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }

		    a = a->p1;
		    b = b->p1;
		}
	    }

	    str = var->p1;
	    etc = var->p2;
	    str->p1 = next;
	    str->p1 = NULL;

	    if(var->typ == 1) {
		d = bextend(str, last);

		if(d == 0) {
		    while(1) {
			a = back->p1;
    
			if(a == NULL) {
			    rfail = 1;
			    list = base;

			    while(1) {
				back = list->p2;
				var = back->p2;
	
				if(list->typ < 2) {
				    delete(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	    
				    if(b == NULL) {
					return a;
				    }
	    
				    list = b;

				    continue;
				}
	
				str = var->p1;
				etc = var->p2;
	
				if((a != NULL) && (etc->p1 != NULL)) {
				    if(str->p2 == NULL) {
					sno_free(str);
					str = NULL;
				    }

				    assign(etc->p1, copy(str));
				}
	
				if(str) {
				    sno_free(str);
				}
	
				sno_free(etc);
				sno_free(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	
				if(b == NULL) {
				    return a;
				}
	
				list = b;
			    }
			}

			list = a;
			back = list->p2;
			var = back->p2;
			str = var->p1;
			etc = var->p2;

			if(etc->p2) {
			    continue;
			}

			if(var->typ == 1) {
			    if(bextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;

			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }

			    goto advanc;
			}

			if(ubextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;
    
			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}
		    }

		    goto advanc;
		}

		if(etc->p2 == NULL) {
		    a = str->p2;

		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }
	    
		    goto advanc;
		}

		while(1) {
		    c -= d;

		    if(c == d) {
			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}

			goto advanc;
		    }

		    if(c < 0) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == NULL) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }

		    d = bextend(str, last);
       
		    if(d == 0) {
			while(1) {
			    a = back->p1;
    
			    if(a == NULL) {
				rfail = 1;
				list = base;

				while(1) {
				    back = list->p2;
				    var = back->p2;
	
				    if(list->typ < 2) {
					delete(var);
					sno_free(back);
					b = list->p1;
					sno_free(list);
	    
					if(b == NULL) {
					    return a;
					}
	    
					list = b;

					continue;
				    }
	
				    str = var->p1;
				    etc = var->p2;
	
				    if((a != NULL) && (etc->p1 != NULL)) {
					if(str->p2 == NULL) {
					    sno_free(str);
					    str = NULL;
					}

					assign(etc->p1, copy(str));
				    }
	
				    if(str) {
					sno_free(str);
				    }
	
				    sno_free(etc);
				    sno_free(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	
				    if(b == NULL) {
					return a;
				    }
	
				    list = b;
				}
			    }

			    list = a;
			    back = list->p2;
			    var = back->p2;
			    str = var->p1;
			    etc = var->p2;

			    if(etc->p2) {
				continue;
			    }

			    if(var->typ == 1) {
				if(bextend(str, last) == 0) {
				    continue;
				}

				a = str->p2;

				if(a == last) {
				    next = NULL;
				}
				else {
				    next = a->p1;
				}

				goto advanc;
			    }

			    if(ubextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;
    
			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }
			}

			goto advanc;
		    }
		}
	    }

	    if(c == 0) {
		if((d == 3) && (next != NULL)) {
		    str->p2 = last;
		    a = str->p2;

		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }
		}

		goto advanc;
	    }

	    while(c) {
		--c;

		if(ubextend(str, last) == 0) {
		    while(1) {
			a = back->p1;
    
			if(a == NULL) {
			    rfail = 1;
			    list = base;

			    while(1) {
				back = list->p2;
				var = back->p2;
	
				if(list->typ < 2) {
				    delete(var);
				    sno_free(back);
				    b = list->p1;
				    sno_free(list);
	    
				    if(b == NULL) {
					return a;
				    }
	    
				    list = b;

				    continue;
				}
	
				str = var->p1;
				etc = var->p2;
	
				if((a != NULL) && (etc->p1 != NULL)) {
				    if(str->p2 == NULL) {
					sno_free(str);
					str = NULL;
				    }

				    assign(etc->p1, copy(str));
				}
	
				if(str) {
				    sno_free(str);
				}
	
				sno_free(etc);
				sno_free(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	
				if(b == NULL) {
				    return a;
				}
	
				list = b;
			    }
			}

			list = a;
			back = list->p2;
			var = back->p2;
			str = var->p1;
			etc = var->p2;

			if(etc->p2) {
			    continue;
			}

			if(var->typ == 1) {
			    if(bextend(str, last) == 0) {
				continue;
			    }

			    a = str->p2;

			    if(a == last) {
				next = NULL;
			    }
			    else {
				next = a->p1;
			    }

			    goto advanc;
			}

			if(ubextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;
    
			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}
		    }

		    goto advanc;
		}
	    }

	    a = str->p2;

	    if(a == last) {
		next = NULL;
	    }
	    else {
		next = a->p1;
	    }

	    goto advanc;
	}

	b = alloc();
	list->p1 = b;
	list = b;
    }

    while(1) {
	a = back->p1;
    
	if(a == NULL) {
	    rfail = 1;
	    list = base;

	    while(1) {
		back = list->p2;
		var = back->p2;
	
		if(list->typ < 2) {
		    delete(var);
		    sno_free(back);
		    b = list->p1;
		    sno_free(list);
	    
		    if(b == NULL) {
			return a;
		    }
	    
		    list = b;

		    continue;
		}
	
		str = var->p1;
		etc = var->p2;
	
		if((a != NULL) && (etc->p1 != NULL)) {
		    if(str->p2 == NULL) {
			sno_free(str);
			str = NULL;
		    }

		    assign(etc->p1, copy(str));
		}
	
		if(str) {
		    sno_free(str);
		}
	
		sno_free(etc);
		sno_free(var);
		sno_free(back);
		b = list->p1;
		sno_free(list);
	
		if(b == NULL) {
		    return a;
		}
	
		list = b;
	    }
	}

	list = a;
	back = list->p2;
	var = back->p2;
	str = var->p1;
	etc = var->p2;

	if(etc->p2) {
	    continue;
	}

	if(var->typ == 1) {
	    if(bextend(str, last) == 0) {
		continue;
	    }

	    a = str->p2;

	    if(a == last) {
		next = NULL;
	    }
	    else {
		next = a->p1;
	    }

	    goto advanc;
	}

	if(ubextend(str, last) == 0) {
	    continue;
	}

	a = str->p2;
    
	if(a == last) {
	    next = NULL;
	}
	else {
	    next = a->p1;
	}
    }

 advanc:
    a = list->p1;
    
    if(a == NULL) {
	a = alloc();
	
	if(r == NULL) {
	    a->p2 = NULL;
	    a->p1 = NULL;
	    list = base;

	    while(1) {
		back = list->p2;
		var = back->p2;
	
		if(list->typ < 2) {
		    delete(var);
		    sno_free(back);
		    b = list->p1;
		    sno_free(list);
	    
		    if(b == NULL) {
			return a;
		    }
	    
		    list = b;

		    continue;
		}
	
		str = var->p1;
		etc = var->p2;
	
		if((a != NULL) && (etc->p1 != NULL)) {
		    if(str->p2 == NULL) {
			sno_free(str);
			str = NULL;
		    }

		    assign(etc->p1, copy(str));
		}
	
		if(str) {
		    sno_free(str);
		}
	
		sno_free(etc);
		sno_free(var);
		sno_free(back);
		b = list->p1;
		sno_free(list);
	
		if(b == NULL) {
		    return a;
		}
	
		list = b;
	    }	  
	}

	b = r->p1;
	a->p1 = b;

	if(next == NULL) {
	    a->p2 = r->p2;
	    list = base;

	    while(1) {
		back = list->p2;
		var = back->p2;
	
		if(list->typ < 2) {
		    delete(var);
		    sno_free(back);
		    b = list->p1;
		    sno_free(list);
	    
		    if(b == NULL) {
			return a;
		    }
	    
		    list = b;

		    continue;
		}
	
		str = var->p1;
		etc = var->p2;
	
		if((a != NULL) && (etc->p1 != NULL)) {
		    if(str->p2 == NULL) {
			sno_free(str);
			str = NULL;
		    }

		    assign(etc->p1, copy(str));
		}
	
		if(str) {
		    sno_free(str);
		}
	
		sno_free(etc);
		sno_free(var);
		sno_free(back);
		b = list->p1;
		sno_free(list);
	
		if(b == NULL) {
		    return a;
		}
	
		list = b;
	    }
	}

	while(1) {
	    e = b->p1;
	    
	    if(e == next) {
		a->p2 = b;
		list = base;

		while(1) {
		    back = list->p2;
		    var = back->p2;
	
		    if(list->typ < 2) {
			delete(var);
			sno_free(back);
			b = list->p1;
			sno_free(list);
	    
			if(b == NULL) {
			    return a;
			}
	    
			list = b;

			continue;
		    }
	
		    str = var->p1;
		    etc = var->p2;
	
		    if((a != NULL) && (etc->p1 != NULL)) {
			if(str->p2 == NULL) {
			    sno_free(str);
			    str = NULL;
			}

			assign(etc->p1, copy(str));
		    }
	
		    if(str) {
			sno_free(str);
		    }
	
		    sno_free(etc);
		    sno_free(var);
		    sno_free(back);
		    b = list->p1;
		    sno_free(list);
	
		    if(b == NULL) {
			return a;
		    }
	
		    list = b;
		}
	    }

	    b = e;
	}
    }

    list = a;
    back = list->p2;
    var = back->p2;
    d = list->typ;

    if(d < 2) {
	if(var == NULL) {
	    goto advanc;
	}

	if(next == NULL) {
	    while(1) {
		a = back->p1;
    
		if(a == NULL) {
		    rfail = 1;
		    list = base;

		    while(1) {
			back = list->p2;
			var = back->p2;
	
			if(list->typ < 2) {
			    delete(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	    
			    if(b == NULL) {
				return a;
			    }
	    
			    list = b;

			    continue;
			}
	
			str = var->p1;
			etc = var->p2;
	
			if((a != NULL) && (etc->p1 != NULL)) {
			    if(str->p2 == NULL) {
				sno_free(str);
				str = NULL;
			    }

			    assign(etc->p1, copy(str));
			}
	
			if(str) {
			    sno_free(str);
			}
	
			sno_free(etc);
			sno_free(var);
			sno_free(back);
			b = list->p1;
			sno_free(list);
	
			if(b == NULL) {
			    return a;
			}
	
			list = b;
		    }
		}

		list = a;
		back = list->p2;
		var = back->p2;
		str = var->p1;
		etc = var->p2;

		if(etc->p2) {
		    continue;
		}

		if(var->typ == 1) {
		    if(bextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;

		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }

		    break;
		}

		if(ubextend(str, last) == 0) {
		    continue;
		}

		a = str->p2;
    
		if(a == last) {
		    next = NULL;
		}
		else {
		    next = a->p1;
		}
	    }

	    goto advanc;
	}

	a = next;
	b = var->p1;
	e = var->p2;

	while(1) {
	    if(a->ch != b->ch) {
		while(1) {
		    a = back->p1;
    
		    if(a == NULL) {
			rfail = 1;
			list = base;

			while(1) {
			    back = list->p2;
			    var = back->p2;
	
			    if(list->typ < 2) {
				delete(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	    
				if(b == NULL) {
				    return a;
				}
	    
				list = b;

				continue;
			    }
	
			    str = var->p1;
			    etc = var->p2;
	
			    if((a != NULL) && (etc->p1 != NULL)) {
				if(str->p2 == NULL) {
				    sno_free(str);
				    str = NULL;
				}

				assign(etc->p1, copy(str));
			    }
	
			    if(str) {
				sno_free(str);
			    }
	
			    sno_free(etc);
			    sno_free(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	
			    if(b == NULL) {
				return a;
			    }
	
			    list = b;
			}
		    }

		    list = a;
		    back = list->p2;
		    var = back->p2;
		    str = var->p1;
		    etc = var->p2;

		    if(etc->p2) {
			continue;
		    }

		    if(var->typ == 1) {
			if(bextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}

			break;
		    }

		    if(ubextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;
    
		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }
		}

		goto advanc;
	    }

	    if(b == e) {
		if(a == last) {
		    next = NULL;
		}
		else {
		    next = a->p1;
		}

		goto advanc;
	    }

	    if(a == last) {
		while(1) {
		    a = back->p1;
    
		    if(a == NULL) {
			rfail = 1;
			list = base;

			while(1) {
			    back = list->p2;
			    var = back->p2;
	
			    if(list->typ < 2) {
				delete(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	    
				if(b == NULL) {
				    return a;
				}
	    
				list = b;

				continue;
			    }
	
			    str = var->p1;
			    etc = var->p2;
	
			    if((a != NULL) && (etc->p1 != NULL)) {
				if(str->p2 == NULL) {
				    sno_free(str);
				    str = NULL;
				}

				assign(etc->p1, copy(str));
			    }
	
			    if(str) {
				sno_free(str);
			    }
	
			    sno_free(etc);
			    sno_free(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	
			    if(b == NULL) {
				return a;
			    }
	
			    list = b;
			}
		    }

		    list = a;
		    back = list->p2;
		    var = back->p2;
		    str = var->p1;
		    etc = var->p2;

		    if(etc->p2) {
			continue;
		    }

		    if(var->typ == 1) {
			if(bextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}

			break;
		    }

		    if(ubextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;
    
		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }
		}

		goto advanc;
	    }

	    a = a->p1;
	    b = b->p1;
	}
    }

    str = var->p1;
    etc = var->p2;
    str->p1 = next;
    str->p1 = NULL;

    if(var->typ == 1) {
	d = bextend(str, last);

	if(d == 0) {
	    while(1) {
		a = back->p1;
    
		if(a == NULL) {
		    rfail = 1;
		    list = base;

		    while(1) {
			back = list->p2;
			var = back->p2;
	
			if(list->typ < 2) {
			    delete(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	    
			    if(b == NULL) {
				return a;
			    }
	    
			    list = b;

			    continue;
			}
	
			str = var->p1;
			etc = var->p2;
	
			if((a != NULL) && (etc->p1 != NULL)) {
			    if(str->p2 == NULL) {
				sno_free(str);
				str = NULL;
			    }

			    assign(etc->p1, copy(str));
			}
	
			if(str) {
			    sno_free(str);
			}
	
			sno_free(etc);
			sno_free(var);
			sno_free(back);
			b = list->p1;
			sno_free(list);
	
			if(b == NULL) {
			    return a;
			}
	
			list = b;
		    }
		}

		list = a;
		back = list->p2;
		var = back->p2;
		str = var->p1;
		etc = var->p2;

		if(etc->p2) {
		    continue;
		}

		if(var->typ == 1) {
		    if(bextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;

		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }

		    break;
		}

		if(ubextend(str, last) == 0) {
		    continue;
		}

		a = str->p2;
    
		if(a == last) {
		    next = NULL;
		}
		else {
		    next = a->p1;
		}
	    }

	    goto advanc;
	}

	if(etc->p2 == NULL) {
	    a = str->p2;

	    if(a == last) {
		next = NULL;
	    }
	    else {
		next = a->p1;
	    }
	    
	    goto advanc;
	}

	while(1) {
	    c -= d;

	    if(c == d) {
		a = str->p2;

		if(a == last) {
		    next = NULL;
		}
		else {
		    next = a->p1;
		}

		goto advanc;
	    }

	    if(c < 0) {
		while(1) {
		    a = back->p1;
    
		    if(a == NULL) {
			rfail = 1;
			list = base;

			while(1) {
			    back = list->p2;
			    var = back->p2;
	
			    if(list->typ < 2) {
				delete(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	    
				if(b == NULL) {
				    return a;
				}
	    
				list = b;

				continue;
			    }
	
			    str = var->p1;
			    etc = var->p2;
	
			    if((a != NULL) && (etc->p1 != NULL)) {
				if(str->p2 == NULL) {
				    sno_free(str);
				    str = NULL;
				}

				assign(etc->p1, copy(str));
			    }
	
			    if(str) {
				sno_free(str);
			    }
	
			    sno_free(etc);
			    sno_free(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	
			    if(b == NULL) {
				return a;
			    }
	
			    list = b;
			}
		    }

		    list = a;
		    back = list->p2;
		    var = back->p2;
		    str = var->p1;
		    etc = var->p2;

		    if(etc->p2) {
			continue;
		    }

		    if(var->typ == 1) {
			if(bextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}

			break;
		    }

		    if(ubextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;
    
		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }
		}

		goto advanc;
	    }

	    d = bextend(str, last);
       
	    if(d == 0) {
		while(1) {
		    a = back->p1;
    
		    if(a == NULL) {
			rfail = 1;
			list = base;

			while(1) {
			    back = list->p2;
			    var = back->p2;
	
			    if(list->typ < 2) {
				delete(var);
				sno_free(back);
				b = list->p1;
				sno_free(list);
	    
				if(b == NULL) {
				    return a;
				}
	    
				list = b;

				continue;
			    }
	
			    str = var->p1;
			    etc = var->p2;
	
			    if((a != NULL) && (etc->p1 != NULL)) {
				if(str->p2 == NULL) {
				    sno_free(str);
				    str = NULL;
				}

				assign(etc->p1, copy(str));
			    }
	
			    if(str) {
				sno_free(str);
			    }
	
			    sno_free(etc);
			    sno_free(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	
			    if(b == NULL) {
				return a;
			    }
	
			    list = b;
			}
		    }

		    list = a;
		    back = list->p2;
		    var = back->p2;
		    str = var->p1;
		    etc = var->p2;

		    if(etc->p2) {
			continue;
		    }

		    if(var->typ == 1) {
			if(bextend(str, last) == 0) {
			    continue;
			}

			a = str->p2;

			if(a == last) {
			    next = NULL;
			}
			else {
			    next = a->p1;
			}

			break;
		    }

		    if(ubextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;
    
		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }
		}

		goto advanc;
	    }
	}
    }

    if(c == 0) {
	if((d == 3) && (next != NULL)) {
	    str->p2 = last;
	    a = str->p2;

	    if(a == last) {
		next = NULL;
	    }
	    else {
		next = a->p1;
	    }
	}

	goto advanc;
    }

    while(c) {
	--c;

	if(ubextend(str, last) == 0) {
	    while(1) {
		a = back->p1;
    
		if(a == NULL) {
		    rfail = 1;
		    list = base;

		    while(1) {
			back = list->p2;
			var = back->p2;
	
			if(list->typ < 2) {
			    delete(var);
			    sno_free(back);
			    b = list->p1;
			    sno_free(list);
	    
			    if(b == NULL) {
				return a;
			    }
	    
			    list = b;

			    continue;
			}
	
			str = var->p1;
			etc = var->p2;
	
			if((a != NULL) && (etc->p1 != NULL)) {
			    if(str->p2 == NULL) {
				sno_free(str);
				str = NULL;
			    }

			    assign(etc->p1, copy(str));
			}
	
			if(str) {
			    sno_free(str);
			}
	
			sno_free(etc);
			sno_free(var);
			sno_free(back);
			b = list->p1;
			sno_free(list);
	
			if(b == NULL) {
			    return a;
			}
	
			list = b;
		    }
		}

		list = a;
		back = list->p2;
		var = back->p2;
		str = var->p1;
		etc = var->p2;

		if(etc->p2) {
		    continue;
		}

		if(var->typ == 1) {
		    if(bextend(str, last) == 0) {
			continue;
		    }

		    a = str->p2;

		    if(a == last) {
			next = NULL;
		    }
		    else {
			next = a->p1;
		    }

		    break;
		}

		if(ubextend(str, last) == 0) {
		    continue;
		}

		a = str->p2;
    
		if(a == last) {
		    next = NULL;
		}
		else {
		    next = a->p1;
		}
	    }

	    goto advanc;
	}
    }

    a = str->p2;

    if(a == last) {
	next = NULL;
    }
    else {
	next = a->p1;
    }

    goto advanc;
}
