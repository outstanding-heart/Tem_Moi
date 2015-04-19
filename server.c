/*
 *  The test model for server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <my_global.h>
#include <mysql.h>

//#define MAX 100
#define DMAX 20
#define MAX 1024
#define NMAX 4
#define PORT 9000
#define PUF_SIZE 1024
#define SA struct sockaddr
#define max(a,b)	((a) > (b) ? (a) : (b))
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


typedef struct TMnode
{
	char id[NMAX];//传感器编号
	char tem[NMAX];//温度
	char moi[NMAX];//湿度
	char date[DMAX];//日期时间
	char place[NMAX];//测点位置
	char kind[NMAX];//库区或运输工具类别
}TMnode;

typedef struct TMlist
{
	TMnode tmnode[MAX];
	int length;
}TMlist;

//********************Write to Mysql*******************************

void WriteToMysql(TMnode node)
{
	MYSQL *sql;
	char DatabaseName[MAX], Ip[MAX], Name[MAX], PassWord[MAX], Test[MAX];
	char Create_s[MAX], Create_tablename[MAX], Create_tablefield[MAX],
		 Insert_s[MAX], Insert_tablename[MAX], Insert_tablevalues[MAX];
	
	//*************Create a new table************************
	sql = mysql_init(NULL);
	sprintf(Ip, "127.0.0.1");
	sprintf(Name, "root");
	sprintf(PassWord, "19921102");
	sprintf(DatabaseName, "mydatabase");
	if(mysql_real_connect(sql,Ip,Name,PassWord,DatabaseName,0,NULL,0) == NULL)
	{
		printf("connect error\n");
		exit(1);
	}
	sprintf(Create_tablename, "note_%s", node.id);
	sprintf(Create_tablefield,"id int(5) NOT NULL auto_increment,Tem char(4),Moi char(4),Date char(20),Place char(4),Kind char(4),PRIMARY KEY (`id`)");
	sprintf(Create_s, "create table %s(%s)",Create_tablename,Create_tablefield);
	sprintf(Test, "show table %s", Create_tablename);
	mysql_query(sql, Create_s);
	printf("Create success!\n");

	//*************Install a new data************************
	
	sprintf(Insert_tablename, Create_tablename);
	sprintf(Insert_tablevalues, "id,'%s','%s','%s','%s','%s'", node.tem,node.moi,node.date,node.place,node.kind);
	sprintf(Insert_s, "insert into %s values (%s)",Insert_tablename,Insert_tablevalues);
	printf("%s\n", Insert_s);
	if(mysql_query(sql, Insert_s) != 0)
	{
		printf("Insert error\n");
		exit(1);
	}
	else printf("Insert success!\n");

	//*************Close sql*********************************
	
	mysql_close(sql);

}

//********************Write to Mysql*******************************


//**********************Receive Information************************

void RecvInfo(int sockfd, int i)
{
	TMlist list;
	int nid, ntem, nmoi, ndate, nplace, nkind;
	char buf1[MAX],buf[MAX];
	
	bzero(buf, sizeof(buf));
	bzero(buf1, sizeof(buf1));
	bzero(list.tmnode[i].id, sizeof(list.tmnode[i].id));
	bzero(list.tmnode[i].tem, sizeof(list.tmnode[i].tem));
	bzero(list.tmnode[i].moi, sizeof(list.tmnode[i].moi));
	bzero(list.tmnode[i].date, sizeof(list.tmnode[i].date));
	bzero(list.tmnode[i].place, sizeof(list.tmnode[i].place));
	bzero(list.tmnode[i].kind, sizeof(list.tmnode[i].kind));

	read(sockfd, buf1, MAX);
	printf("%s\n", buf1);
	sprintf(buf,"OK");
	write(sockfd, buf, MAX);
	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "OVER");

	if((nid = read(sockfd, list.tmnode[i].id, NMAX)) < 0)
	{
		printf("read id error\n");
		return;
	}
	else printf("%s\n", list.tmnode[i].id);
	write(sockfd, buf1, MAX);
	if((ntem = read(sockfd, list.tmnode[i].tem, NMAX)) < 0)
	{
		printf("read tem error\n");
		return;
	}
	else printf("%s\n", list.tmnode[i].id);
	write(sockfd, buf1, MAX);
	if((nmoi = read(sockfd, list.tmnode[i].moi, NMAX)) < 0)
	{
		printf("read moi error\n");
		return;
	}
	write(sockfd, buf1, MAX);
	if((ndate = read(sockfd, list.tmnode[i].date, DMAX)) < 0)
	{
		printf("read date error\n");
		return;
	}
	write(sockfd, buf1, MAX);
	if((nplace = read(sockfd, list.tmnode[i].place, NMAX)) < 0)
	{
		printf("read place error\n");
		return;
	}
	write(sockfd, buf1, MAX);
	if((nkind = read(sockfd, list.tmnode[i].kind, NMAX)) < 0)
	{
		printf("read kind error\n");
		return;
	}

	printf("The id is: %s\n", list.tmnode[i].id);
	printf("The temperature is: %s\n", list.tmnode[i].tem);
	printf("The moisture is: %s\n", list.tmnode[i].moi);
	printf("The date is: %s\n", list.tmnode[i].date);
	printf("The place is: %s\n", list.tmnode[i].place);
	printf("The kind is: %s\n", list.tmnode[i].kind);
	
	bzero(buf1, sizeof(buf1));
	sprintf(buf1, "All over");
	write(sockfd, buf1, MAX);
	printf("%s\n", buf1);

	WriteToMysql(list.tmnode[i]);

}

//**************************************************
void sig_chld(int signo)//处理僵死程序
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d teminated\n", pid);
	return;
}

int main(int argc, char **argv)
{
	int listenfd, i, j, maxi, maxfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	char buf1[MAX], sendline[MAX];
	//FILE *fp = stdin;
	fd_set rset,allset;
	socklen_t clilen;
	pid_t childpid;
	struct sockaddr_in servaddr, cliaddr;


//*******************socket*****************************
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1)
	{
		printf("Can't create socket\n");
		exit(1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd,(SA *)&servaddr,sizeof(SA)) == -1)
	{
		printf("bind error\n");
		exit(1);
	}
	if(listen(listenfd,2) == -1)
	{
		printf("listen error\n");
		exit(1);
	}
//*******************socket*****************************
	
	if(signal(SIGCHLD, sig_chld) == -1)// must call waitpid()
	{
		printf("signal error\n");
		exit(1);
	}

//*****************************************************

	printf("Server start!\n");
	for( ; ; )
	{
		clilen = sizeof(cliaddr);
		if((connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0)
		{
			printf("accept error\n");
			continue;
		}

		if((childpid = fork()) == 0)
		{
			close(listenfd);
			printf("One client has connected!\n");
			RecvInfo(connfd, 0);
			exit(0);
		}
		if(close(connfd) == -1)
		{
			printf("close error\n");
			exit(1);
		}
	}

//*****************************************************
	printf("Server is closed\n");
	return 0;
}
