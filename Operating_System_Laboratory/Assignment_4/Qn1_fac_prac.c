#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024

void factorial(int n) {
    int i, result = 1;
    for (i = 1; i <= n; i++) {
        result *= i;
    }
    printf("Factorial of %d is %d\n", n, result);
}

int main() {
  int status;
  pid_t p1 = 0;

  sighandler_t shandler;

  shandler = signal(SIGINT, releaseSHM);

  getSHM(&shmid_c);
  getSHM(&shmid_n);
  printf("Parent process: shmid_c = %d, shmid_n = %d.\n", shmid_c, shmid_n);

  p1 = fork();
  if (p1 == 0) { /* child process */
    char *c = getPtrSHM(shmid_c, false);
    int *n = getPtrSHM(shmid_n, false);

    while (1) {
      if ((*c) == 'n') {
        printf("Child process calculating factorial of %d is %d\n", *n,
               factorial(*n));
        *c = 'y';
        sleep(1);
      }
    }

    exit(0);

  } else { /* parent process */
    int i;
    char *c = getPtrSHM(shmid_c, true);
    int *n = getPtrSHM(shmid_n, true);
    *c = 'y';

    while (1) {
      if ((*c) == 'y') {
        *c = 'n';
        *n = rand() % MAX_FACTORIAL;
        printf("Parent updated SHM to %d\n", *n);
      }
    }
    exit(0);
  }
}
