#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 100

int a[MAX_SIZE][MAX_SIZE], b[MAX_SIZE][MAX_SIZE], c[MAX_SIZE][MAX_SIZE];
int n, m, p;

void inputMatrices() {
    printf("Enter dimensions of matrix A (n x m): ");
    scanf("%d %d", &n, &m);
    printf("Enter elements of matrix A:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &a[i][j]);
        }
    }
    printf("Enter dimensions of matrix B (m x p): ");
    scanf("%d %d", &m, &p);
    printf("Enter elements of matrix B:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            scanf("%d", &b[i][j]);
        }
    }
}

void multiplyRow(int row) {
    for (int i = 0; i < p; i++) {
        c[row][i] = 0;
        for (int j = 0; j < m; j++) {
            c[row][i] += a[row][j] * b[j][i];
        }
    }
}

int main() {
    inputMatrices();
    for (int i = 0; i < n; i++) {
        if (fork() == 0) {
            // child process
            multiplyRow(i);
            exit(0);
        }
    }
    // parent process
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
    printf("Resultant matrix C (n x p):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            printf("%d ", c[i][j]);
        }
        printf("\n");
    }
    return 0;
}
