//2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define N 4
#define M 5
#define P 6

int a[N][M], b[M][P], c[N][P];

int main() {
    int i, j, k;

    //Read matrix A from the user
    printf("Enter elements of matrix A (size: %dx%d):\n", N, M);
    for(i = 0; i < N; i++) {
        for(j = 0; j < M; j++) {
            scanf("%d", &a[i][j]);
        }
    }

    //Read matrix B from the user
    printf("Enter elements of matrix B (size: %dx%d):\n", M, P);
    for(i = 0; i < M; i++) {
        for(j = 0; j < P; j++) {
            scanf("%d", &b[i][j]);
        }
    }

    //Create shared memory for array c
    int shm_id = shmget(IPC_PRIVATE, sizeof(c), IPC_CREAT | 0666);
    // c = (int (*)[P]) shmat(shm_id, NULL, 0);
    int (*c)[P] =shmat(shm_id, NULL, 0);


    //Create child processes
    for(i = 0; i < N; i++) {
        int pid = fork();
        if(pid == 0) {
            for(k = 0; k < P; k++) {
                c[i][k] = 0;
                for(j = 0; j < M; j++) {
                    c[i][k] += a[i][j] * b[j][k];
                }
            }
            shmdt(c);
            exit(0);
        }
    }

    //Wait for child processes to finish
    for(i = 0; i < N; i++) {
        wait(NULL);
    }

    //Print matrix C
    printf("Matrix C (size: %dx%d):\n", N, P);
    for(i = 0; i < N; i++) {
        for(j = 0; j < P; j++) {
            printf("%d ", c[i][j]);
        }
        printf("\n");
    }

    //Detach and remove shared memory
    shmdt(c);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}

