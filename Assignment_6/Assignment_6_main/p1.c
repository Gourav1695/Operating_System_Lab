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
    /*
    The macro takes two arguments: semid, which is the semaphore set identifier returned by semget(), and i, which is the index of the semaphore within the set to operate on.

    The semop() system call is used with the semaphore operation structure Pop[i], which represents the semaphore operation to be performed on the semaphore at index i in the semaphore set.
    */
    // int semop(int semid, struct sembuf *sops, size_t nsops);

#define V(semid, i) semop(semid, &Vop[i], 1)

struct sembuf Pop[NO_SEM];
/*
This is a declaration of an array named "Pop" of "struct sembuf" elements. The size of the array is "NO_SEM", which is a preprocessor constant defined elsewhere in the code. Each element in the "struct sembuf" array represents a semaphore operation, with fields such as "sem_num" (the semaphore number), "sem_op" (the operation to be performed on the semaphore), and "sem_flg" (flags that control the behavior of the semaphore operation).
*/
struct sembuf Vop[NO_SEM];

int main()
{
    key_t key;
    pid_t pid;
    int semid;
    int status;

    union semun
    {
        int val;               /* Value for SETVAL */
        struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
        unsigned short *array; /* Array for GETALL, SETALL */
        struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
    } setvalArg;

    setvalArg.val = 1;

    /*

    Here we are setting parameters of sembuf structure for Pop and Vop for sem0

    */
    Pop[0].sem_num = 0;
    Pop[0].sem_op = -1;
    Pop[0].sem_flg = SEM_UNDO;

    Vop[0].sem_num = 0;
    Vop[0].sem_op = 1;
    Vop[0].sem_flg = SEM_UNDO;

    // It is for sem1
    Pop[1].sem_num = 1;
    Pop[1].sem_op = -1;
    Pop[1].sem_flg = SEM_UNDO;

    Vop[1].sem_num = 1;
    Vop[1].sem_op = 1;
    Vop[1].sem_flg = SEM_UNDO;

    // It is for sem2
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

    status = semctl(semid, 0, SETVAL, setvalArg); // int semctl(int semid, int semnum, int cmd, ...);

    if (status == -1)
    {
        perror("semctl() failed");
        exit(1);
    }

    setvalArg.val = 0;
    /*
    In the code snippet, four semaphores are used. The first semaphore with index 0 is initialized to 1, and the other three semaphores with indices 1, 2, and 3 are initialized to 0 using the setval.

    */

    status = semctl(semid, 1, SETVAL, setvalArg);

    if (status == -1)
    {
        perror("semctl() failed");
        exit(1);
    }
    status = semctl(semid, 2, SETVAL, setvalArg);
    if (status == -1)
    {
        perror("semctl() failed");
        exit(1);
    }

    status = semctl(semid, 3, SETVAL, setvalArg);
    if (status == -1)
    {
        perror("semctl() failed");
        exit(1);
    }

    while (1)
    {
        P(semid, 0);
        printf("I am p1!\n");
        sleep(5);
        V(semid, 1);
    }
}