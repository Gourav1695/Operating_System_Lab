#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


pid_t Fork()
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        // pid is less than 0, fork() failed
        perror("Fork error");
        exit(EXIT_FAILURE);
    }
    return pid;
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        // iterating through the arguments which contains the executable file paths
        pid_t pid = Fork();

        if (pid == 0)
        {
            // child process
            printf("executing %d: %s\n", i, argv[i]);
            if (execvp(argv[i], (char *[]){argv[i], NULL}) == -1)
            {
                perror("[ERROR] execve failed");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_SUCCESS);
}
