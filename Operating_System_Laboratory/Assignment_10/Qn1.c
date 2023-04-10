// 2020CSB010_GOURAV_KUMAR_SHAW

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 512 // maximum number of blocks in a sequence
const int HEADER_SIZE = 1024;
#define MAX_FILENAME 256;
// const int BLOCK_SIZE = 4096;

struct indexblock
{
    int seq_no;             
    int nblocks;           
    int blocks[MAX_BLOCKS]; 
};
// Struct to store file information
typedef struct
{
    int n;    // total number of blocks
    int s;    // size of each block
    int ubn;  // number of used blocks
    int fbn;  // number of free blocks
    char *ub; // bitmap showing which blocks are used
} FileHeader;

// Function to initialize a new file system
int init_File_dd(const char *fname, int bsize, int bno)
{
    int fd = open(fname, O_RDWR | O_CREAT, 0666);
    if (fd < 0)
    {
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

    memcpy(ub, &ubn, sizeof(int));               // copy ubn to ub array
    memcpy(ub + sizeof(int), &fbn, sizeof(int)); // copy fbn to ub array

    // Write metadata to the file
    if (write(fd, &n, sizeof(int)) < 0 ||
        write(fd, &s, sizeof(int)) < 0 ||
        write(fd, &ubn, sizeof(int)) < 0 ||
        write(fd, &fbn, sizeof(int)) < 0 ||
        write(fd, ub, bno) < 0)
    {
        perror("Failed to write metadata to file");
        close(fd);
        free(ub);
        return -1;
    }

    // Allocate blocks for metadata and set them as used
    for (int i = 0; i < 1024 / BLOCK_SIZE; i++)
    {
        if (write(fd, "", BLOCK_SIZE) < 0)
        {
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
        write(fd, ub, bno) < 0)
    {
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
int get_freeblock(const char *fname)
{
    FileHeader header;

    // Open the file and read the header
    FILE *fp = fopen(fname, "r+b");
    if (!fp)
    {
        return -1; // Failed to open file
    }
    fread(&header, sizeof(header), 1, fp);

    // Find the first free block
    int blockno = -1;
    for (int i = 0; i < header.n; i++)
    {
        if (header.ub[i] == 0)
        {
            blockno = i;
            break;
        }
    }

    if (blockno == -1)
    {
        fclose(fp);
        return -1; // No free block available
    }

    // Mark the block as used and update the header
    header.ub[blockno] = 1;
    header.ubn++;
    header.fbn--;

    // Write the header back to the file
    fseek(fp, 0, SEEK_SET); // fseek is a function in the C standard library that allows you to set the file position indicator for a given file stream. The syntax for fseek is as follows

    // int fseek(FILE *stream, long int offset, int whence);
    fwrite(&header, sizeof(header), 1, fp);

    // Write 1's to the block
    char *block = calloc(header.s, sizeof(char));
    memset(block, 1, header.s);
    fseek(fp, (blockno + 1) * header.s, SEEK_SET);
    fwrite(block, sizeof(char), header.s, fp);
    free(block);

    fclose(fp);
    return blockno; // Return the block number
}

int free_block(const char *fname, int bno)
{
    FILE *fp = fopen(fname, "r+b");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 0;
    }

    // Read header information
    fseek(fp, 0, SEEK_SET);
    int n, s, ubn, fbn;
    if (fread(&n, sizeof(int), 1, fp) != 1 ||
        fread(&s, sizeof(int), 1, fp) != 1 ||
        fread(&ubn, sizeof(int), 1, fp) != 1 ||
        fread(&fbn, sizeof(int), 1, fp) != 1)
    {
        perror("Error reading header information");
        fclose(fp);
        return 0;
    }

    // Check if block number is valid
    if (bno < 0 || bno >= n)
    {
        fprintf(stderr, "Error: invalid block number %d\n", bno);
        fclose(fp);
        return 0;
    }

    // Calculate bit position in ub
    int ub_pos = HEADER_SIZE + (bno / 8);
    int ub_bit = 7 - (bno % 8);

    // Check if block is already free
    fseek(fp, ub_pos, SEEK_SET);
    unsigned char ub_byte;
    if (fread(&ub_byte, sizeof(unsigned char), 1, fp) != 1)
    {
        perror("Error reading ub byte");
        fclose(fp);
        return 0;
    }
    if ((ub_byte & (1 << ub_bit)) == 0)
    {
        fprintf(stderr, "Error: block %d is already free\n", bno);
        fclose(fp);
        return 0;
    }

    // Free block
    fseek(fp, HEADER_SIZE + (bno * BLOCK_SIZE), SEEK_SET);
    unsigned char zero = 0;
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (fwrite(&zero, sizeof(unsigned char), 1, fp) != 1)
        {
            perror("Error writing block");
            fclose(fp);
            return 0;
        }
    }
    fseek(fp, ub_pos, SEEK_SET);
    ub_byte &= ~(1 << ub_bit);
    if (fwrite(&ub_byte, sizeof(unsigned char), 1, fp) != 1)
    {
        perror("Error writing ub byte");
        fclose(fp);
        return 0;
    }
    ubn--;
    fbn++;

    // Update header information
    fseek(fp, 0, SEEK_SET);
    if (fwrite(&n, sizeof(int), 1, fp) != 1 ||
        fwrite(&s, sizeof(int), 1, fp) != 1 ||
        fwrite(&ubn, sizeof(int), 1, fp) != 1 ||
        fwrite(&fbn, sizeof(int), 1, fp) != 1)
    {
        perror("Error writing header information");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

int check_fs(const char *fname)
{
    int fd;
    int n, s, ubn, fbn;
    char *ub;
    char temp;

    fd = open(fname, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("open fail");
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    read(fd, &n, sizeof(n));
    read(fd, &s, sizeof(s));
    read(fd, &ubn, sizeof(ubn));
    read(fd, &fbn, sizeof(fbn));
    ub = (char *)malloc((n / 8 + 1) * sizeof(char));
    read(fd, ub, (n / 8 + 1) * sizeof(char));

    if (ubn + fbn > n)
    {
        printf("file corrupted: ubn + fbn > total \n");
        return 1;
    }
    if (ubn + fbn < n)
    {
        printf("file corrupted: ubn + fbn < total \n");
        return 1;
    }

    printf("file not corrupted \n");

    close(fd);
    free(ub);
}
int readblock(const char *fname, int seq_no, int bno, char *buffer)
{
    int block_size = get_blocksize(fname);
    int index_block_num = get_index_blocknum(fname, seq_no);

    if (index_block_num == -1)
    { // Sequence doesn't exist
        return 0;
    }

    int block_num = get_block_num(fname, seq_no, bno);
    if (block_num == -1)
    { // Block doesn't exist
        return 0;
    }

    FILE *fp = fopen(fname, "rb");
    if (fp == NULL)
    {
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
int writeblock(char *fname, int seq_no, int bno, char *buffer)
{
    int block_size = get_blocksize(fname);
    int index_block_num = get_index_blocknum(fname, seq_no);

    // If sequence doesn't exist, create it
    if (index_block_num == -1)
    {
        index_block_num = get_freeblock(fname);
        if (index_block_num == -1)
        { // No free block available
            return 0;
        }

        // Initialize index block with sequence info
        if (!init_seq(fname, index_block_num, seq_no))
        {
            return 0;
        }
    }

    // If block doesn't exist then we will have to create it
    int block_num = get_block_num(fname, seq_no, bno);
    if (block_num == -1)
    {
        block_num = get_freeblock(fname);
        if (block_num == -1)
        { // No free block available
            return 0;
        }

        // Update index block with new block number
        if (!add_block_to_seq(fname, index_block_num, block_num))
        {
            return 0;
        }
    }

    // Write block data to file
    FILE *fp = fopen(fname, "rb+");
    if (fp == NULL)
    {
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

// int main()
// {
//     // int x = init_File_dd("abc.txt", 4096, 2048);
//     // //  int init_File_dd(const char *fname , int bsize , int bno)
//     // init_File_dd("newfile" , 2 , 40);
//     // // int get_freeblock(const char *fname)
//     // get_freeblock("newfile");
//     // // int free_block(const char *fname , int bno)
//     // free_block("newfile" , 4);
//     // check_fs("newfile");
//     // readblock();
//     // printf("%d", x);
// }
int main()
{
    char filename[256];
    strcpy(filename, "test.txt"); 
    int block_no = 3;             
    int nblocks = 512;          
    int bsize = 1024;            
    int seq_no = 0;               
    char buffer[bsize];           
    int i;

    // initialize the file
    int res = init_File_dd(filename, bsize, nblocks);
    if (res == -1)
    {
        printf("Failed to initialize the file.\n");
        return 1;
    }

    for (i = 0; i < bsize; i++)
    {
        buffer[i] = i % 256;
    }
    res = writeblock(filename, seq_no, block_no, buffer);
    if (res == 0)
    {
        printf("Failed to write block %d of sequence %d.\n", block_no, seq_no);
        return 1;
    }

    res = readblock(filename, seq_no, block_no, buffer);
    if (res == 0)
    {
        printf("Failed to read block %d of sequence %d.\n", block_no, seq_no);
        return 1;
    }
    for (i = 0; i < bsize; i++)
    {
        if (buffer[i] != i % 256)
        {
            printf("Data in block %d of sequence %d is corrupted.\n", block_no, seq_no);
            return 1;
        }
    }

    printf("Tests passed .\n");
    return 0;
}