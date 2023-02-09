#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

int main()
{
    void *shared_memory;
    char buff[100];
    int shmid;
    //  int shmget(key_t key, size_t size, int shmflg);
    /*  IPC_CREAT
              Create  a  new  segment.  If this flag is not used, then shmget() will find the segment associated with
              key and check to see if the user has permission to access the segment.*/

    shmid = shmget((key_t)1122, 1024, 0666 | IPC_CREAT);
    // creates shared memory segment with key 1122 having memory 1024
    printf("Key of shared memory is %d\n", shmid);
    // void *shmat(int shmid, const void *shmaddr, int shmflg);
    //  If shmaddr is NULL, the system chooses a suitable (unused) page-aligned address to attach the segment.
    shared_memory = shmat(shmid, NULL, 0); // process attached to shared memory segment
    // When the flags argument is set to 0, the function will attach the shared memory segment with the default settings. This usually means that the shared memory segment is attached as read-write, so the calling process can read and write to the segment, and that it is attached at the first available address in the process's address space.

    printf("Process attached at %p\n", shared_memory);
    // this prints the address where the segment is attached
    printf("Enter some data to write to shared memory\n");
    read(0, buff, 100); // get some input from user
    /*The function read(0, buff, 100) is a function from the Unix/Linux system programming API for file input/output.

    read is a function that reads data from a file descriptor and stores it in a buffer. The first argument, 0, is the file descriptor for standard input (stdin), which is usually the keyboard for a terminal session. The second argument, buff, is a pointer to the buffer where the data read from stdin will be stored. The third argument, 100, is the maximum number of bytes to be read from stdin and stored in the buffer.

    This function will block until at least one byte of data is available for reading from stdin, or until an error occurs. If successful, the function returns the number of bytes actually read from stdin and stored in the buffer. If an error occurs, the function returns -1.*/
    strcpy(shared_memory, buff); // data written to shared memory
    printf("You wrote : %s\n", (char *)shared_memory);
}
//  int shmctl(int shmid, int cmd, struct shmid_ds *buf);
/* shmctl()  performs  the control operation specified by cmd on the System V shared memory segment whose identi‐
       fier is given in shmid.*/
/*The shmctl() function is used to control operations on a System V shared memory segment. It is a system call in Unix-like operating systems for performing operations on shared memory segments. Some of the operations that can be performed using shmctl() include getting information about the shared memory segment, setting its ownership and permissions, or marking it for removal. The shmctl() function is declared in the sys/shm.h header file.*/
// key_t ftok(const char *pathname, int proj_id);
/*
ftok use 

ftok() is a function in the Unix/Linux operating system that is used to generate a unique key for a System V shared memory segment or a System V semaphore. The key is created by combining the file pathname and an identifier. The function is declared in the sys/ipc.h header file. The key is used as an argument for shmget() or semget() functions to identify a shared memory segment or a semaphore set, respectively.

The purpose of ftok() is to ensure that each shared memory segment or semaphore set has a unique key, even if it is created by different processes. This helps to prevent accidental access to shared memory segments or semaphore sets that are intended for other processes.*/




// address space meaning in operating systems
/*In operating systems, the address space refers to the range of memory addresses that a process can access. Each process in an operating system has its own virtual address space, which is a contiguous range of virtual memory addresses that the process can use to store its data and code. The operating system maps these virtual addresses to physical memory addresses, allowing the process to access its memory as if it had exclusive use of the physical memory.

The address space of a process is usually divided into several regions, including the text segment for executable code, the data segment for read-write data, and the stack segment for automatic variables and function call frames. The address space of a process is isolated from the address space of other processes, which means that each process has its own private memory and cannot access the memory of other processes. This provides protection and data isolation between processes, ensuring that a process cannot interfere with the memory or execution of another process.
*/


/*
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
where shmid is the shared memory identifier, cmd is the command to perform on the shared memory segment, and buf is a pointer to a shmid_ds structure that holds information about the shared memory segment.

Possible values for cmd include:

    IPC_STAT: retrieve information about the shared memory segment and store it in buf.
    IPC_SET: set information about the shared memory segment using data in buf.
    IPC_RMID: remove the shared memory segment.

shmctl() returns 0 on success and -1 on error, with errno set to indicate the error.


*/

/*
IPC stands for Inter-Process Communication, and refers to the mechanisms that allow processes to exchange data and coordinate with each other on a computer system. IPC provides the means for separate processes to communicate with each other and synchronize their actions, allowing for efficient collaboration and the sharing of resources.

There are several types of IPC mechanisms in operating systems, including:

    Pipes: a simple form of IPC that allows processes to communicate by writing to and reading from a common buffer.

    Named Pipes: a more advanced form of IPC that provides a way for processes to communicate across different systems.

    Message Queues: a mechanism that allows processes to send and receive messages in a First-In-First-Out (FIFO) manner.

    Shared Memory: a low-level form of IPC that allows processes to share a common region of memory.

    Semaphores: a mechanism that provides a way for processes to coordinate and synchronize their access to shared resources.

    Sockets: a network-based form of IPC that allows processes to communicate over a network.

Each of these mechanisms provides different advantages and trade-offs, and the choice of which one to use depends on the specific requirements of the application.
*/


/*
    sleep is a function that temporarily suspends the execution of a process for a specified amount of time. This allows other processes to run during this interval, and the process will resume after the specified time has elapsed.

    wait is a function that blocks a process until one of its child processes has terminated. The wait function returns information about the child process that terminated, such as its exit status.

    In other words, sleep is used to introduce delays in a process's execution, while wait is used to synchronize the execution of parent and child processes
*/


/*
exit and return are two ways of indicating the termination of a program in an operating system.

exit is a system call that terminates the current process, returning a status code to the operating system. When a process calls exit, it terminates all of its threads, releases all its resources, and notifies its parent process that it has terminated.

return is a statement used within a program to return control to the calling function or to the operating system. When a function returns, it passes a value back to the calling function or to the operating system, and control is transferred to the point immediately after the function was called. Unlike exit, return does not terminate the entire process, it only returns control to the calling function.

In summary, exit terminates the entire process, whereas return terminates only the current function and returns control to the calling function
*/