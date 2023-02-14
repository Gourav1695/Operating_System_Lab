#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define SEM_KEY 1234

int chairs = 3; // Number of chairs in the classroom
int semid; // Semaphore ID for the set of semaphores

void handler(int signum) {
    // Handle interrupt signal (^C)
    printf("Student has left the classroom.\n");
    if (chairs < 3) {
        chairs++; // Free up a chair
    }
}

union semun {
    int val;               // Value for SETVAL
    struct semid_ds *buf;  // Buffer for IPC_STAT, IPC_SET
    unsigned short *array; // Array for GETALL, SETALL
    struct seminfo *__buf; // Buffer for IPC_INFO (Linux-specific)
};

int init_sem(int semid, int value) {
    union semun arg;
    arg.val = value;
    return semctl(semid, 0, SETVAL, arg);
}

void wait_sem(int semid) {
    struct sembuf sb = { 0, -1, SEM_UNDO };
    semop(semid, &sb, 1);
}

void signal_sem(int semid) {
    struct sembuf sb = { 0, 1, SEM_UNDO };
    semop(semid, &sb, 1);
}

int main() {
    signal(SIGINT, handler); // Register the interrupt handler

    // Create a semaphore set with two semaphores: one for mutual exclusion
    // and one for the number of chairs available in the classroom
    semid = semget(SEM_KEY, 0, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    // Initialize the semaphores
    if (init_sem(semid, 1) == -1 || init_sem(semid + 1, chairs) == -1) {
        perror("semctl");
        exit(1);
    }

    while (1) {
        // Wait for a chair to become available
        wait_sem(semid + 1);

        // Acquire the lock
        wait_sem(semid);

        printf("Please enter the classroom!\n");
        chairs--;
        sleep(10); // Simulate occupying the chair

        // Release the chair
        chairs++;
        signal_sem(semid + 1);

        // Release the lock
        signal_sem(semid);
    }

    // Remove the semaphore set
    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(1);
    }

    return 0;
}
