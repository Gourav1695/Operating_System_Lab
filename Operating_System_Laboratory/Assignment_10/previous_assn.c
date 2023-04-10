// #include <stdio.h>
// #include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
// int init_File_dd(const char *fname , int bsize , int bno)
// {
//     int fd, n, s, ubn, fbn; 
//     char temp;
//     char ub = (char)malloc( sizeof(char)*(n/8+1) );
//         n = bno ;
//     memset(ub , 0 , (n/8+1)*sizeof(char) );
//     fd  = open(fname , O_CREAT|O_RDWR, 0666);
//     if(fd == -1)
//     {
//         perror("open fail");
//         return -1;
//     }
//     // n = bno ;
//     s = bsize;
//     ubn = 0;
//     fbn = bno;
//     lseek(fd , 0 , SEEK_SET);
//     write(fd, &n , sizeof(n));
//     write(fd, &s , sizeof(s));
//     write(fd, &ubn , sizeof(ubn));
//     write(fd, &fbn , sizeof(fbn));
//     write(fd, ub , n/8+1);
    
//     lseek(fd, 1023 , SEEK_SET);
//     temp = '#';
//     write(fd, &temp , sizeof(temp));
//     lseek(fd ,1024 + n*s , SEEK_SET);
//     temp = '\0';
//     write(fd, &temp , sizeof(temp));
//     // printf("pointer at %ld\n",lseek(fd, 0 , SEEK_CUR) );
//     close(fd);
//     free(ub);
//     return 0;
// }
int init_File_dd(const char *fname, int bsize, int bno) {
    int fd = open(fname, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("Failed to create file");
        return -1;
    }

    // Calculate the size of the file system
    int fs_size = 1024 + (bsize * bno);

    // Write metadata to the first 1024 bytes of the file
    int n = bno;
    int s = bsize;
    int ubn = 0;
    int fbn = bno;
    uint8_t *ub = calloc(bno, sizeof(uint8_t));

    memcpy(ub, &ubn, sizeof(int)); // copy ubn to ub array
    memcpy(ub + sizeof(int), &fbn, sizeof(int)); // copy fbn to ub array

    // Write metadata to the file
    if (write(fd, &n, sizeof(int)) < 0 ||
        write(fd, &s, sizeof(int)) < 0 ||
        write(fd, &ubn, sizeof(int)) < 0 ||
        write(fd, &fbn, sizeof(int)) < 0 ||
        write(fd, ub, bno) < 0) {
        perror("Failed to write metadata to file");
        close(fd);
        free(ub);
        return -1;
    }

    // Allocate blocks for metadata and set them as used
    for (int i = 0; i < 1024 / BLOCK_SIZE; i++) {
        if (write(fd, "", BLOCK_SIZE) < 0) {
            perror("Failed to allocate block for metadata");
            close(fd);
            free(ub);
            return -1;
        }
        ub[i / 8] |= 1 << (i % 8);
        ubn++;
        fbn--;
    }
}
int get_freeblock(const char *fname)
{
    int fd;
    int n, s, ubn ,fbn;
    char *ub;
    char temp ;

    fd  = open(fname , O_CREAT|O_RDWR, 0666);
    if(fd == -1)
    {
        perror("open fail");
        return -1;
    }
    lseek(fd , 0 ,SEEK_SET);
    read(fd ,&n , sizeof(n));
    read(fd ,&s , sizeof(s));
    read(fd ,&ubn , sizeof(ubn));
    read(fd ,&fbn , sizeof(fbn));
    ub = (char*)malloc((n/8 + 1)*sizeof(char));
    read(fd ,ub , (n/8+1)*sizeof(char));

    // finding first free block
    int freebit = -1 , i ;
    for(i = 0 ;i< n/8+1 ; i++){
        // printf(" %x " , ub[i]);
        for(int j = 0 ;j < 8 ;j++)
            if((ub[i]&(1<<j)) == 0){
                freebit = j;
                break;
            }
        if(freebit!= -1)
            break;
    }
    if(freebit == -1)
        return freebit;

    ub[i] = (ub[i]|(1<<freebit));

    ubn = ubn+1;
    fbn = fbn-1;
    int foundfreeblockno = i*8 + freebit;
    char towrite = (char)malloc(sizeof(char)*s);
    memset(towrite , INT_MAX, sizeof(char)*s);

    lseek(fd , 0 ,SEEK_SET);

    write(fd , &n , sizeof(n));
    write(fd , &s , sizeof(s));
    write(fd , &ubn , sizeof(ubn));

    write(fd , &fbn , sizeof(fbn));
    write(fd ,ub, sizeof(char)*(n/8+1));

    lseek(fd ,1024 + foundfreeblockno*s , SEEK_SET);

    write(fd, towrite , sizeof(char)*s);

    printf("free block found %d\n" ,foundfreeblockno);

    close(fd);
    free(towrite);
    free(ub);
}

int free_block(const char *fname , int bno)
{
    int fd;
    int n, s, ubn ,fbn;
    char *ub;
    char temp ;

    fd  = open(fname , O_CREAT|O_RDWR, 0666);
    if(fd == -1)
    {
        perror("open fail");
        return -1;
    }
    lseek(fd , 0 ,SEEK_SET);
    read(fd ,&n , sizeof(n));
    read(fd ,&s , sizeof(s));
    read(fd ,&ubn , sizeof(ubn));
    read(fd ,&fbn , sizeof(fbn));
    ub = (char*)malloc((n/8 + 1)*sizeof(char));
    read(fd ,ub , (n/8+1)*sizeof(char));

    // setting ub
    int i, bitno;
    i = bno/8;
    bitno = bno%8;
    char t = (1<<bitno);
    t = ~t;

    ub[i] = (ub[i]&t);

    ubn = ubn-1;
    fbn = fbn+1;
    int foundfreeblockno = i*8 + bitno;
    char towrite = (char)malloc(sizeof(char)*s);
    memset(towrite , 0, sizeof(char)*s);

    lseek(fd , 0 ,SEEK_SET);

    write(fd , &n , sizeof(n));
    write(fd , &s , sizeof(s));
    write(fd , &ubn , sizeof(ubn));

    write(fd , &fbn , sizeof(fbn));
    write(fd ,ub, sizeof(char)*(n/8+1));

    lseek(fd ,1024 + foundfreeblockno*s , SEEK_SET);

    write(fd ,towrite ,sizeof(char)*s);

    printf("free block found %d\n" ,foundfreeblockno);

    close(fd);
    free(towrite);
    free(ub);
}
int readblock(const char *fname, int seq_no, int bno, char *buffer){

}
int writeblock(char *fname, int seq_no, int bno, char *buffer) {
    
}
int check_fs(const char *fname )
{
    int fd;
    int n, s, ubn ,fbn;
    char *ub;
    char temp ;

    fd  = open(fname , O_CREAT|O_RDWR, 0666);
    if(fd == -1)
    {
        perror("open fail");
        return -1;
    }
    lseek(fd , 0 ,SEEK_SET);
    read(fd ,&n , sizeof(n));
    read(fd ,&s , sizeof(s));
    read(fd ,&ubn , sizeof(ubn));
    read(fd ,&fbn , sizeof(fbn));
    ub = (char*)malloc((n/8 + 1)*sizeof(char));
    read(fd ,ub , (n/8+1)*sizeof(char));

    if(ubn+fbn > n)
    {
        printf("file corrupted: ubn + fbn > total \n" );
        return 1;
    }
    if(ubn+fbn < n)
    {
        printf("file corrupted: ubn + fbn < total \n" );
        return 1;
    }

    printf("file not corrupted \n");

    close(fd);
    free(ub);
}

int main(){
    // int init_File_dd(const char *fname , int bsize , int bno)
    // init_File_dd("newfile" , 2 , 40);  
    // int get_freeblock(const char *fname)
    // get_freeblock("newfile");
    // int free_block(const char *fname , int bno)
    // free_block("newfile" , 4);
    check_fs("newfile");
}