#include "myh.h"

void start_chating(int sockfd){
	char	recvline[MAXLINE + 1], sendline[MAXLINE + 1],status[10];
	int n,maxfd;
	socklen_t		len;
	struct sockaddr_storage	ss;
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(sockfd,&rset);
	FD_SET(STDIN_FILENO,&rset);

	Fputs("chat begins. Enter \"cntl + c \" to exit chat\n",stdout);
	do{
		FD_SET(sockfd,&rset);
		FD_SET(STDIN_FILENO,&rset);

		maxfd = max(sockfd,STDIN_FILENO)+1;
		Select(maxfd,&rset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&rset)){
			printf("****************RECIEVING*************************\n");
			if((n = Readline(sockfd, recvline, MAXLINE)) > 0) {
					recvline[n] = 0;	/* null terminate */
					Fputs(recvline, stdout);
					fflush(stdout);
		}else if(n==0){
				Fputs("---------------------------EXITING CHAT...BYE-------------------------\n", stdout);
								break;

			}
		}

		if(FD_ISSET(STDIN_FILENO,&rset)){
			printf("********************SEND*************************\n");
			if(Fgets(sendline,MAXLINE,stdin) != NULL){
				Write(sockfd, sendline, strlen(sendline));
				fflush(stdin);
			}else if(n==0){
				Fputs("---------------------------EXITING CHAT...BYE-------------------------\n", stdout);
								break;

			}
		}

	}while(1);

}

