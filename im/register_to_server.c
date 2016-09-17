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

