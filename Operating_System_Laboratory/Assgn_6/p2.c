
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> /* for shmget() shmctl() */
#include <unistd.h>
#include <sys/shm.h> /* for shmget() shmctl() */
#include <signal.h>  /* for signal(2) kill(2) */
#include <time.h>

sem_t sem1, sem2, sem3;

int main() {
    int projid=1;
     int shmid;
    key_t shmkey=ftok("./file.txt",projid);
    shmid=shmget(shmkey,100, 0666);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);

    while (1) {
        sem_wait(&sem1);
        printf("I am p2 section 1!\n");
        sem_post(&sem2);
        sem_wait(&sem2);
        printf("I am p2 section 2!\n");
        sem_post(&sem3);
        getchar();
    }

    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);

    return 0;
}
