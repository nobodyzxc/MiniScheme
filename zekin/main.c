#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "type.h"
#include "token.h"

int main(int args , char *argv[]){
    FILE *stream = stdin;
    if(args > 1)
        if(!(stream = fopen(argv[1] , "r")))
            printf("cannot open %s\n" , argv[1]) , exit(1);
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
