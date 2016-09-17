#include "myh.h"
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

