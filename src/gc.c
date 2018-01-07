#include "gc.h"
#include "util.h"
#include "mem.h"

ObjList head = NULL;

void gc_list_cons(Obj obj){
    ObjList inst = malloc(sizeof(obj_list_t));
    inst->v = obj;
    inst->next = head ? head : NULL;
    head = inst;
}

void mark(Obj);

void mark_tree(Symtree tree){
    if(!tree) return;
    mark(tree->sym);
    mark(tree->val);
    mark_tree(tree->lt);
    mark_tree(tree->rt);
}

void mark(Obj obj){
    if(!obj) return;
    if(obj->mark) return;
    obj->mark = true;
    if(obj->type == PAIR){
        mark(obj->pair->car);
        mark(obj->pair->cdr);
    }
    else if(obj->type == CLOSURE)
        mark(obj->clos->exp) , mark(obj->clos->env);
    else if(obj->type == MACRO)
        mark(obj->mac->keyws) , mark(obj->mac->rules);
    else if(obj->type == EXPR)
        free(obj->expr->name) ,
            mark(obj->expr->args) ,
            mark(obj->expr->body);
    else if(obj->type == ENV)
        mark_tree(obj->env->symtab) ,
            mark(obj->env->parent);
}

void sweep(){
    for(ObjList it = head , pre = NULL ;
            it ; pre = it , it = it->next){
        //if(it->v->mark) it->v->mark = false;
        if(!it->v->mark){
            ObjList toFree = it;
            it = it->next;
            if(pre) pre->next = it;
            if(toFree == head) head = toFree->next;
            free_obj(toFree->v) , free(toFree);
            if(pre) it = pre;
        }
    } // todo
}

void unmark(){
    for(ObjList it = head ; it ; it = it->next)
        it->v->mark = false;
}

void smark(){
    for(ObjList it = head ; it ; it = it->next)
        print_obj(it->v) , printf(" : %p\n" , it->v);
}

void rec(){
    for(ObjList it = head , pre = NULL ;
            it ; pre = it , it = it->next){
        fprintf(stderr , "%p " , it->v);
        falert(stderr , "obj : " , it->v);
        fputs("\n" , stderr);
    }
    fputs(">>>\n" , stderr);
}

void gc(){
    if(!head) return;
    mark(glenv);
    sweep();
    unmark();
}

void auto_try_gc(){
    static long long pre_obj_num = 0;
    long long before = get_obj_num();
    if(before > pre_obj_num * 2){
        gc();
        long long after = get_obj_num();
        printf("auto gc end , %d obj cleared , %d obj left\n" ,
                before - after , after);
        pre_obj_num = after;
    }
}
