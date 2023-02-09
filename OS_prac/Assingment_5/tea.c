#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <time.h>
#include <sys/shm.h>
#include <signal.h>

typedef struct att
{
    int roll;
    time_t tm;

} att;
int shmid;

