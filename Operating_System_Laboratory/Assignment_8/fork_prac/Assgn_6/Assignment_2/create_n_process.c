// 2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for fork
#include <sys/wait.h>

int factorial(int n) {
    int res = 1;
    for (int i = 2; i <= n; i++) {
        res *= i;
    }
    return res;
}

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        int d = atoi(argv[i]);// changing to integer
        if (d < 0) {
            printf("Error: Factorial of a negative number is not defined.\n");
            return 1;
        }

        pid_t pid = fork();
        if (pid == 0) {
            // child process created here
            int f = factorial(d);
            printf("The factorial of %d is %d\n", d, f);
            return 0;
        } else if (pid > 0) {
            // parent process created here
            wait(NULL);
        } else {
            // fork failed
            printf("Error: fork failed.\n");
            return 1;
        }
    }

    return 0;
}