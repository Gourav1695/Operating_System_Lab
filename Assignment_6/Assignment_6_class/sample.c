/* sem1.c */
#include <stdio.h>
#include <sys/types.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/ipc.h> /* for semget(2) ftok(3) semop(2) semctl(2) */
#include <sys/sem.h> /* for semget(2) semop(2) semctl(2) */
#include <unistd.h> /* for fork(2) */

#include <stdlib.h> /* for exit(3) */


#define NO_SEM	1

#define P(s) semop(s, &Pop, 1);//int semop(int semid, struct sembuf *sops, size_t nsops);
#define V(s) semop(s, &Vop, 1);



struct sembuf Pop;
struct sembuf Vop;



int main() {
	key_t mykey;
	pid_t pid;

	int semid;

	int status;

	union semun {
		int              val;    /* Value for SETVAL */
		struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
		unsigned short  *array;  /* Array for GETALL, SETALL */
		struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
	} setvalArg;

	setvalArg.val = 1;


	/* struct sembuf has the following fields */
	//unsigned short sem_num;  /* semaphore number */
        //short          sem_op;   /* semaphore operation */
        //short          sem_flg;  /* operation flags */



/*
P(s) is a macro for a semaphore "wait" operation on the semaphore with the identifier s. It uses the semop() system call with the Pop argument, which is a struct representing a semaphore operation that will decrement the semaphore value by 1.

V(s) is a macro for a semaphore "signal" operation on the semaphore with the identifier s. It uses the semop() system call with the Vop argument, which is a struct representing a semaphore operation that will increment the semaphore value by 1.

These macros can be used to provide mutual exclusion or synchronization between multiple processes accessing a shared resource.
*/

	Pop.sem_num = 0;
	Pop.sem_op = -1;
	Pop.sem_flg = SEM_UNDO;
	/*
	Pop.sem_flg = SEM_UNDO; sets the sem_flg field of Pop to SEM_UNDO, which specifies that the semaphore operation should be undone if the process terminates unexpectedly. This ensures that the semaphore value is not left in an inconsistent state if a process crashes or is killed.
	*/


	Vop.sem_num = 0;
	Vop.sem_op = 1;
	Vop.sem_flg = SEM_UNDO;
	/*
	Vop.sem_flg = SEM_UNDO; sets the sem_flg field of Vop to SEM_UNDO, which specifies that the semaphore operation should be undone if the process terminates unexpectedly. This ensures that the semaphore value is not left in an inconsistent state if a process crashes or is killed.

	*/

	// key_t ftok(const char *pathname, int proj_id);
	mykey = ftok("file.txt", 1);
	
	if (mykey == -1) {
		perror("ftok() failed");
		exit(1);
	}

	//       int semget(key_t key, int nsems, int semflg);
	semid = semget(mykey, NO_SEM, IPC_CREAT | 0777);
	/*
	In this case, mykey is the key value used to identify the semaphore set, which is usually obtained using ftok() to generate a unique key based on a file path and an integer ID.

	NO_SEM is the number of semaphores to be created in the set, which is set to a specific value determined by the program.

	IPC_CREAT | 0777 is a bit-wise OR operator that combines two values: IPC_CREAT, which specifies that a new semaphore set should be created if the key doesn't already exist, and 0777, which sets the permissions for the semaphore set to read, write, and execute for the owner, group, and others.
	*/
	if(semid == -1) {
		perror("semget() failed");
		exit(1);
	}

	// int semctl(int semid, int semnum, int cmd, ...);
	status = semctl(semid, 0, SETVAL, setvalArg);
	if(status == -1) {
		perror("semctl() failed");
		exit(1);
	}

	pid = fork();
	if (pid == -1) {
		perror("fork() failed");
		exit(1);
	}

	if (pid == 0) {
		/* child process task */
		P(semid);
		fprintf(stdout, "Child.\n");
		V(semid);
	} else {
		/* parent process task */
		P(semid);
		fprintf(stdout, "Parent.\n");
		V(semid);
	}

}