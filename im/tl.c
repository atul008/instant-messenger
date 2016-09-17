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

	//printf("Waiting for server to ask buddy name:%\n");
	Fputs("Waiting for server prompt:%\n",stdout);
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
		Writen(sockfd,"nothanks\n", strlen("nothanks\n"));
	//}
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

int
main(int argc, char **argv)
{
	int				sockfd;
	void				sig_chld(int), sig_int(int), web_child(int);
	char			*bip,*bport,*myport,*tip,cmd[30],cfd[30];
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

printf("Listening begins\n");
	Close(sockfd);

	listenfd = Tcp_listen(NULL,myport, &addrlen);

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
				sscanf("./tc","%s",cmd);
				sprintf(cfd,"%d",connfd);
				strcat(cmd," ");
				strcat(cmd,cfd);
				printf("connection from %s\n",Sock_ntop((SA *)cliaddr, addrlen));
				if ( (childpid = Fork()) == 0) {
					Close(listenfd);
					if(execl("/usr/bin/xterm","xterm","-e","bash","-c",cmd,(char*)0) < 0){
						Fputs("execl error",stdout);
						Close(connfd);
					}
					
					exit(0);
				}
				Close(connfd);
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

