// 2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("textFile.txt", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // child process 
        char c;
        printf("Child process writing file:\n");
        while ((c = fgetc(fp)) != EOF) {
            putchar(c);
        }
        printf("\n");
    } else {
        // parent process
        char c;
        printf("Parent process writing file:\n");
        while ((c = fgetc(fp)) != EOF) {
            putchar(c);
        }
        printf("\n");
    }
    fclose(fp);
    return 0;
}