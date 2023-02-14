// 2020CSB010 GOURAV KUMAR SHAW

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

struct student {
    int roll;
    char name[100];
};

void fill_student_info(struct student *s, int n) {
    for (int i = 0; i < n; i++) {
        printf("Enter roll and name for student %d:\n", i+1);
        scanf("%d %s", &(s[i].roll), s[i].name);
    }
}

void print_student_info(struct student *s, int n) {
    printf("Student information:\n");
    for (int i = 0; i < n; i++) {
        printf("Roll: %d, Name: %s\n", s[i].roll, s[i].name);
    }
}

int cmp_roll(const void *a, const void *b) {
    struct student *s1 = (struct student *) a;
    struct student *s2 = (struct student *) b;
    return s1->roll - s2->roll;
}

int main() {
    int n;
    printf("Enter the number of students: ");
    scanf("%d", &n);

    int fd1[2], fd2[2];
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe error");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork error");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process (P2)
        close(fd1[1]); // close unused write end of first pipe
        close(fd2[0]); // close unused read end of second pipe

        struct student s[n];
        if (read(fd1[0], s, n*sizeof(struct student)) == -1) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }

        qsort(s, n, sizeof(struct student), cmp_roll);

        if (write(fd2[1], s, n*sizeof(struct student)) == -1) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }

        close(fd1[0]);
        close(fd2[1]);
        exit(EXIT_SUCCESS);
    } else { // parent process (P1)
        close(fd1[0]); // close unused read end of first pipe
        close(fd2[1]); // close unused write end of second pipe

        struct student ss[n];
        fill_student_info(ss, n);

        if (write(fd1[1], ss, n*sizeof(struct student)) == -1) {
            perror("Write error");
            exit(EXIT_FAILURE);
        }

        if (waitpid(pid, NULL, 0) == -1) {
            perror("Wait error");
            exit(EXIT_FAILURE);
        }

        struct student sorted_ss[n];
        if (read(fd2[0], sorted_ss, n*sizeof(struct student)) == -1) {
            perror("Read error");
            exit(EXIT_FAILURE);
        }

        print_student_info(sorted_ss, n);

        close(fd1[1]);
        close(fd2[0]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
