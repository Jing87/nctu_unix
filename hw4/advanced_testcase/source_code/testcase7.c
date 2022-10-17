#include<stdio.h>
#include<stdlib.h>

void safe_leave(long rdi){
    if(rdi == 0){
        printf("safe leave\n");
        exit(0);
    }else{
        printf("You crash the process!!\n");
    }
}
void flag1(long rdi, long rsi, long rdx){
    printf("FLAG{unix programming is fun!!}\n");
}
void flag2(long rdi, long rsi, long rdx){
    printf("JOP is a fantastic exploit technique!!}\n");
}

int main(void){
   return 0;
}