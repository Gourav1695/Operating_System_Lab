// 2020CSB010 GOURAV KUMAR SHAW

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <sys/shm.h> /* for shmget() shmctl() */  
#include <signal.h> /* for signal(2) kill(2) */
#include <time.h>

typedef void (*sighandler_t)(int);
typedef struct att
{
    int roll;
    time_t tm;

} att;
int shmid;

int main(int argc, char *argv[])
{
    int projid=1;
    int n=atoi(argv[1]);
    key_t shmkey=ftok("./file.txt",projid);
    shmid=shmget(shmkey,sizeof(att), 0666);
    printf("%d shmid\n",shmid);
    if(shmid<0)
    {
        perror("shmget");
        return 1;
    }
    att * shm;
    shm=shmat(shmid,NULL,0);
    while(1)
    {
        if(shm->roll==-1)
        {
            shm->roll=n;// here n is number of students
            shm->tm=time(NULL);
            break;
        }
        shm=shm+1;
    }
    shmdt(shm);
    return 0;
    
}