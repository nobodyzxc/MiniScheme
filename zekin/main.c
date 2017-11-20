#include <stdio.h>
#include <stdbool.h>
#include "type.h"
#include "token.h"

int main(int args , char *argv[]){
    char *text;
    FILE *stream = stdin;
    tokenize(stream);
    return 0;
}




/*   Obj obj = new_obj(INTEGER);
 *   obj->integer = 1;
 *   obj = cons(obj , cons(obj , nil));
     print_obj(obj) , puts("");
     print_obj(nil) , puts("");
    *
    *
    */
