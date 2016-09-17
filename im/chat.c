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

	Fputs("Waiting for server prompt:%\n",stdout);
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	//printf("Enter buddy's username to search: ");
	if(Fgets(bname,20,stdin) != NULL){
		Write(sockfd, bname, strlen(bname));
		printf("your buddy name: %s",bname);
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

int
main(int argc, char **argv)
{
	int				sockfd,sockfd2,n,status,maxfd;
	char			recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	void				sig_chld(int), sig_int(int), web_child(int);
	char			*p,uname[20],bname[20],temp[10],ip[30],*bip,*bport,*myport,*tip,cmd[30],cfd[30];
	socklen_t		len,len1;
	struct sockaddr_storage	ss,ss1;
	
	int listenfd,connfd ;
	pid_t			 childpid;
	socklen_t		 clilen, addrlen;
	struct sockaddr	 *cliaddr;

	fd_set rset;
	FD_ZERO(&rset);

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
		printf("bip:%s bport:%s\n",bip,bport);

	}
	listenfd = Tcp_listen(NULL,myport+1, &addrlen);

	cliaddr = Malloc(addrlen);
	Signal(SIGCHLD, sig_chld);

	FD_SET(listenfd,&rset);
	//FD_SET(listenfd+1,&rset);
	FD_SET(STDIN_FILENO,&rset);

	for ( ; ; ) {
		Fputs("Enter buddy name\n",stdout);
		maxfd = max(listenfd,STDIN_FILENO)+1;
		Select(maxfd,&rset,NULL,NULL,NULL);

		if(FD_ISSET(listenfd,&rset)){
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

			if(FD_ISSET(STDIN_FILENO,&rset)){

				tip = search_buddy(sockfd);
				if(tip!=NULL){

					printf("tip: %s\n",tip);
					if(tip != NULL){
						bip = strtok(tip,":");
						bport = strtok(NULL,":");
						printf("bip:%s bport:%s\n",bip,bport);
						sockfd2 = Tcp_connect(bip, bport+1);
						len = sizeof(ss1);
						Getpeername(sockfd2, (SA *)&ss1, &len1);
						printf("connected to %s\n", Sock_ntop_host((SA *)&ss1, len1));
						sscanf("./tc","%s",cmd);
						sprintf(cfd,"%d",sockfd2);
						strcat(cmd," ");
						strcat(cmd,cfd);

						if ( (childpid = Fork()) == 0) {
							Close(listenfd);

							if(execl("/usr/bin/xterm","xterm","-e","bash","-c",cmd,(char*)0) < 0){
								Fputs("execl error",stdout);
								Close(sockfd2);
							}

							exit(0);
						}
						Close(sockfd2);
					}
				}
			}
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

