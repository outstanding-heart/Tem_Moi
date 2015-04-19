/*
 * The test model for client.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

#define max(a,b)	((a) > (b) ? (a) : (b))
#define MAX 1024
#define NodeNumMAX 100
#define DMAX 20
#define NMAX 4
#define PORT 9000
#define SA struct sockaddr
//#define PLACE "1"
//#define KIND "1"
//#define ID "1"
//#define BUF_SIZE 1024

typedef struct TMnode
{
	char id[NMAX];
	char tem[NMAX];
	char moi[NMAX];
	char date[DMAX];
	char place[NMAX];
	char kind[NMAX];
}TMnode;

typedef struct TMlist
{
	TMnode tmnode[NodeNumMAX];
	int length;
}TMlist;

//****************

void SendInfo(int sockfd)
{
	time_t ticks;
	struct tm *t;
	TMlist list;
	int i = 0, j;
	char buf1[MAX], buf2[MAX], name[MAX],
		 Id[NMAX], Tem[NMAX], Moi[NMAX], 
		 Date[DMAX], Place[NMAX], Kind[NMAX];

//	for(i=0;;i++)
//	{
		//********************************************************

		//fgets(name, MAX, stdin);
		//name[strlen(name)-1] = '\0';
			
		//**************Put in hte information*********************

		printf("temperature is: ");
		fgets(list.tmnode[i].tem, MAX, stdin);
		sprintf(list.tmnode[i].id, "1");
		list.tmnode[i].tem[strlen(list.tmnode[i].tem)-1] = '\0';
		printf("moisture is: ");
		fgets(list.tmnode[i].moi, MAX, stdin);
		list.tmnode[i].moi[strlen(list.tmnode[i].moi)-1] = '\0';
		ticks = time(NULL);
		t = localtime(&ticks);
		strftime(list.tmnode[i].date, 127, "%Y-%m-%d %H:%M:%S", t);
		sprintf(list.tmnode[i].place, "1");
		sprintf(list.tmnode[i].kind, "1");
		list.length = i;

		//**************Put in the information*********************
		

		printf("id is %s\n", list.tmnode[i].id);
		printf("temperature is %s\n", list.tmnode[i].tem);
		printf("moisture is %s\n", list.tmnode[i].moi);
		printf("date is %s\n", list.tmnode[i].date);
		printf("place is %s\n", list.tmnode[i].place);
		printf("kind is %s\n", list.tmnode[i].kind);
		

		sprintf(buf1, "begin to send");
		write(sockfd, buf1, strlen(buf1));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);

		write(sockfd, list.tmnode[i].id, strlen(list.tmnode[i].id));
		bzero(buf2, sizeof(buf2));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);

		write(sockfd, list.tmnode[i].tem, strlen(list.tmnode[i].tem));
		bzero(buf2, sizeof(buf2));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);
		
		write(sockfd, list.tmnode[i].moi, strlen(list.tmnode[i].moi));
		bzero(buf2, sizeof(buf2));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);
		
		write(sockfd, list.tmnode[i].date, strlen(list.tmnode[i].date));
		bzero(buf2, sizeof(buf2));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);
		
		write(sockfd, list.tmnode[i].place, strlen(list.tmnode[i].place));
		bzero(buf2, sizeof(buf2));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);
		
		write(sockfd, list.tmnode[i].kind, strlen(list.tmnode[i].kind));
		bzero(buf2, sizeof(buf2));
		read(sockfd, buf2, MAX);
		printf("%s\n", buf2);


		bzero(buf1, sizeof(buf1));
		bzero(buf2, sizeof(buf2));
//	}
	//	sleep(30);
}


int main(int argc, char **argv)
{
	int sockfd;
	char ip[] = "127.0.0.1", buf[MAX];
	struct sockaddr_in servaddr;

	//****************Socket*************
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		printf("Can't create socket\n");
		exit(1);
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);

	if(inet_aton(ip,&servaddr.sin_addr) == -1)
	{
		printf("addr convert error\n");
		exit(1);
	}

	if(connect(sockfd,(SA *)&servaddr,sizeof(SA)) == -1)
	{
		printf("Connect error\n");
		exit(1);
	}
	//****************Socket****************
	
	printf("1 is connect success!\n");

	//*********Send information**********
	
	SendInfo(sockfd);

	//***********************************

	close(sockfd);
	printf("Good bye!\n");
	return 0;
}
