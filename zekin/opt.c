#include "opt.h"
#define TAB_SIZE 1000

int hash_cnt = 0;

MapObj sym_hashtab[TAB_SIZE] = {
    [0 ... TAB_SIZE - 1].key = NULL ,
    [0 ... TAB_SIZE - 1].val = NULL ,
};

/* djb2 ref from
 * http://www.cse.yorku.ca/~oz/hash.html */
unsigned long
hash(unsigned char *str) {
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    /* hash * 33 + c */
    return hash;
}


Obj lookup_sym_pool(char *s){
    int hc = hash(s) % TAB_SIZE;
    while(sym_hashtab[hc].key &&
            !EQS(sym_hashtab[hc].key , s))
        hc++;
    return sym_hashtab[hc].key ?
        sym_hashtab[hc].val : NULL;
}

void push_sym_pool(Obj sym){
    if(hash_cnt > TAB_SIZE){
        puts("cannot opt : tab size is not enough");
        return;
    }
    int hc = hash(sym->str) % TAB_SIZE;
    while(sym_hashtab[hc].key) hc++;
    sym_hashtab[hc].key = sym->str;
    sym_hashtab[hc].val = sym;
    hash_cnt++;
}
