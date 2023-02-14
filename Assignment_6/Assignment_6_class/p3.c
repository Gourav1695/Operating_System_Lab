// 2020CSB010 GOURAV KUMAR SHAW

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <unistd.h>
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h>  /* for signal(2) kill(2) */
#include <time.h>
 
sem_t sem3;

int main()
{
    sem_init(&sem3, 0, 1);
    int projid = 1;
    int shmid;
    key_t shmkey = ftok("./file.txt", projid);
    shmid = shmget(shmkey, 100, 0666);
    while (1)
    {
        sem_wait(&sem3);
        printf("I am p3!\n");
        sem_post(&sem3);
        // Sleep for 1 second
        // sleep(1);
        // Or wait for key-press
        getchar();
    }

    sem_destroy(&sem3);

    return 0;
}