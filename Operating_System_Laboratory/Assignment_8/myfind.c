// 2020CSB010 GOURAV KUMAR SHAW
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <sys/stat.h>

#include <dirent.h>

// ssize_t write(int fd, const void *buf, size_t noct);
// int open(const char *path, int flags /* mode_t mod */);
// int creat(const char *path, mode_t mod);
// open(path, O_WRONLY | O_CREAT | O_TRUNC, mod);
// ssize_t read(int fd, void *buf, size_t noct);
// int close(int fd);
// off_t lseek(int fd, off_t offset, int ref);

void myfind(char *dirName)

{

    DIR *dir;

    struct dirent *dirEntry;

    struct stat inode;

    char name[1000];

    dir = opendir(dirName);

    if (dir == 0)
    {

        perror("Error opening file..");

        exit(1);
    }

    while ((dirEntry = readdir(dir)) != 0)
    {

        sprintf(name, "%s/%s", dirName, dirEntry->d_name);

        lstat(name, &inode);

        // test the type of file

        if (S_ISDIR(inode.st_mode))

            printf("dir ");

        else if (S_ISREG(inode.st_mode))

            printf("Files: ");

        else

            if (S_ISLNK(inode.st_mode))

            printf("lnk ");

        else
            ;

        printf(" %s\n", dirEntry->d_name);
    }
}

int main(int argc, char **argv)

{

    if (argc != 2)
    {

        printf("USE: %s nume_dir\n", argv[0]);

        exit(0);
    }

    myfind(argv[1]);
}