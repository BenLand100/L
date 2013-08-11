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

#include "lisp.h"
#include "scope.h"
#include "parser.h"
#include "listops.h"
#include "binmap.h"

// "('((x y) (+ x y)) 5 7)"
// "(macro set (symbol value) (list 'seta (list 'ref (list 'quote symbol)) value)) (bind 'x NIL) (set x (+ 1 1))"

int main(int argc, char **argv) {
    NODE *prog = parseForms("(+ 1 1)");
    NODE *macros = binmap(newSYMBOL(intern("NIL")),NIL);
    NODE *macro_scope = scope_push(NIL);
    debugVal(prog,"before macroexpand: ");
    prog = (NODE*)macroexpand(prog,macro_scope,macros);
    debugVal(prog,"after macroexpand: ");
    NODE *scope = scope_push(NIL);
    VALUE *val = evaluate((VALUE*)prog,scope);
    printVal(val,"result: ");
    decRef(val);
    decRef(prog);
}
