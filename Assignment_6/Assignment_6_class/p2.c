// 2020CSB010 GOURAV KUMAR SHAW

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <unistd.h>
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h>  /* for signal(2) kill(2) */
#include <time.h>

sem_t sem1;
sem_t sem2;

int main()
{ 
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);
     int projid=1;
     int shmid;
    key_t shmkey=ftok("./file.txt",projid);
    shmid=shmget(shmkey,100, 0666);
    /*
    The line sem_init(&sem2, 0, 1); initializes a semaphore sem2 with the following parameters:

    &sem2: A pointer to the semaphore that is being initialized.

    0: A value of 0 indicates that the semaphore is shared between threads of the same process.

    1: The third argument, 1, is the initial value of the semaphore. In this case, the semaphore is initialized with a value of 1, which means that it is available for use by the first process that acquires it.
    */

    while (1)
    {
        sem_wait(&sem1);//  decrement  1 ->0
        printf("I am p2 section 1!\n");
        sem_post(&sem2);// increment 0->1
        sem_wait(&sem2);// decrement 1->0
        printf("I am p2 section 2!\n");
        sem_post(&sem1);// increment 0->1
        // sleep(1);
         getchar();
    }
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}


