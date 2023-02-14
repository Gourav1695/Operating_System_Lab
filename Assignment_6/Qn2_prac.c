#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

#define SEM_NAME "/classroom_sem"

int chairs = 3; // Number of chairs in the classroom
sem_t* sem; // Semaphore for synchronizing access to the chairs

void handler(int signum) {
    // Handle interrupt signal (^C)
    printf("Student has left the classroom.\n");
    if (chairs < 3) {
        chairs++; // Free up a chair
    }
}

int main() {
    signal(SIGINT, handler); // Register the interrupt handler

    // Initialize the semaphore
    sem = sem_open(SEM_NAME, O_CREAT, 0644, chairs);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    while (1) {
        // Wait for a chair to become available
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            exit(1);
        }

        printf("Please enter the classroom!\n");
        chairs--;
        sleep(10); // Simulate occupying the chair

        // Release the chair
        if (sem_post(sem) == -1) {
            perror("sem_post");
            exit(1);
        }
    }

    // Close and unlink the semaphore
    sem_close(sem);
    sem_unlink(SEM_NAME);

    return 0;
}
