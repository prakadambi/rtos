#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

//Q4 a) A program to illustrate shared memory using fork.The format of the command line arguments are r/w flag for the parent,data;if parent writes,then child reads.IF parent reads,then child writes.

int main(int argc, char *argv[])
{
    key_t key;
    int shmid;
    char *data;
    int mode;

    if (argc !=3) {//command line arguments are the read/write flag for parent;data to be written either by parent/child
        fprintf(stderr, "usage: read/write option for the parent, [data_to_write]\n");
        exit(1);
    }

    /* make the key: */
    if ((key = ftok("shmem.c", 'R')) == -1) {
        perror("ftok");
        exit(1);
    }

    /* connect to (and possibly create) the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, (void *)0, 0);//id,address,flag;null implies system chooses its own address
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

     if (*argv[1]=='w') {//w implies parent writes to the segment
        printf("Parent:writing to segment: \"%s\"\n", argv[2]);
        strncpy(data, argv[2], SHM_SIZE);
    } else//else parent reads from teh segment
        printf("Parent:segment contains: \"%s\"\n", data);

    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

	if(!fork())//this is the child process
	{

	    /* make the key: */
	    if ((key = ftok("shmem.c", 'R')) == -1) {
		perror("ftok");
		exit(1);
	    }

	    /* connect to (and possibly create) the segment: */
	    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	    }

	    /* attach to the segment to get a pointer to it: */
	    data = shmat(shmid, (void *)0, 0);
	    if (data == (char *)(-1)) {
		perror("shmat");
		exit(1);
	    }

	    /* read or modify the segment, based on the command line: */
	    if (*argv[1]=='r') {//r implies parent is reading therefore child writes
		printf("Child:writing to segment: \"%s\"\n", argv[2]);
		strncpy(data, argv[2], SHM_SIZE);
	    } else//else child reads
		printf("Child:segment contains: \"%s\"\n", data);

	    /* detach from the segment: */
	    if (shmdt(data) == -1) {
		perror("shmdt");
		exit(1);
	    }

		exit(0);//work of the child is done;exit
	}
   
    return 0;
}
