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

#ifndef _PRIMITIVES
#define _PRIMITIVES

#include "lisp.h"

#define PRIMS_INVOKE 0
#define PRIM_LAMBDA  PRIMS_INVOKE+0
#define PRIM_MACRO   PRIMS_INVOKE+1

#define PRIMS_CTRL   PRIMS_INVOKE+2
#define PRIM_PROG    PRIMS_CTRL+0
#define PRIM_COND    PRIMS_CTRL+1

#define PRIMS_LIST   PRIMS_CTRL+2
#define PRIM_LIST    PRIMS_LIST+0
#define PRIM_QUOTE   PRIMS_LIST+1
#define PRIM_NODE    PRIMS_LIST+2
#define PRIM_DATA    PRIMS_LIST+3
#define PRIM_ADDR    PRIMS_LIST+4
#define PRIM_SETA    PRIMS_LIST+5
#define PRIM_SETD    PRIMS_LIST+6

#define PRIMS_NUMBER PRIMS_LIST+7
#define PRIM_ADD     PRIMS_NUMBER+0
#define PRIM_SUB     PRIMS_NUMBER+1
#define PRIM_MUL     PRIMS_NUMBER+2
#define PRIM_DIV     PRIMS_NUMBER+3

#define PRIMS_SYMBOL PRIMS_NUMBER+4
#define PRIM_REF     PRIMS_SYMBOL+0
#define PRIM_BIND    PRIMS_SYMBOL+1

VALUE* lambda(NODE *args, NODE *scope);

VALUE* prog(NODE *args, NODE *scope);
VALUE* cond(NODE *args, NODE *scope);

NODE* list(NODE *args, NODE *scope);
VALUE* quote(NODE *args, NODE *scope);
VALUE* node(NODE *args, NODE *scope);
VALUE* data(NODE *args, NODE *scope);
VALUE* addr(NODE *args, NODE *scope);
VALUE* seta(NODE *args, NODE *scope);
VALUE* setd(NODE *args, NODE *scope);

VALUE* add(NODE *args, NODE *scope);
VALUE* mul(NODE *args, NODE *scope);
VALUE* sub(NODE *args, NODE *scope);
VALUE* div_(NODE *args, NODE *scope); //stdlib reserves div

VALUE* ref(NODE *args, NODE *scope);
VALUE* bind(NODE *args, NODE *scope);

#endif 
