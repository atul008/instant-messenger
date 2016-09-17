#include	"unp.h"
int register_to_server(int sockfd,char *uname){
	char	recvline[MAXLINE + 1];
	int n;
	//Prompt from server for username 
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	//sending username to server 
	if(Fgets(uname,20,stdin) != NULL){
		Write(sockfd, uname, strlen(uname));
	}
	//recieving confirmation of registration
	printf("Waiting for confirmation of registration...\n"); 
	if((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}	
}
int search_buddy(int sockfd,char * ip,char *bname,char *bip,char *bport){

	char	recvline[MAXLINE + 1],temp[10];
	int n;

	printf("Waiting for server prompt:%\n");
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
		Fputs(recvline, stdout);
	}
	sscanf(recvline,"%s%s%s",temp,ip,bname);
	if(!strcmp(temp,"yes")){
		//breaking full ip into ip and port number.		
		bip = strtok(ip,":");
		bport = strtok(NULL,":");
		printf("%s available.\nConnecting to %s with port %s ...\n",bname,bip,bport);
		return 1;
	} else {
		printf("%s not available\n",bname);
		return 0;
	}
}

int
main(int argc, char **argv)
{
	int				sockfd, n;
	char			recvline[MAXLINE + 1];
	char			*p,uname[20],bname[20],temp[10],ip[30],*bip,*bport;
	socklen_t		len;
	struct sockaddr_storage	ss;

	if (argc != 3)
		err_quit("usage: cl2 <hostname/IPaddress> <service/port#>");

	sockfd = Tcp_connect(argv[1], argv[2]);

	len = sizeof(ss);
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));
	register_to_server(sockfd,uname);
	search_buddy(sockfd,ip,bname,bip,bport);	
	
	
	exit(0);
}
