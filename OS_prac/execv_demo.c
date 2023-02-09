#include<stdio.h>
#include<unistd.h>

int main(){
    printf("I am in execv_demo.c \n");
    printf(" PID of execv_demo.c is %d\n", getpid());
    char * args[]={"./hello", NULL};
    execv(args[0], args);
    printf("coming back to execv_demo.c"); //This will not be executed 
    return 0;
}