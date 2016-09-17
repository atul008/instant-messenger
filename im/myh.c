#include "myh.h"

char* register_to_server(int sockfd){
	char	recvline[MAXLINE + 1],*myip,*port;
	int n;
	//Prompt from server for username 
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	//sending username to server 
	if(Fgets(recvline,20,stdin) != NULL){
		Write(sockfd, recvline, strlen(recvline));
	}
	//recieving confirmation of registration
	printf("Waiting for confirmation of registration...\n"); 
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs("Registration confirm.\n", stdout);

	}
	myip = strtok(recvline,":");
	port = strtok(NULL,":");

return port;
}
char* search_buddy(int sockfd){

	char	recvline[MAXLINE + 1],temp[10],ip[MAXLINE+1],bname[30];
	int n;

	//printf("Waiting for server prompt:%\n");
	Fputs("Waiting for server prompt:%\n",stdout);
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	} 	
	//printf("Enter buddy's username to search: ");
	if(Fgets(bname,20,stdin) != NULL){	
		Write(sockfd, bname, strlen(bname));
	}
	//Reding result of buddy query
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		//Fputs(recvline, stdout);
	}
	sscanf(recvline,"%s%s%s",temp,ip,bname);
	if(!strcmp(temp,"yes")){

		printf("ip: %s\n",ip);
		return ip;
	} else {
		printf("Buddy not available.\n");
		return NULL;
	}
}

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

#endif
