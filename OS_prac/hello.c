#include<stdio.h>
#include<unistd.h>

int main(){
    printf(" I am in hello.c\n");
    printf("PID of hello.c %d\n", getpid());
    return 0;
}
