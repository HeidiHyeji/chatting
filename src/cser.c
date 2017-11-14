#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/select.h>

#define MAXLINE 100
#define MAX_CLIENT 1000
#define CHATDATA 1024
#define INVALID_SOCK -1

char greeting[]="Welcome to chatting room\n";
char ERROR[]= "Sorry No More Connection\n";
char quit[]="/quit\n";
char list[]="/list\n";
char smsg[]="/smsg";
char smsg_quit[]="/smsg_quit\n";

struct List_c{
	int socket_num;
	char nick[CHATDATA];
	char ip[40];
	int port;
}list_c[MAX_CLIENT];

int
pushClient(int connfd,char* c_nick,char* c_ip,int c_port){
	int i;

	for(i=0;i<MAX_CLIENT;i++){
		if(list_c[i].socket_num==INVALID_SOCK){
			list_c[i].socket_num=connfd;
			strcpy(list_c[i].nick,c_nick);
			strcpy(list_c[i].ip,c_ip);
			list_c[i].port=c_port;
			return i;
		}
	}

	if(i==MAX_CLIENT)
		return -1;
}

int popClient(int s)
{
	int i;
	
	
	for(i =0; i<MAX_CLIENT;i++){
		if(s==list_c[i].socket_num){
			list_c[i].socket_num=INVALID_SOCK;
			memset(list_c[i].nick,0,sizeof(list_c[i].nick));
			memset(list_c[i].ip,0,sizeof(list_c[i].ip));
			break;
		}
	}
	close(s);
	return 0;
}
void 
constr_func(int i,int index){
	char buf1[MAXLINE];
	
	memset(buf1,0,sizeof(buf1));
	sprintf(buf1,"[%s is connected]\r\n",list_c[index].nick);
	write(list_c[i].socket_num,buf1,strlen(buf1));

	sprintf(buf1,"[%s is connected]\r\n",list_c[i].nick);
	write(list_c[index].socket_num,buf1,strlen(buf1));
}
void
quit_func(int i){
	int j;
	char* token=NULL;
	char buf1[MAXLINE];

	memset(buf1,0,sizeof(buf1));
	printf("%s is leaved at %s\r\n",list_c[i].nick, list_c[i].ip);
	for(j=0; j<MAX_CLIENT;j++)
		if(j!=i && list_c[j].socket_num!=INVALID_SOCK){
			sprintf(buf1,"[%s is leaved]\r\n",list_c[i].nick);
			write(list_c[j].socket_num,buf1,strlen(buf1));
		}
}
void 
list_func(int i){
	int j,cnt=0;
	char buf1[MAXLINE];

	memset(buf1,0,sizeof(buf1));
	for(j=0; j<MAX_CLIENT;j++)
		if(list_c[j].socket_num!=INVALID_SOCK)cnt++;
	sprintf(buf1,"[the number of current user is %d]\r\n",cnt);
	write(list_c[i].socket_num,buf1,strlen(buf1));
	for(j=0; j<MAX_CLIENT;j++)
		if(list_c[j].socket_num!=INVALID_SOCK){
			sprintf(buf1,"[%s from %s:%d]\r\n",list_c[j].nick,list_c[j].ip,list_c[j].port);
			write(list_c[i].socket_num,buf1,strlen(buf1));
		}
}
int
smsg_func(char* chatData,int i){
	int j,smsg_sock;
	char* token=NULL;
	char buf1[MAXLINE];



	memset(buf1,0,sizeof(buf1));
	token=strtok(chatData," ");
	char * end;
	
	if(strcmp(token,smsg)){
		if((end=strtok(NULL,"\n"))==NULL)
			sprintf(buf1,"%s",token);
		else sprintf(buf1,"%s %s",token,end);
		sprintf(chatData,"[%s] %s\n",list_c[i].nick,buf1);
		return 1;
	}
	else{
		token=strtok(NULL," ");
		for(j=0;j<MAX_CLIENT;j++)
			if(!strcmp(list_c[j].nick,token))
				smsg_sock=list_c[j].socket_num;
		token=strtok(NULL,"\n");
		memset(buf1,0,sizeof(buf1));
		sprintf(buf1, "[smsg from %s] %s\r\n", list_c[i].nick, token);
		write(smsg_sock, buf1, strlen(buf1));
		return 0;
	}
}
	
main(int argc,char *argv[])
{
	int connfd,listenfd;
	struct sockaddr_in servaddr,cliaddr;
	int clilen;
	int maxfd=0;
	int i,j,n;
	fd_set rset;
	
	int index;
	
	char* token=NULL;
	char buf1[MAXLINE];
	char buf2[MAXLINE];
	char chatData[CHATDATA];

	

	
	if(argc<2){
		printf("usage: %s port_number\n",argv[0]);
		exit(-1);
	}
	
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[1]));

	if(bind(listenfd,(struct sockaddr *) &servaddr,sizeof(servaddr))==-1){
		printf("Can not Bind\n");
		return -1;
	}
	if(listen(listenfd,MAX_CLIENT)==-1){
		printf("listen Fail\n");
		return -1;
	}

	for(i=0;i<MAX_CLIENT;i++)
		list_c[i].socket_num=INVALID_SOCK;
	
	memset(buf1,0,sizeof(buf1));
	inet_ntop(AF_INET,&servaddr.sin_addr,buf1,sizeof(buf1));
	printf("[server address is %s : %d]\r\n", buf1, ntohs(servaddr.sin_port));

	for ( ; ; )
	{	
		maxfd=listenfd;

		FD_ZERO(&rset);
		FD_SET(listenfd,&rset);
		for(i=0;i<MAX_CLIENT;i++){
			if(list_c[i].socket_num!=INVALID_SOCK){
				FD_SET(list_c[i].socket_num,&rset);
				if(list_c[i].socket_num > maxfd) maxfd=list_c[i].socket_num;
			}
		}
		maxfd++;

		if(select(maxfd,&rset,(fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0){
			printf("Select error\n");
			exit(1);
		}

		if(FD_ISSET(listenfd,&rset)){
			clilen=sizeof(cliaddr);
			if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr, &clilen)) > 0) {
				memset(buf1,0,sizeof(buf1));
				memset(buf2,0,sizeof(buf2));
				read(connfd,buf1,sizeof(buf1));//read client's nickname
				inet_ntop(AF_INET,&cliaddr.sin_addr,buf2,sizeof(buf2));
				index=pushClient(connfd,buf1,buf2,ntohs(cliaddr.sin_port));//push socknum,nick,ip,port_num of client
				printf("%s is connected from %s\r\n",list_c[index].nick,list_c[index].ip );
				
				if(index<0){
					write(connfd,ERROR,strlen(ERROR));
					close(connfd);
				}else{
					write(connfd,greeting, strlen(greeting));
					for(i=0; i<MAX_CLIENT;i++)
						if(i!=index && list_c[i].socket_num!=INVALID_SOCK){
							constr_func(i,index);
						}
				}
			}
		}
		for(i=0; i<MAX_CLIENT;i++){
			if((list_c[i].socket_num != INVALID_SOCK) && FD_ISSET(list_c[i].socket_num,&rset)){
				memset(chatData,0,sizeof(chatData));
				if((n=read(list_c[i].socket_num,chatData, sizeof (chatData)))>0){
					if(!strcmp(chatData,quit)){//disconnect from the client "i"
						quit_func(i);
						popClient(list_c[i].socket_num);
						continue;
					}
					if(!strcmp(chatData,list)){//print the list of clients
						list_func(i);
						continue;
					}
					if(strstr(chatData,smsg)!=NULL){
						if(smsg_func(chatData,i)==0) continue;	
						else;
					}
					for(j=0;j<MAX_CLIENT;j++){//send chatting letters
						if(list_c[i].socket_num !=INVALID_SOCK)
							if(j!=i)
								write(list_c[j].socket_num,chatData,sizeof(chatData));
					}
						

				}
			}
		}
		
	}
}
	

		
