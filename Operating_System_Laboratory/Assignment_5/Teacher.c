//2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define MAX_STUDENTS 107
#define KEY_NUM 1234
#define ERROR -1

struct att {
    int roll;
    time_t tm;
};

// shared memory segment for rolls array
struct att *rolls;
int shmid;

void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        printf("\nTerminating Teacher process...\n");
        printf("Attendance list:\n");
        for (int i = 0; i < MAX_STUDENTS; i++)
        {
            if (rolls[i].roll != -1)
            {
                char *ctime1 = ctime(&rolls[i].tm);
                ctime1[strlen(ctime1) - 1] = '\0';
                printf("%d %s\n", rolls[i].roll, ctime1);
            }
        }

        shmdt(rolls);
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    int num_students;
    if (argc != 2)
    {
        printf("Usage: ./teacher.out <n> (n = number of students)\n");
        return ERROR;
    }

    num_students = atoi(argv[1]);
    if (num_students <= 0 || num_students > MAX_STUDENTS)
    {
        printf("Error: Invalid number of students. Must be between 1 and 107.\n");
        return ERROR;
    }

    // create shared memory segment
    shmid = shmget(KEY_NUM, num_students * sizeof(struct att), IPC_CREAT | 0666);
    if (shmid == ERROR)
    {
        printf("Error: Failed to create shared memory segment\n");
        return ERROR;
    }

    // attach rolls array to shared memory
    rolls = shmat(shmid, NULL, 0);
    if (rolls == (void *)ERROR)
    {
        printf("Error: Failed to attach shared memory segment\n");
        return ERROR;
    }

    // initialize rolls array
    for (int i = 0; i < num_students; i++)
    {
        rolls[i].roll = -1;
        rolls[i].tm = (time_t)0;
    }

    // handle ctrl+c signal to terminate process
    signal(SIGINT, signal_handler);

    while (1)
    {
        // busy wait
    }

    return 0;
}
