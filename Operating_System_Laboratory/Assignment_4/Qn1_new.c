// 2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    int shmid, *shm;
    pid_t pid;

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("Error creating shared memory");
        exit(1);
    }

    // Attach shared memory to the parent process
    shm = (int *)shmat(shmid, NULL, 0);
    if (shm == (int *)-1)
    {
        perror("Error attaching shared memory to parent");
        exit(1);
    }

    // Fork a child process
    pid = fork();
    if (pid < 0)
    {
        perror("Error forking child process");
        exit(1);
    }
    else if (pid == 0)
    {
        // Child process
        while (1)
        {
            // Print factorial of number in shared memory
            int n = *shm;
            int factorial = 1;
            for (int i = 1; i <= n; i++)
                factorial *= i;
            printf("Child: Factorial of %d is %d\n", n, factorial);
            sleep(1);
        }
    }
    else
    {
        // Parent process
        while (1)
        {
            // Put random number in shared memory
            *shm = rand() % 10; // Generates a random number between 0 and 9
            printf("Parent: Putting %d in shared memory\n", *shm);
            sleep(1);
        }
    }

    // Detach and release shared memory
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
