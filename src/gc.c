#include "gc.h"
#include "util.h"
#include "memory.h"

ObjList head = NULL;

void gc_list_cons(Obj obj){
    ObjList inst = MALLOC(sizeof(obj_list_t));
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
        mark(car(obj));
        mark(cdr(obj));
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
        if(!it->v->mark){
            ObjList toFree = it;
            it = it->next;
            if(pre) pre->next = it;
            if(toFree == head) head = toFree->next;
            free_obj(toFree->v) , free(toFree);
            if(pre) it = pre;
        }
    }
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
    long long pre = get_obj_num();
    mark(glenv);
    sweep();
    unmark();
    long long cur = get_obj_num();
#ifdef SHOW_GC_INFO
        printf("%d obj cleared,"
                " %d obj left , %d pr left\n" ,
                pre - cur , cur , pr_count);
#endif
}

void auto_try_gc(){
    static long long pre = 0;
    long long cur = get_obj_num();
    if(cur > pre * 2){
#ifdef SHOW_GC_INFO
        printf("auto gc end, ");
#endif
        gc();
        pre = get_obj_num();
    }
}
