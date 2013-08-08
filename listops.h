/**
 *  Copyright 2013 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of L, a virtual machine for a lisp-like language.
 *
 *  L is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  L is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with L. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LISTOPS
#define _LISTOPS

#include "lisp.h"

//these functions DO NOT inc or dec refc.

inline int list_length(NODE *list) {
    int i = 0;
    while (list) {
        i++;
        list = asNODE(list->addr);
    }
    return i;
}

inline NODE* list_join(NODE *a, NODE *b) {
    NODE *head = a;
    while (a->addr) a = asNODE(a->addr);
    a->addr = asVALUE(b);
    return head;
}

inline NODE* list_reverse(NODE *list, NODE *tail) {
    if (!list) return tail;
    NODE* next = asNODE(list->addr);
    list->addr = asVALUE(tail);
    return list_reverse(next, list);
}

inline void list_push(void *val, NODE *&list) {
    list = newNODE(val,list);
}

inline VALUE* list_pop(NODE *&list) {
    failNIL(list,"NIL cannot be popped");
    VALUE *val = list->data;
    list = asNODE(list->addr);
    return val;
}

inline VALUE* list_peak(NODE *&list) {
    failNIL(list,"NIL cannot be peaked");
    return list->data;
}

inline NODE* list_reverse(NODE *list) {
    return list_reverse(list,NIL);
}

#endif
