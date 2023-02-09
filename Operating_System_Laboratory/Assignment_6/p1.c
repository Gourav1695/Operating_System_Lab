// 2020CSB010 GOURAV KUMAR SHAW

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <unistd.h>
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h>  /* for signal(2) kill(2) */
#include <time.h>
 
sem_t sem2;

int main()
{
    sem_init(&sem2, 0, 1);//second argument 0, then the semaphore  is  shared  between
    //    the  threads  of  a  process, and should be located at some address
    //    that is visible to all threads 
    //    third argument 1, which means that it is available for use by the first process that acquires it.
    int shmid;
    int projid = 1;
    key_t shmkey = ftok("./file.txt", projid);
    shmid = shmget(shmkey, 100, IPC_CREAT | 0666);
 
    while (1)
    {
        sem_wait(&sem2);
        printf("I am p1!\n");
        sem_post(&sem2);
        // Sleep for 1 second
        // sleep(1);
        // Or wait for key-press
        getchar();
    }

    sem_destroy(&sem2);

    return 0;
}
