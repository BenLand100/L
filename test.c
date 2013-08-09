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
#include "parser.h"
#include "listops.h"

int main(int argc, char **argv) {
    NODE *forms = parseForms("(+ (+ 1 1) 2)");
    int len = list_length(forms);
    printf("Evaluating %i forms\n",len);
    debugVal(forms,"forms: ");
    for (NODE *form = forms; form; form = (NODE*)form->addr) {
        VALUE *val = evaluate(form->data,NIL);
        printVal(val);
        decRef(val);
    }
    decRef(forms);
}
