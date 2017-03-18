//Q2)program to demonstrate a message queue--client side.client inputs two operands and an operator and sends it to the server

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>//ftok
#include <sys/msg.h>

pid_t getpid(void);

struct my_msgbuf {
    long mtype;//type
    char mtext[200];//message
};

int main(void)
{
    struct my_msgbuf buf;
    int msqid,i,l;
    key_t key;
     char pid[10];

		
	snprintf(pid, 6,"%d",(int)getpid());//pid is used as the client id
	//printf("%s\n",pid); 
	l=strlen(pid);

	//printf("%d",l);
    if ((key = ftok("client2.c", 'B')) == -1) {//ftok converts a pathname(and also a project id-B) to an IPC key and returns an object of type key_t;
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {//msgget takes in the key and returns a message queue identifier,create if queue doesn't already exist,644-read write owner,read-user group
        perror("msgget");
        exit(1);
    }
    
    printf("Enter an arithmetic expression in the form operand1 operator operand2, ^D to quit:\n");

    buf.mtype = 1; /* we don't really care in this case */

        while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL)  {
       
	
        int len = strlen(buf.mtext);
	buf.mtext[len-1] = ' ';//add a space before appending client id
	
	for(i=0;i<l;i++)//append the client id to the message
		buf.mtext[len+i]=pid[i];
	 buf.mtext[len+i] = '\0';

	//printf("%s %lu",buf.mtext,strlen(buf.mtext));

        if (msgsnd(msqid, &buf, strlen(buf.mtext), 0) == -1) //send a msg
            perror("msgsnd");
	
    }

    if (msgctl(msqid, IPC_RMID, NULL) == -1) //remove the message queue awakening all reader and writer programs
    {
        perror("msgctl");
        exit(1);
    }

    return 0;
}
