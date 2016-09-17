/* Central server */
#include 	"unp.h"

int registration(FILE * fp,int connfd,struct sockaddr *cliaddr,int addrlen){
		char			line[MAXLINE],*clientip,com[70],email[100];
		ssize_t		nread;		
		int l;
				Writen(connfd, "Enter your <email id>:", strlen("Enter your <email id>:"));
				if ( (nread = Readline(connfd, line, MAXLINE)) == 0){
							Close(connfd);
							printf("Error reading emial id\n");
							return 0;
				}
				l = strlen(line);
				line[l-1]='\0';
				//Here,check for duplicate email ids.
				sscanf(line,"%s",email);
				sscanf("grep","%s",com);
				strcat(com," -vw ");
				strcat(com,line);
				strcat(com," buddylist.txt > temp.txt");
				system(com);
				system("cat temp.txt > buddylist.txt");

				clientip = Sock_ntop((SA *)cliaddr,addrlen);
				printf("uname: %s\n",line);
				line[l-1] = '\n';
				line[l] = '\0';
				printf("line:%s",line);
				fp = fopen("buddylist.txt","a");

				if(fprintf(fp,"%s\t%s",clientip,line) < 0 ){
					printf("Registration of %s failed :(\n", clientip);
					Writen(connfd, "Registration failed\n", strlen("Registration failed\n"));
					fflush(fp);
					return 0;
				}
				fflush(fp);

				printf("Registration of %s successfull :)\n", clientip);
				//Writen(connfd, "Registration succesfull\n", strlen("Registration succesfull\n"));
				Writen(connfd, clientip, strlen(clientip));

				fflush(fp);
				return 1;
}

void
serv_child(int sockfd)
{
	ssize_t		nread;
	char		line[MAXLINE],result[50],bname[20],bip[30],com[50];
	FILE *rp;
	rp = fopen("result","w+");
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(sockfd,&rset);
	FD_SET(STDIN_FILENO,&rset);

	printf("sending client prompt for buddy\n");
	Writen(sockfd,"Enter buddy name: ", strlen("Enter buddy name: "));
	printf("Buddy name prompt send.\n");
	if ( (nread = Readline(sockfd, line, MAXLINE)) == 0)
		return;		/* connection closed by other end */

	line[nread]=0;
	if( (sscanf(line,"%s",bname)) > 0 ){

		sscanf("grep","%s",com);
		strcat(com," -w ");
		strcat(com,bname);
		strcat(com," buddylist.txt > result");
		system(com);
		if( (Fgets(bip,30,rp)) == NULL ){
			Writen(sockfd, "no sorry", strlen("no sorry"));
		}else {
			sscanf("yes","%s",result);
			strcat(result," ");
			strcat(result,bip);
			Writen(sockfd, result, strlen(result));
		//	printf("Query result send to client\n");
		}

	} else {
		Writen(sockfd,"Error reading buddy name \n", strlen("Error reading buddy name \n"));
		Close(sockfd);
		return ;
	}

	
}
int 
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	void				sig_chld(int), sig_int(int), web_child(int);
	socklen_t			clilen, addrlen;
	struct sockaddr		*cliaddr;
	FILE* fp;
	fp = fopen("buddylist.txt","w+");

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv01 [ <host> ] <port#>");
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
				Close(listenfd);	/* close listening socket */
				if(registration(fp,connfd,cliaddr,addrlen) == 0)
					exit(0);
				Fclose(fp);
				serv_child(connfd);	/* process request */
				printf("Exited child.\n");
				Close(connfd);
				exit(0);
			}
		
			Close(connfd);			/* parent closes connected socket */

	}
	return 0;
}
/* end serv01 */

/* include sigint */
void
sig_int(int signo)
{
	void	pr_cpu_time(void);

	pr_cpu_time();
	exit(0);
}
/* end sigint */
