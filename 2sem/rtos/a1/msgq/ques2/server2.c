//Q2)program to demonstrate a message queue--server side which performs the computation\

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include<string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf {
    long mtype;
    char mtext[200];
};

int main(void)
{
    struct my_msgbuf buf;
    int msqid;
	
   int op1,op2;
	float res;
	char cid[6];
	char a[4],b[4],op[4];
	char oper;

	char* str="ClientID operand1 operator operand2 result\n";

    	key_t key;
	FILE *fd;
	int l,i;
	fd=fopen("res.txt","a");
	//printf("opening file\n");

	if(fd==NULL)
		printf("file error");

	fprintf(fd,"%s",str);//write to file the format
	fclose(fd);

 	if ((key = ftok("client2.c", 'B')) == -1) {  /* same key as kirk.c */
        perror("ftok");
        exit(1);
    	}

	//printf("%d",key);
    	if ((msqid = msgget(key, 0644)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    	}
    
    	printf("Server: ready to compute.Send me an expression\n");

    for(;;) //server keeps listening for messages
	{
        if (msgrcv(msqid, &buf, sizeof buf.mtext, 0, 0) == -1) 
        {
            perror("msgrcv");
            exit(1);
        }
        
	//use strtok to split the space separated string
	strcpy(a ,strtok(buf.mtext, " "));//operand 1
	strcpy(op,strtok(NULL," "));//operator
	strcpy(b,strtok(NULL," "));//operand 2
	strcpy(cid,strtok(NULL," "));//client id
	
	
	op1=atoi(a);//convert to integer
	op2=atoi(b);
	oper=op[0];
			
		printf("Received:%d %c %d %s\n",op1,oper,op2,cid);

	switch(oper)//perform the desired operation
		{
		case '+':res=op1+op2;
				break;
		case '-':res=op1-op2;
				break;
		case '*':res=op1*op2;
				break;
		case '/':if(op2==0)
				{printf("division by zero error\n");
				continue;}
				res=(float)op1/op2;
				break;
		default:printf("unkown operator\n");
				break;
		}

	fd=fopen("res.txt","a");
	fprintf(fd,"\n%d\t%c\t%d\t%s\t%f\n",op1,oper,op2,cid,res);//write results to file
	fclose(fd);//close file
	printf("Result of computation is:%f \n",res);
    }
	
    return 0;
}
