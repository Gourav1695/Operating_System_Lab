#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEQ 10      // maximum number of sequences
#define MAX_BLOCKS 512  // maximum number of blocks in a sequence
#define MAX_FILENAME 256 // maximum length of file name

struct superblock {
    int n;    // total number of blocks in the file
    int s;    // size (number of bytes) of each block
    int ubn;  // number of blocks used in any sequence
    int fbn;  // number of free blocks
    char *ub; // bit pattern showing used blocks
};

struct indexblock {
    int seq_no;   // sequence number of this index block
    int nblocks;  // number of blocks in this sequence
    int blocks[MAX_BLOCKS]; // block numbers in this sequence
};

int init_File_dd(const char *fname, int bsize, int bno);

int get_freeblock(const char *fname);

int free_block(const char *fname, int bno);

int check_fs(const char *fname);

int readblock(const char *fname, int seq_no, int bno, char *buffer);

int writeblock(char *fname, int seq_no, int bno, char *buffer);

int main() {
    char filename[MAX_FILENAME];
    strcpy(filename, "test.dat"); // name of the file to work on
    int bsize = 1024; // size of each block
    int nblocks = 1000; // total number of blocks in the file
    int seq_no = 0; // sequence number to work on
    int block_no = 5; // block number to read/write
    char buffer[bsize]; // buffer to hold data
    int i;

    // initialize the file
    int res = init_File_dd(filename, bsize, nblocks);
    if (res == -1) {
        printf("Failed to initialize the file.\n");
        return 1;
    }

    // write data to the 5th block of the 0th sequence
    for (i = 0; i < bsize; i++) {
        buffer[i] = i % 256;
    }
    res = writeblock(filename, seq_no, block_no, buffer);
    if (res == 0) {
        printf("Failed to write block %d of sequence %d.\n", block_no, seq_no);
        return 1;
    }

    // read data from the 5th block of the 0th sequence
    res = readblock(filename, seq_no, block_no, buffer);
    if (res == 0) {
        printf("Failed to read block %d of sequence %d.\n", block_no, seq_no);
        return 1;
    }
    for (i = 0; i < bsize; i++) {
        if (buffer[i] != i % 256) {
            printf("Data in block %d of sequence %d is corrupted.\n", block_no, seq_no);
            return 1;
        }
    }

    printf("Tests passed successfully.\n");
    return 0;
}

// int readblock(const char *fname, int seq_no, int bno, char *buffer) {
//     FILE *fp;
//     struct superblock sb;
//     struct indexblock ib;
//     int i, j, res;
//     int bsize, nblocks, seq_start, seq_size, ibno, block_no;

//     // open the file
//     fp = fopen(fname, "rb");
//     if (fp == NULL) {
//         printf("Failed to open file %s
int readblock(const char *fname, int seq_no, int bno, char *buffer) {
    int block_size = get_blocksize(fname);
    int index_block_num = get_index_blocknum(fname, seq_no);
    
    if (index_block_num == -1) { // Sequence doesn't exist
        return 0;
    }
    
    int block_num = get_block_num(fname, seq_no, bno);
    if (block_num == -1) { // Block doesn't exist
        return 0;
    }
    
    FILE *fp = fopen(fname, "rb");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", fname);
        return 0;
    }
    
    // Move file pointer to the correct block
    fseek(fp, block_num * block_size, SEEK_SET);
    
    // Read block data into buffer
    fread(buffer, block_size, 1, fp);
    
    fclose(fp);
    return 1;
}

int writeblock(char *fname, int seq_no, int bno, char *buffer) {
    int block_size = get_blocksize(fname);
    int index_block_num = get_index_blocknum(fname, seq_no);
    
    // If sequence doesn't exist, create it
    if (index_block_num == -1) {
        index_block_num = get_freeblock(fname);
        if (index_block_num == -1) { // No free block available
            return 0;
        }
        
        // Initialize index block with sequence info
        if (!init_seq(fname, index_block_num, seq_no)) {
            return 0;
        }
    }
    
    // If block doesn't exist, create it
    int block_num = get_block_num(fname, seq_no, bno);
    if (block_num == -1) {
        block_num = get_freeblock(fname);
        if (block_num == -1) { // No free block available
            return 0;
        }
        
        // Update index block with new block number
        if (!add_block_to_seq(fname, index_block_num, block_num)) {
            return 0;
        }
    }
    
    // Write block data to file
    FILE *fp = fopen(fname, "rb+");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", fname);
        return 0;
    }
    
    // Move file pointer to the correct block
    fseek(fp, block_num * block_size, SEEK_SET);
    
    // Write data to block
    fwrite(buffer, block_size, 1, fp);
    
    fclose(fp);
    return 1;
}