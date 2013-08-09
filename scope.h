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

#ifndef _SCOPE
#define _SCOPE

#include "lisp.h"

// scope = (symtree . parent_scope)

NODE* scope_push(NODE *parent_scope);
NODE* scope_pop(NODE *scope);

NODE* scope_ref(SYMBOL *sym, NODE *scope);
VALUE* scope_resolve(SYMBOL *sym, NODE *scope);
void scope_bind(SYMBOL *sym, VALUE *val, NODE *scope);
void scope_bindMany(NODE *syms, NODE *vals, NODE *scope);

#endif
