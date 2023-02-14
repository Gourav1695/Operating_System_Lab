#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sem.h>

#define NUM_CHAIRS 3

int sem_id; // semaphore id

// function to handle semaphore operations
void sem_op(int sem_num, int op) {
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = op;
    sem_buf.sem_flg = 0;
    semop(sem_id, &sem_buf, 1);
}

int main() {
    // get semaphore set
    sem_id = semget(IPC_PRIVATE, NUM_CHAIRS, 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(1);
    }

    // initialize semaphore values
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    arg.val = NUM_CHAIRS; // all chairs available
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("semctl");
        exit(1);
    }

    // attempt to enter classroom
    printf("Attempting to enter classroom...\n");
    sem_op(0, -1); // decrement chair count semaphore
    if (semctl(sem_id, 0, GETVAL, arg) >= 0) {
        int chairs_available = arg.val;
        if (chairs_available >= 0) {
            printf("%d chairs are available!\n", chairs_available);
            sem_op(0, 1); // increment chair count semaphore
            sem_op(1, -1); // decrement chair occupied semaphore
            printf("Occupying chair...\n");
            // simulate time in classroom
            printf("Entering classroom...\n");
            sleep(5); // assume 5 seconds in classroom
            // free up chair and exit
            printf("Leaving classroom...\n");
            sem_op(1, 1); // increment chair occupied semaphore
        } else {
            printf("No chairs are available.\n");
            sem_op(0, 1); // increment chair count semaphore
        }
    }

    return 0;
}
