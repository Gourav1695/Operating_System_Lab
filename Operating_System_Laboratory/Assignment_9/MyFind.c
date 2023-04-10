#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Command should be like this: %s <Directory Name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    DIR *dp = opendir(argv[1]);
    struct dirent *dirEntry;
    struct stat inode;

    if (dp == NULL)
    {
        perror("Error occured in opendir()\n");
        exit(EXIT_FAILURE);
    }

    int operation;
    printf("Enter 1 for searching based on directory or file Name\n");
    printf("Enter 2 for searching based on directory or file Modification Time\n");
    printf("Enter 3 for searching based on directory or file Permissions\n");
    scanf("%d", &operation);

    switch (operation)
    {
    case 1:
        /* code */
        char searchName[100];
        char name[1000];
        printf("Enter the file name to be searched\n");
        scanf("%s", searchName);
        while ((dirEntry = readdir(dp)) != NULL)
        {
            memset(name, 0, sizeof(name));
            strcat(name, argv[1]);
            strcat(name, "/");
            strcat(name, dirEntry->d_name);
            lstat(name, &inode);

            if (strstr(dirEntry->d_name, searchName) != NULL)
            {
                printf(" %s\n", dirEntry->d_name);
                
            }
        }
        break;

    case 2:
        int searchTime;
        printf("Enter the time to be comapred with\n");
        printf("%ld\n", time(NULL));
        while ((dirEntry = readdir(dp)) != NULL)
        {
            memset(name, 0, sizeof(name));
            strcat(name, argv[1]);
            strcat(name, "/");
            strcat(name, dirEntry->d_name);
            lstat(name, &inode);
            printf("%d\n", (int)inode.st_mtime);
        }
        break;
    case 3:
        mode_t permission;
        printf("Enter the permission in octal form\n");
        scanf("%o", &permission);
        while ((dirEntry = readdir(dp)) != NULL)
        {
            memset(name, 0, sizeof(name));
            strcat(name, argv[1]);
            strcat(name, "/");
            strcat(name, dirEntry->d_name);
            lstat(name, &inode);

            if ((inode.st_mode & 0777) == permission)
            {
                printf(" %s\n", dirEntry->d_name);
            }
        }
        break;
    default:
        printf("Please choose a correct option!!\n");
        break;
    }

    return 0;
}