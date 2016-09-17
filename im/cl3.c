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
void start_chating_l(int sockfd){
	char	recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	int n;
	//Fputs("chat begins. Enter \"QUIT\" to exit chat\n",stdout);
	do{

			if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
				recvline[n] = 0;	/* null terminate */
				Fputs(recvline, stdout);

				if(Fgets(sendline,MAXLINE,stdin) != NULL){
					Write(sockfd, sendline, strlen(sendline));
				}

			}
	}while(!strcmp(recvline,"QUIT") || !strcmp(sendline,"QUIT") );

}
void start_chating_c(int sockfd){
	char	recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	int n;
	socklen_t		len;
	struct sockaddr_storage	ss;
	Write(sockfd, "chat begins. Enter \"QUIT\" to exit chat", strlen("chat begins. Enter \"QUIT\" to exit chat"));
	Fputs("chat begins. Enter \"QUIT\" to exit chat\n",stdout);
	do{
									
			if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
				recvline[n] = 0;	/* null terminate */
				Fputs(recvline, stdout);
									
				if(Fgets(sendline,MAXLINE,stdin) != NULL){	
					Write(sockfd, sendline, strlen(sendline));
				}
				
			}
	}while(!strcmp(recvline,"QUIT") || !strcmp(sendline,"QUIT") );

}

int
main(int argc, char **argv)
{
	int				sockfd,sockfd2,n;
	char			recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	void				sig_chld(int), sig_int(int), web_child(int);
	char			*p,uname[20],bname[20],temp[10],ip[30],*bip,*bport,*myport,*tip;
	socklen_t		len;
	struct sockaddr_storage	ss;
	
	int listenfd,connfd ;
	pid_t			 childpid;
	socklen_t		 clilen, addrlen;
	struct sockaddr	 *cliaddr;


	if (argc != 3)
		err_quit("usage: cl2 <hostname/IPaddress> <service/port#>");

	sockfd = Tcp_connect(argv[1], argv[2]);

	len = sizeof(ss);
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

	myport = register_to_server(sockfd);
	printf("My port %s\n",myport);


	tip = search_buddy(sockfd);
	printf("tip: %s\n",tip);
	if(tip != NULL){
		bip = strtok(tip,":");
		bport = strtok(NULL,":");
	}
	printf("bip:%s bport:%s\n",bip,bport);


	Close(sockfd);

	listenfd = Tcp_listen(NULL,myport, &addrlen);

	if(tip != NULL){
		sockfd = Tcp_connect(bip, bport);
		len = sizeof(ss);
		Getpeername(sockfd, (SA *)&ss, &len);
		printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));
		if ( (childpid = Fork()) == 0) {	/* child process */

			Close(listenfd);
			if ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
					recvline[n] = 0;	/* null terminate */
					Fputs(recvline, stdout);
			}
			start_chating_c(connfd);	/* process request */

			exit(0);
		}
		Close(sockfd);

	}
	cliaddr = Malloc(addrlen);
	Signal(SIGCHLD, sig_chld);
	Signal(SIGINT, sig_int);

	for ( ; ; ) {
				Fputs("Listening ....\n",stdout);
				clilen = addrlen;
				if ( (connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
					if (errno == EINTR)
						continue;		/* back to for() */
					else
						err_sys("accept error");
				}

				printf("connection from %s\n",Sock_ntop((SA *)cliaddr, addrlen));

				if ( (childpid = Fork()) == 0) {	/* child process */
					Close(listenfd);
					start_chating_l(connfd);	/* process request */
					exit(0);
				}
				Close(connfd);			/* parent closes connected socket */
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

