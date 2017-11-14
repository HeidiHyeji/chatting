#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

#define CHATDATA 1024

char quit[]="/quit\n";
char list[]="/list\n";
char smsg[]="/smsg";
char smsg_quit[]="/smsg_quit\n";

void
chatting(int sockfd, int maxfdp1, fd_set rset, char *argv[])
{
	char chatData[CHATDATA];
	char buf[CHATDATA];
	int n;

	while(1){
		FD_ZERO(&rset);
		FD_SET(0,&rset);
		FD_SET(sockfd, &rset);
	
		if(select(maxfdp1, &rset, (fd_set *)0, (fd_set *)0, (struct timeval *)0) <0) {
			printf("select error\n");
			exit(1);
		}

		if(FD_ISSET(sockfd,&rset)){
			memset(chatData, 0, sizeof(chatData));
			if((n=read(sockfd, chatData, sizeof(chatData))) >0){
				write(1, chatData, n);
			}
		}

		if(FD_ISSET(0, &rset)) {
			memset(buf, 0, sizeof(buf));
			if((n=read(0,buf,sizeof(buf)))>0){
				if(!strcmp(buf, quit)){
					write(sockfd, buf, strlen(buf));
					break;
				}
				if(!strcmp(buf,list)){
					write(sockfd, buf, strlen(buf));
					continue;
				}
				if(strstr(buf,smsg)!=NULL){
					write(sockfd, buf, strlen(buf));
					continue;
				}
				sprintf(chatData, "[%s] %s", argv[3], buf);
				write(sockfd, chatData, strlen(chatData));


				
			}
		}
	}
}
main(int argc,char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	int maxfdp1;
	fd_set rset;

	int len;
	char chatData[CHATDATA];
	char buf[CHATDATA];
	int n;
	char* token=NULL;

	if(argc<4){
		printf("usage:%s ip_address port_number nickname\n",argv[0]);
		exit(-1);
	}

	sockfd=socket(AF_INET, SOCK_STREAM,0);
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(atoi(argv[2]));

	
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))==-1){
		printf("Can not connect\n");
		return -1;
	}

	write(sockfd, argv[3], strlen(argv[3])); //send client's nickname
	maxfdp1=sockfd +1;
	
	chatting(sockfd, maxfdp1, rset, argv);

	close(sockfd);
	//exit(0);
}

