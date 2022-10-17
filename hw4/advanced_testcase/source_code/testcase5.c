#include<stdio.h>

void flag(int a, int b, int c, int d, int e, int f){
    if(a == 0xdead && b == 0xbeaf && c == -1 && d == 0x00abcdef && e == 0 && f == e){
        printf("FLAG{unix programming is fun!!}\n");
    }else{
        printf("NO NO NO\n");
    }
}

int main(void){
   flag(0,1,2,3,4,5); 
   return 0;
}