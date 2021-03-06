#include	"unp.h"
#include	"unp.h"

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
	char	recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	int n,maxfd;

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
{
	int				sockfd;
	void			sig_chld(int), sig_int(int), web_child(int);
	char			*bip,*bport,*myport,*tip;
	socklen_t		len;
	struct sockaddr_storage	ss;

	if (argc != 3)
		err_quit("usage: cl2 <hostname/IPaddress> <service/port#>");

	sockfd = Tcp_connect(argv[1], argv[2]);

	len = sizeof(ss);
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

	myport = register_to_server(sockfd);
	printf("My port %s\n",myport);


	tip = search_buddy(sockfd);
	if(tip != NULL){
		bip = strtok(tip,":");
		bport = strtok(NULL,":");
		printf("bip:%s bport:%s\n",bip,bport);

	}


	Close(sockfd);

	if(tip != NULL){
		sockfd = Tcp_connect(bip, bport);
		len = sizeof(ss);
		Getpeername(sockfd, (SA *)&ss, &len);
		printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));
			start_chating(sockfd);	/* process request */
	}

return 0;
}

void
sig_int(int signo)
{
	void	pr_cpu_time(void);

	pr_cpu_time();
	exit(0);
}
/* end sigint */

