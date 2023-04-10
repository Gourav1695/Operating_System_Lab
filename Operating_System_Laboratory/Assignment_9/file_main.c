#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BLOCK_SIZE 4096

// Struct to store file information
typedef struct {
    int n;      // total number of blocks
    int s;      // size of each block
    int ubn;    // number of used blocks
    int fbn;    // number of free blocks
    char *ub;   // bitmap showing which blocks are used
} FileHeader;

// Function to initialize a new file
int init_File_dd(const char *fname, int bsize, int bno) {
    FileHeader header;
    header.n = bno;
    header.s = bsize;
    header.ubn = 0;
    header.fbn = bno;
    header.ub = calloc(bno, sizeof(char));  // Initialize all bits to 0
    
    // Create the file and write the header
    FILE *fp = fopen(fname, "wb");
    if (!fp) {
        free(header.ub);
        return -1;  // Failed to create file
    }
     if (fseek(fp, bsize - 1, SEEK_SET) != 0) {
        fclose(fp);
        return -1;
    }
    sprintf(fp, "%d %d %d %d",bno ,bsize ,0, bno);
    // fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
    free(header.ub);
    
    return 0;  // Success
}

int main(){
    int x=init_File_dd("abc.txt",4096,2048);
    printf("%d",x);
}

