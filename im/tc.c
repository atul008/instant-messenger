#include	"unp.h"

void start_chating(int sockfd){
	char	recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	int n,maxfd;
	socklen_t		len;
	struct sockaddr_storage	ss;
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(sockfd,&rset);
	FD_SET(STDIN_FILENO,&rset);

	//Write(sockfd, "chat begins. Enter \"QUIT\" to exit chat", strlen("chat begins. Enter \"QUIT\" to exit chat"));
	Fputs("chat begins. Enter \"cntl + c \" to exit chat\n",stdout);
	do{
		FD_SET(sockfd,&rset);
		FD_SET(STDIN_FILENO,&rset);


		maxfd = max(sockfd,STDIN_FILENO)+1;
		Select(maxfd,&rset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&rset)){
			Fputs("******************************************************************* RECIEVED \n",stdout);
			if((n = Readline(sockfd, recvline, MAXLINE)) > 0) {
					recvline[n] = 0;	/* null terminate */
					Fputs(recvline, stdout);
					Fputs("****************************************************************************\n",stdout);
				//	fflush(stdout);
			}else if(n==0){
				Fputs("---------------------------EXITING CHAT...BYE-------------------------\n", stdout);
				Close(sockfd);
				return;

			}
		}

		if(FD_ISSET(STDIN_FILENO,&rset)){
			Fputs("************************************************************************ SEND \n",stdout);
			if(Fgets(sendline,MAXLINE,stdin) != NULL){
				Write(sockfd, sendline, strlen(sendline));
			//	Fputs("************\n",stdout);
				//fflush(stdin);
		}else if(n==0){
			Fputs("---------------------------EXITING CHAT...BYE-------------------------\n", stdout);
			Close(sockfd);
			return;


		}
		}
	}while(1);

}
int
main(int argc, char **argv)
{	//printf(argv)
	int connfd;
	connfd = atoi(argv[1]);
	start_chating(connfd);
	
return 0;
}

