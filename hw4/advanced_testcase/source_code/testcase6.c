#include<stdio.h>
#include<stdlib.h>

long rop_buf[1000];
void safe_leave(long rdi){
    if(rdi == 0){
        printf("safe leave\n");
        exit(0);
    }else{
        printf("You crash the process!!\n");
    }
}
void flag(long rdi, long rsi, long rdx){
    printf("FLAG{unix programming is fun!!}\n");
}

int main(void){
   register long* tmp1;
   register long tmp2;

   tmp1 = rop_buf;
   *tmp1 = tmp2;
   *tmp1 = tmp2;
   *tmp1 = tmp2;
   *tmp1 = tmp2;
   *tmp1 = tmp2;
   *tmp1 = tmp2;
   *tmp1 = tmp2;
   return 0;
}