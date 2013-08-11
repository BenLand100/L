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

VALUE* eval_string(char *prog_str, NODE *static_scope, NODE *macro_map) {
    NODE *prog = parseForms(prog_str);
    debugVal(prog,"before macroexpand: ");
    prog = (NODE*)macroexpand(prog,static_scope,macro_map);
    debugVal(prog,"after macroexpand: ");
    VALUE *val = evaluate((VALUE*)prog,static_scope);
    decRef(prog);
    return val;
} 

int main(int argc, char **argv) {
    NODE *static_scope = scope_push(NIL);
    NODE *macro_map = binmap(newSYMBOL(intern("NIL")),NIL);
    for (int i = 1; i < argc; i++) {
        debug("loading file: %s\n",argv[i]);
        FILE *f = fopen(argv[i],"rb");
        if (!f) error("Could not open file %s",argv[i]);
        fseek(f, 0, SEEK_END);
        size_t len = ftell(f);
        rewind(f);
        char *prog_str = malloc(len+1);
        fread(prog_str,1,len,f);
        prog_str[len] = '\0';
        fclose(f);
        debug("program(%i):\n%s",len,prog_str);
        VALUE *val = eval_string(prog_str,static_scope,macro_map);
        printVal(val,"result: ");
        decRef(val);
    }
}
