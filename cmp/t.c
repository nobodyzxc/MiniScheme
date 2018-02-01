#include<stdio.h>
int main(void){
    long long acc = 0 , i = 1;
    while(i < 100000001)
        acc += i , i++;
    printf("%lld\n" , acc);
    return 0;
}
