// 2020CSB010 GOURAV KUMAR SHAW

#include <unistd.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define INTERVAL 1 // Interval in seconds between updates

int factorial(int n)
{ // function for factorial
    int result = 1;
    for (int i = 1; i <= n; i++)
    {
        result *= i;
    }
    return result;
}

int main()
{
    int shm_id;
    int *shared_num;

    // Create shared memory segment
    shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0)
    {
        perror("Error creating shared memory segment");
        exit(1);
    }

    // Attach shared memory segment to process
    shared_num = (int *)shmat(shm_id, NULL, 0);
    if (shared_num == (int *)-1)
    {
        perror("Error attaching shared memory segment");
        exit(1);
    }
    while (1)
    {
        pid_t pid = fork();
        // infinite loop
        //    pid_t pid = fork();
        if (pid == 0)
        {

            printf("Child: Factorial of %d is %d\n", *shared_num, factorial(*shared_num));
            sleep(INTERVAL);
            exit(1);
        }
        else
        {

            srand(time(NULL));
            *shared_num = rand() % 12 + 1; // generate random number between 1 and 12
            printf("Parent: Generated random number %d\n", *shared_num);
            sleep(INTERVAL);
        }
    }

    shmdt(shared_num);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}