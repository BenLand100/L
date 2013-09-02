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

#include "parser.h"
#include "listops.h"
#include "primitives.h"
#include "binmap.h"
#include <ctype.h>

T_SYMBOL hash(char *sym) {
    T_SYMBOL hval = 0xAAAAAAAA;
    for (int i = 0; sym[i]; i++) hval ^= ((T_SYMBOL)sym[i]) << ((4*i)%24);
    return hval;
}

static NODE *sym_map = NIL;
static NODE *literal_map = NIL;
static NODE *literal_name_map = NIL;

#define addPrimFunc(sym,spec,func) { \
    binmap_put(newSYMBOL(intern(#sym)),newPRIMFUNC(spec,(NATIVE_FUNC)func),literal_map); \
    binmap_put(newPRIMFUNC(spec,(NATIVE_FUNC)func),newSTRING(#sym),literal_name_map); \
}
void parser_init() {
    debug("Defining built-in symbols\n");
    sym_map = binmap(newSYMBOL(hash("NIL")),newSTRING("NIL"));
    literal_map = binmap(newSYMBOL(intern("NIL")),NIL);
    literal_name_map = binmap(newPRIMFUNC(SPEC_LAMBDA,l_lambda),newSTRING(strdup("LAMBDA")));
    addPrimFunc(LAMBDA,SPEC_LAMBDA,l_lambda);
    addPrimFunc(PROG,SPEC_MACRO,l_prog);
    addPrimFunc(COND,SPEC_MACRO,l_cond);
    addPrimFunc(MACRO,SPEC_MACRODEF,l_macro);
    addPrimFunc(QUOTE,SPEC_QUOTE,l_quote);
    addPrimFunc(NODE,SPEC_FUNC,l_node);
    addPrimFunc(LIST,SPEC_MACRO,l_list);
    addPrimFunc(ADDR,SPEC_FUNC,l_addr);
    addPrimFunc(DATA,SPEC_FUNC,l_data);
    addPrimFunc(SETA,SPEC_FUNC,l_seta);
    addPrimFunc(SETD,SPEC_FUNC,l_setd);
    addPrimFunc(REF,SPEC_FUNC,l_ref);
    addPrimFunc(BIND,SPEC_FUNC,l_bind);
    addPrimFunc(+,SPEC_FUNC,l_add);
    addPrimFunc(-,SPEC_FUNC,l_sub);
    addPrimFunc(*,SPEC_FUNC,l_mul);
    addPrimFunc(/,SPEC_FUNC,l_div);
    addPrimFunc(PRINT,SPEC_FUNC,l_print);
    addPrimFunc(ISNODE,SPEC_FUNC,l_isnode);
}


const char* prim_str(PRIMFUNC *prim) {
    NODE *entry = binmap_find(prim,literal_name_map);
    if (entry) {
        const char* str = ((STRING*)entry->addr)->str;
        decRef(entry);
        return str;
    } else {
        return NIL;
    }
}

const char* sym_str(SYMBOL *sym) {
    NODE *entry = binmap_find(sym,sym_map);
    if (entry) {
        const char* str = ((STRING*)entry->addr)->str;
        decRef(entry);
        return str;
    } else {
        return NIL;
    }
}

T_SYMBOL intern(char *c_str) {
    if (!sym_map) parser_init();
    c_str = strdup(c_str);
    for (int i = 0; c_str[i]; i++) c_str[i] = toupper(c_str[i]);
    debug("intern: %s %u\n",c_str,hash(c_str));
    SYMBOL *sym = newSYMBOL(hash(c_str));
    STRING *str = newSTRING(c_str);
    NODE *entry;
    while ((entry = binmap_find(sym,sym_map))) {
        debugVal(entry,"matching: ");
        if (cmpSTRING((STRING*)entry->addr,str)) {
            decRef(entry);
            sym->sym++;
        } else {
            break;
        }
    }
    if (!entry) {
        debug("adding symbol: %s\n",c_str);
        binmap_put(sym,str,sym_map);
        return sym->sym;
    } else {
        decRef(sym);
        decRef(str);
        decRef(entry);
        return ((SYMBOL*)entry->data)->sym;
    }
}

NODE* parse(char **exp) {
    if (!sym_map) parser_init();
    debug("Parse List: %s\n",*exp);
    NODE *head = NIL;
    while (**exp) {
        switch (*((*exp)++)) {
            case '\'': {
                debug("to quote: %s\n",*exp);
                NODE *quoted = parse(exp);
                debugVal(quoted->data,"quoted: ");
                list_push(newNODE(newPRIMFUNC(SPEC_QUOTE,l_quote),newNODE(quoted->data,NIL)),&head);
                if (quoted->addr) head = list_join(list_reverse((NODE*)quoted->addr),head);
                head = list_reverse(head);
                debugVal(head,"expression: ");
                return head;
            }
            case '(':
                list_push(parse(exp),&head);
                break;
            case ')':
                head = list_reverse(head);
                debugVal(head,"expression: ");
                return head;
            case ';':
                while (**exp != '\0' && **exp != '\r' && **exp != '\n') (*exp)++;
                break;
            case '\n':
            case '\r':
            case '\t':
            case ' ':
                break;
            default: {
                char *sym = *exp-1;
                debug("origin: %s\n",sym);
                while (**exp && **exp != ' ' && **exp != ')' && **exp != '\n' && **exp != '\r' && **exp != '\t') (*exp)++;
                char old = **exp;
                **exp = 0;
                debug("literal: %s\n",sym);
                switch (sym[0]) {
                    case '+':
                    case '-':
                        if (!isdigit(sym[1])) {
                            list_push(newSYMBOL(intern(sym)),&head);
                            break;
                        }  
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        {
                            bool real = false;
                            char *scn = sym+1;
                            while (*scn) {
                                if (*scn == '.') { 
                                    real = true;
                                } else if (!isdigit(*scn)) {
                                    error("Malformed number character %c",*scn);
                                }
                                scn++;
                            }
                            if (real) {
                                list_push(newREAL(atof(sym)),&head);
                            } else {
                                list_push(newINTEGER(atoi(sym)),&head);
                            }
                        }
                        break;
                    default:
                        list_push(newSYMBOL(intern(sym)),&head);
                        break;
                 }
                 **exp = old;
                 if (head->data->type == ID_SYMBOL) {
                    NODE *literal;
                    if ((literal = binmap_find(head->data,literal_map))) {
                        NODE *last = (NODE*)head->addr;
                        incRef(last);
                        decRef(head);
                        incRef(literal->addr);
                        head = last;
                        list_push(literal->addr,&head);
                        decRef(literal);
                    }
                 }
                 debugVal(head,"parsed: ");
             }
        }
    }
    head = list_reverse(head);
    debugVal(head,"dangling: ");
    return head;
}

NODE* parseForms(char *exp) {
    char *dup = strdup(exp);
    char *org = dup;
    NODE *forms = parse(&dup);
    free(org);
    return newNODE(newPRIMFUNC(SPEC_MACRO,l_prog),forms);
}
