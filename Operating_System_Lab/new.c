#include <stdio.h>
#include <unistd.h> /* for fork() */
#include <signal.h> /* for signal(2) kill(2) */
#include <errno.h> /* for perror */
#include <stdlib.h>

int main(){
    int a=10;
    int b=13;
    pid_t id = fork();
    if(id==0){
        // child process
        a=30;
        b=40;
        printf("child process %d  , %d ", a,b);
    }
    else if(id>0){
        printf("Parent Process %d %d ",a,b);
    }
}