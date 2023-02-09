// 2020CSB010 GOURAV KUMAR SHAW

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // for fork
#include <sys/wait.h>

int main(int argc, char *argv[])
{

    if (argc < 1)
    {
        printf("Please give atleast one file\n");
        return 1;
    }
    for (int i = 1; i < argc; i+=2)
    {
        pid_t pid = fork();
        if (pid > 0)
        {
            wait(NULL);
        }
        if (pid == 0)
        {
            // child process
            printf("\n executing %d: %s\n", i/2+1, argv[i]);
            if (execv(argv[i], (char *[]){argv[i], argv[i+1]}) == -1)
            {
                printf("Error in Executable file\n");
            }
            return 0;
        }
    }
}