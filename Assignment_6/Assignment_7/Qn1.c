#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// Define the student structure
struct student
{
    int roll;
    char name[100];
};
// Compare function for qsort to sort the students by roll number
int cmp_roll(const struct student *s1, const struct student *s2)
{
    return s1->roll - s2->roll;
}

int main()
{
    // Create the pipes for communication between P1 and P2
    int fd1[2], fd2[2];
    if (pipe(fd1) < 0 || pipe(fd2) < 0)
    {
        perror("pipe");
        exit(1);
    }

    // Fork to create the child process P2
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    { // P2 code
        // Close the unused ends of the pipes
        close(fd1[1]);
        close(fd2[0]);

        // Read the number of students to expect from P1
        int n;
        if (read(fd1[0], &n, sizeof(int)) < 0)
        {
            perror("read");
            exit(1);
        }

        // Allocate memory for the student array
        struct student *students = malloc(n * sizeof(struct student));
        if (students == NULL)
        {
            perror("malloc");
            exit(1);
        }

        // Read the student structures from P1
        for (int i = 0; i < n; i++)
        {
            if (read(fd1[0], &students[i], sizeof(struct student)) < 0)
            {
                perror("read");
                exit(1);
            }
        }

        // Sort the students by roll number
        qsort(students, n, sizeof(struct student),
              (int (*)(const void *, const void *))cmp_roll);

        // Send the sorted student structures back to P1
        for (int i = 0; i < n; i++)
        {
            if (write(fd2[1], &students[i], sizeof(struct student)) < 0)
            {
                perror("write");
                exit(1);
            }
        }

        // Clean up
        free(students);
        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    }
    else
    { // P1 code
        // Close the unused ends of the pipes
        close(fd1[0]);
        close(fd2[1]);

        // Read the number of students from the user
        int n;
        printf("Enter the number of students: ");
        scanf("%d", &n);

        // Send the number of students to P2
        if (write(fd1[1], &n, sizeof(int)) < 0)
        {
            perror("write");
            exit(1);
        }

        // Fill up the student array with data from the user
        struct student ss[100];
        for (int i = 0; i < n; i++)
        {
            printf("Enter roll number and name of student %d: ", i + 1);
            scanf("%d %s", &ss[i].roll, ss[i].name);
            if (write(fd1[1], &ss[i], sizeof(struct student)) < 0)
            {
                perror("write");
                exit(1);
            }
        }

        // Wait for P2 to finish sorting the students
        wait(NULL);
        // Read the sorted student structures from P2
        struct student sorted_students[100];
        for (int i = 0; i < n; i++)
        {
            if (read(fd2[0], &sorted_students[i], sizeof(struct student)) < 0)
            {
                perror("read");
                exit(1);
            }
        }

        // Print the sorted student data in user-readable format
        printf("Sorted student data:\n");
        printf("Roll Number\tName\n");
        for (int i = 0; i < n; i++)
        {
            printf("%d\t\t%s\n", sorted_students[i].roll, sorted_students[i].name);
        }

        // Clean up
        close(fd1[1]);
        close(fd2[0]);
        exit(0);
    }

    return 0;
}

