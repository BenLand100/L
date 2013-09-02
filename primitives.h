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

VALUE* l_macro(NODE *args, NODE *scope);
VALUE* l_lambda(NODE *args, NODE *scope);

VALUE* l_prog(NODE *args, NODE *scope);
VALUE* l_cond(NODE *args, NODE *scope);

NODE* l_list(NODE *args, NODE *scope);
VALUE* l_quote(NODE *args, NODE *scope);
VALUE* l_node(NODE *args, NODE *scope);
VALUE* l_data(NODE *args, NODE *scope);
VALUE* l_addr(NODE *args, NODE *scope);
VALUE* l_seta(NODE *args, NODE *scope);
VALUE* l_setd(NODE *args, NODE *scope);

VALUE* l_add(NODE *args, NODE *scope);
VALUE* l_mul(NODE *args, NODE *scope);
VALUE* l_sub(NODE *args, NODE *scope);
VALUE* l_div(NODE *args, NODE *scope);

VALUE* l_ref(NODE *args, NODE *scope);
VALUE* l_bind(NODE *args, NODE *scope);

VALUE* l_print(NODE *args, NODE *scope);

VALUE* l_isnode(NODE *args, NODE *scope);

#endif 
