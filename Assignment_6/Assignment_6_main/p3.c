// 2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <sys/types.h>/* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h>  /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h>  /* for semget(2) semop(2) semctl(2) */
#include <unistd.h>   /* for fork(2) */
#include <time.h>
#include <stdlib.h>/* for exit(3) */

#define NO_SEM 4

#define P(semid, i) semop(semid, &Pop[i], 1)
#define V(semid, i) semop(semid, &Vop[i], 1)

struct sembuf Pop[NO_SEM];
struct sembuf Vop[NO_SEM];

int main()
{
    key_t key;
    pid_t pid;
    int semid;
    int status;
    //  Here we are setting parameters of sembuf structure for Pop  and Vop for sem0
    Pop[0].sem_num = 0;
    Pop[0].sem_op = -1;
    Pop[0].sem_flg = SEM_UNDO;

    Vop[0].sem_num = 0;
    Vop[0].sem_op = 1;
    Vop[0].sem_flg = SEM_UNDO;

    // for sem1
    Pop[1].sem_num = 1;
    Pop[1].sem_op = -1;
    Pop[1].sem_flg = SEM_UNDO;

    Vop[1].sem_num = 1;
    Vop[1].sem_op = 1;
    Vop[1].sem_flg = SEM_UNDO;

    // for sem2
    Pop[2].sem_num = 2;
    Pop[2].sem_op = -1;
    Pop[2].sem_flg = SEM_UNDO;

    Vop[2].sem_num = 2;
    Vop[2].sem_op = 1;
    Vop[2].sem_flg = SEM_UNDO;

    Pop[3].sem_num = 3;
    Pop[3].sem_op = -1;
    Pop[3].sem_flg = SEM_UNDO;

    Vop[3].sem_num = 3;
    Vop[3].sem_op = 1;
    Vop[3].sem_flg = SEM_UNDO;
    key = ftok("file.txt", 1);
    if (key == -1)
    {
        perror("ftok() failed");
        exit(1);
    }

    semid = semget(key, NO_SEM, IPC_CREAT | 0777);
    if (semid == -1)
    {
        perror("semget() failed");
        exit(1);
    }

    while (1)
    {
        P(semid, 2);
        printf("I am p3 !\n");
        sleep(5);
        V(semid, 3);
    }
}