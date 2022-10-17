#include<stdio.h>

void flag(int a){
    if(a == 0xdeadbeaf){
        printf("FLAG{unix programming is fun!!}\n");
    }else{
        printf("NO NO NO\n");
    }
}

int main(void){
   flag(0); 
   return 0;
}