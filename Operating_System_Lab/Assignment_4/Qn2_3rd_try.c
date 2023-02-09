#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define ROW_A 3
#define COL_A 4
#define ROW_B 4
#define COL_B 5

int main() {
    int a[ROW_A][COL_A], b[ROW_B][COL_B], c[ROW_A][COL_B];
    int i, j, k, sum;

    // Read matrix A from user
    printf("Enter elements of matrix A (%dx%d):\n", ROW_A, COL_A);
    for (i = 0; i < ROW_A; i++) {
        for (j = 0; j < COL_A; j++) {
            scanf("%d", &a[i][j]);
        }
    }

    // Read matrix B from user
    printf("Enter elements of matrix B (%dx%d):\n", ROW_B, COL_B);
    for (i = 0; i < ROW_B; i++) {
        for (j = 0; j < COL_B; j++) {
            scanf("%d", &b[i][j]);
        }
    }

    // Create shared memory for matrix C
    int shmid = shmget(IPC_PRIVATE, sizeof(c), IPC_CREAT | 0666);
    int (*c_shared)[COL_B] = shmat(shmid, NULL, 0);

    // Create child processes
    for (i = 0; i < ROW_A; i++) {
        if (fork() == 0) {
            // Child process computes one row of matrix C
            for (j = 0; j < COL_B; j++) {
                sum = 0;
                for (k = 0; k < COL_A; k++) {
                    sum += a[i][k] * b[k][j];
                }
                c_shared[i][j] = sum;
                exit(0);
            }
        }
    }

    // Wait for all child processes to finish
    while (wait(NULL) > 0);

    // Parent process prints matrix C
    printf("\nMatrix C:\n");
    for (i = 0; i < ROW_A; i++) {
        for (j = 0; j < COL_B; j++) {
            printf("%d ", c_shared[i][j]);
        }
        printf("\n");
    }

    // Detach and remove shared memory
    shmdt(c_shared);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
