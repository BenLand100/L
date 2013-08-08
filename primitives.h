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

#define PRIMS_LIST   0x00
#define PRIM_LAMBDA  PRIMS_LIST+0
#define PRIM_LIST    PRIMS_LIST+1
#define PRIM_QUOTE   PRIMS_LIST+2
#define PRIM_ADDR    PRIMS_LIST+3
#define PRIM_DATA    PRIMS_LIST+4

#define PRIMS_NUMBER PRIMS_LIST+5
#define PRIM_ADD     PRIMS_NUMBER+0
#define PRIM_SUB     PRIMS_NUMBER+1
#define PRIM_MUL     PRIMS_NUMBER+2
#define PRIM_DIV     PRIMS_NUMBER+3

NODE* list(NODE *args, NODE *scope);
VALUE* quote(NODE *args, NODE *scope);
VALUE* data(NODE *args, NODE *scope);
VALUE* addr(NODE *args, NODE *scope);
VALUE* add(NODE *args, NODE *scope);
VALUE* mul(NODE *args, NODE *scope);
VALUE* sub(NODE *args, NODE *scope);
VALUE* div(NODE *args, NODE *scope);

#endif 
