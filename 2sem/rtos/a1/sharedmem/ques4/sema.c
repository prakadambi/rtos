//Q4)A program to illustrate the use of semaphores in prevention of accidental overwrites while read is in progress
//parent process is reading from a variable and using it to perform some computation while child tries to write to the same variable which is prevented using a semaphore

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_RETRIES 10

union semun {
    int val;//for setval
    struct semid_ds *buf;
    ushort *array;//getter setter array
};

int initsem(key_t key, int nsems)  /* key from ftok() */
{
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid;

    semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);//create a semaphore

    printf("Semget() returned %d \n",semid);

    if (semid >= 0) 
    { /* we got it first */
        sb.sem_op = 1; sb.sem_flg = 0;//sem op is +ve will be added to the semaphore
        arg.val = 1;

        printf("press return\n"); getchar();

        for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) 
        { 
            /* do a semop() to "free" the semaphores. */
            /* this sets the sem_otime field, as needed below. */
            if (semop(semid, &sb, 1) == -1) 
            {
                int e = errno;
                semctl(semid, 0, IPC_RMID); /* clean up */
                errno = e;
                return -1; /* error, check errno */
            }
        }

    } 
    else if (errno == EEXIST) 
    { /* someone else got it first */
        int ready = 0;

        semid = semget(key, nsems, 0); /* get the id */
        if (semid < 0) return semid; /* error, check errno */

        /* wait for other process to initialize the semaphore: */
        arg.buf = &buf;
        for(i = 0; i < MAX_RETRIES && !ready; i++) 
        {
            semctl(semid, nsems-1, IPC_STAT, arg);//copy from kernel ds into semi_ds
            if (arg.buf->sem_otime != 0) //last semop time
            {
                ready = 1;
            } 
            else 
            {
                sleep(1);
            }
        }
        if (!ready) {
            errno = ETIME;
            return -1;
        }
    } else {
        return semid; /* error, check errno */
    }
    //printf("%d %d\n",sb.sem_op,arg.val);
    return semid;
}

int main(void)
{
    key_t key;
    int semid,i,res;
    struct sembuf sb;
    
    sb.sem_num = 0;
    sb.sem_op = -1;  /* set to allocate resource */
    sb.sem_flg = SEM_UNDO|IPC_NOWAIT;//undo after process terminates

    if ((key = ftok("sema.c", 'J')) == -1) {
        perror("ftok");
        exit(1);
    }
    printf("ftok() returned \n");
    /* grab the semaphore set created by seminit.c: */
    if ((semid = initsem(key, 1)) == -1) {
        perror("initsem");
        exit(1);
    }

    printf("initsem returned \n");

   
    printf("Enter any number\n");
    scanf("%d",&i);

    printf("\nTrying to lock...\n");

    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
    
    printf("locked\n\n");
    printf("reading from a variable:this process can't be interrupted.Critical section\n\n");

    if(!fork())//child process;
    {printf("child process:trying to write to variable\n");
     printf("trying to lock the semaphore\n");
     if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        printf("child process exiting.Unable to obtain a lock on the semaphore\n\n");
        exit(1);}
    i=20;//trying to write to variable
    exit(0);
    
    }
    sleep(1);
    res=10*i;//read from variable and do some computation

    sb.sem_op = 1;//computation complete;free variable
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }

    printf("Parent process:Read done:Unlocked\n");

    return 0;
}
