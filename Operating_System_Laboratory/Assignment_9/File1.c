#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BLOCK_SIZE 4096

// Function to initialize a new file system
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

    // Write back the updated metadata to the file
    if (lseek(fd, 0, SEEK_SET) < 0 ||
        write(fd, &n, sizeof(int)) < 0 ||
        write(fd, &s, sizeof(int)) < 0 ||
        write(fd, &ubn, sizeof(int)) < 0 ||
        write(fd, &fbn, sizeof(int)) < 0 ||
        write(fd, ub, bno) < 0) {
        perror("Failed to update metadata in file");
        close(fd);
        free(ub);
        return -1;
    }

    close(fd);
    free(ub);
    return 0;
}
// Function to get a free block from the file
int get_freeblock(const char *fname) {
    FileHeader header;
    
    // Open the file and read the header
    FILE *fp = fopen(fname, "r+b");
    if (!fp) {
        return -1;  // Failed to open file
    }
    fread(&header, sizeof(header), 1, fp);
    
    // Find the first free block
    int blockno = -1;
    for (int i = 0; i < header.n; i++) {
        if (header.ub[i] == 0) {
            blockno = i;
            break;
        }
    }
    
    if (blockno == -1) {
        fclose(fp);
        return -1;  // No free block available
    }
    
    // Mark the block as used and update the header
    header.ub[blockno] = 1;
    header.ubn++;
    header.fbn--;
    
    // Write the header back to the file
    fseek(fp, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, fp);
    
    // Write 1's to the block
    char *block = calloc(header.s, sizeof(char));
    memset(block, 1, header.s);
    fseek(fp, (blockno+1)*header.s, SEEK_SET);
    fwrite(block, sizeof(char), header.s, fp);
    free(block);
    
    fclose(fp);
    return blockno;  // Return the block number
}

int main(){
    int x=init_File_dd("abc.txt",4096,2048);
    printf("%d",x);
}