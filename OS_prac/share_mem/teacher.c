#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>

#define SHM_KEY 12345

struct att {
     int roll; /* The roll number of the student giving the attendance */

     time_t tm; /* To keep the time when attendance was given by the student. Read the manual for time(), that is, "man 2 time" Also see the manual entries mentioned in the "see also" section of this manual page! */
};

// void cleanup(int sig) {
//     int shmid = shmget(SHM_KEY, sizeof(struct att), 0);
//     struct att *rolls = shmat(shmid, NULL, 0);
//     int n = atoi(argv[1]);

//     printf("Attendance Report:\n");
//     for (int i = 0; i < n; i++) {
//         if (rolls[i].roll != -1) {
//             char buf[100];
//             strftime(buf, 100, "%B %d, %Y, %T", localtime(&rolls[i].tm));
//             printf("Roll number %d gave attendance at %s\n", rolls[i].roll, buf);
//         }
//     }

//     shmdt(rolls);
//     shmctl(shmid, IPC_RMID, NULL);
//     exit(0);
// }

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of students>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    int shmid = shmget(SHM_KEY, sizeof(struct att) * n, IPC_CREAT | 0666);
    struct att *rolls = shmat(shmid, NULL, 0);

    memset(rolls, -1, sizeof(struct att) * n);
    
void cleanup(int sig) {
    int shmid = shmget(SHM_KEY, sizeof(struct att), 0);
    struct att *rolls = shmat(shmid, NULL, 0);
    int n = atoi(argv[1]);

    printf("Attendance Report:\n");
    for (int i = 0; i < n; i++) {
        if (rolls[i].roll != -1) {
            char buf[100];
            strftime(buf, 100, "%B %d, %Y, %T", localtime(&rolls[i].tm));
            printf("Roll number %d gave attendance at %s\n", rolls[i].roll, buf);
        }
    }

    shmdt(rolls);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

    signal(SIGINT, cleanup);

    while (1) {
        sleep(1);
    }

    return 0;
}
