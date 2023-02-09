// 2020CSB010 GOURAV KUMAR SHAW

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX_ARGS = 10;
const int MAX_PROCESSES = 10;

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
    pid_t childPIDs[MAX_PROCESSES]; // array of child PIDs
    int childPIDIndex = 0;

    int argIndex = 1;

    while (argIndex < argc)
    {
        char *execArgs[MAX_ARGS];
        execArgs[0] = argv[argIndex]; // executable name
        int execArgIndex = 1;
        while (execArgIndex < MAX_ARGS && argIndex + 1 < argc &&
               argv[argIndex + 1][0] == '-')
        {
            
            execArgs[execArgIndex++] = argv[++argIndex];
        }
        execArgs[execArgIndex] = NULL;

        pid_t pid = Fork();
        childPIDs[childPIDIndex++] = pid; 

        if (pid == 0)
        {
            // child process
            printf("executing =>");
            for (int i = 0; i < execArgIndex; i++)
            {
                printf(" %s", execArgs[i]);
            }
            printf("\n");

            if (execve(execArgs[0], execArgs, NULL) == -1)
            {
                perror("execve failed");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }

        argIndex++;
    }

    // checking for the status of the child processes
    for (int i = 0; i < childPIDIndex; i++)
    {
        int wstatus;
        pid_t cpid = childPIDs[i], w;
        do
        {
            w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
            if (w == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            printf("[%d] ", cpid);
            if (WIFEXITED(wstatus))
            {
                printf("exited, status=%d\n", WEXITSTATUS(wstatus));
            }
            else if (WIFSIGNALED(wstatus))
            {
                printf("killed by signal %d\n", WTERMSIG(wstatus));
            }
            else if (WIFSTOPPED(wstatus))
            {
                printf("stopped by signal %d\n", WSTOPSIG(wstatus));
            }
            else if (WIFCONTINUED(wstatus))
            {
                printf("continued\n");
            }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    }

    exit(EXIT_SUCCESS);
}
