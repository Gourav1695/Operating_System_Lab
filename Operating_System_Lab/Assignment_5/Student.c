#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

struct att {
     int roll;
     time_t tm;
};

int main(int argc, char *argv[]) {
    int n, i;
    key_t key;
    struct att *rolls;
    int shmid;

    if (argc != 2) {
        printf("Usage: %s <roll number of student>\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    key = ftok("Teacher.c", 'R');
    shmid = shmget(key, 0, 0);
    rolls = (struct att *)shmat(shmid, NULL, 0);

    for (i = 0; i < 107; i++) {
        if (rolls[i].roll == -1) {
            rolls[i].roll = n;
            rolls[i].tm = time(NULL);
            break;
        }
    }

    shmdt(rolls);
    return 0;
}
