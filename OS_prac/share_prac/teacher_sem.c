#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>

#define SHM_KEY 0x1234 // Key for shared memory
#define SEM_KEY 0x5678 // Key for semaphore

// Struct for student attendance
struct att
{
    int roll;
    time_t tm;
};

int shmid;         // Shared memory ID
struct att *rolls; // Pointer to shared memory
sem_t *sem;        // Pointer to semaphore
int n;             // Number of students

// Signal handler for Ctrl+C
void sigint_handler(int signo)
{
    int i;
    char time_str[26];

    // Print roll numbers and attendance times of all students
    printf("\nAttendance:\n");
    for (i = 0; i < n; i++)
    {
        if (rolls[i].roll != -1)
        {
            strftime(time_str, 26, "%B %d, %Y, %H:%M", localtime(&rolls[i].tm));
            printf("Roll %d: %s\n", rolls[i].roll, time_str);
        }
    }

    // Detach shared memory
    shmdt(rolls);

    // Destroy shared memory
    shmctl(shmid, IPC_RMID, NULL);

    // Close semaphore
    sem_close(sem);

    // Exit program
    exit(0);
}

int main(int argc, char *argv[])
{
    int i;

    // Check command-line arguments
    if (argc != 2)
    {
        printf("Usage: %s <number of students>\n", argv[0]);
        return 1;
    }

    // Get number of students
    n = atoi(argv[1]);

    // Create shared memory
    shmid = shmget(SHM_KEY, n * sizeof(struct att), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        return 1;
    }

    // Attach shared memory
    rolls = shmat(shmid, NULL, 0);
    if (rolls == (struct att *)-1)
    {
        perror("shmat");
        return 1;
    }

    // Initialize shared memory
    memset(rolls, -1, n * sizeof(struct att));

    // Open semaphore
    sem = sem_open("/sem", IPC_CREAT, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("sem_open");
        return 1;
    }

    // Register signal handler for Ctrl+C
    signal(SIGINT, sigint_handler);

    // Wait for student processes
    while (1)
    {
        sleep(1);
    }
}