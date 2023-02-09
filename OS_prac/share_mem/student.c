#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define KEY 1234

struct att
{
    int roll;
    time_t tm;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./student.out <roll number>\n");
        return 1;
    }
    int roll = atoi(argv[1]);
    int shmid = shmget(KEY, 0, 0);
    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    struct att *rolls = shmat(shmid, 0, 0);
    if (rolls == (void *)-1)
    {
        perror("shmat");
        return 1;
    }

    int i;
    for (i = 0; i < 107; i++)
    {
        if (rolls[i].roll == -1)
        {
            rolls[i].roll = roll;
            rolls[i].tm = time(NULL);
            break;
        }
    }

    shmdt(rolls);
    return 0;
}