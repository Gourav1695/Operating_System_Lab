// 2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <sys/types.h>/* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h>  /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h>  /* for semget(2) semop(2) semctl(2) */
#include <unistd.h>   /* for fork(2) */
#include <time.h>
#include <stdlib.h>/* for exit(3) */
#include <errno.h>
#include <signal.h>

int NUM_CHAIRS = 3;

int sem_id; // semaphore id
void handler(int signum)
{
    // Handle interrupt signal (^C)
    printf("Student has left the classroom.\n");
    if (NUM_CHAIRS < 3)
    {
        NUM_CHAIRS++; // Free up a chair
    }
}

// function to handle semaphore operations
void sem_op(int sem_num, int op)
{
    struct sembuf sem_buf;
    sem_buf.sem_num = sem_num;
    sem_buf.sem_op = op;
    sem_buf.sem_flg = 0;
    semop(sem_id, &sem_buf, 1);
}
/*
The "sem_num" argument is used to specify the semaphore number, and the "op" argument is used to specify the operation to be performed on the semaphore.
"semop" system call is used to perform the operation on the semaphore. The "sem_id" argument is the identifier of the semaphore set to which the semaphore belongs. The "&sem_buf" argument is a pointer to the struct "sem_buf", and "1" is the number of semaphores in the array that is being operated on
*/
int main()
{
    // get semaphore set
    signal(SIGINT, handler); // Register the interrupt handler
    sem_id = semget(IPC_PRIVATE, NUM_CHAIRS, 0666);
    if (sem_id == -1)
    {
        perror("semget");
        exit(1);
    }

    // initialize semaphore values
    union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
    } arg;
    arg.val = 1; // chairs available
    for (int i = 0; i < NUM_CHAIRS; i++)
    {
        if (semctl(sem_id, i, SETVAL, arg) == -1)
        {
            perror("semctl");
            exit(1);
        }
    }

    // attempt to enter classroom
    printf("Attempting to enter classroom...\n");
    int entered = 0;
    while (!entered)
    {
        // wait for available chair
        sem_op(0, -1); // first argument 0 is for sem_num and the 2nd argument -1 is the operation to be performed
        printf("A chair is available!\n");
        // try to occupy chair
        sem_op(1, -1);
        if (semctl(sem_id, 1, GETVAL, arg) == 0)
        {
            //  Return semval (i.e., the semaphore value) for the semnum-th semaphore of the set.  The calling  process
            //  must have read permission on the semaphore set.
            printf("Occupying chair...\n");
            entered = 1;
        }
        else
        {
            printf("Another student is occupying the chair.\n");
            sem_op(1, 1);
        }
        sem_op(0, 1); // free up the chair semaphore
    }

    // simulate time in classroom
    printf("Entering classroom...\n");
    sleep(50); // assume 50 seconds in classroom

    // free up chair and exit
    printf("Leaving classroom...\n");
    sem_op(1, 1);

    return 0;
}
