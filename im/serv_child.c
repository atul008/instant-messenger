#include	"unp.h"

#define	MAXN	16384		/* max # bytes client can request */

void
serv_child(int sockfd)
{
	int			ntowrite;
	ssize_t		nread;
	char		line[MAXLINE], *result,bname[20],bip[30],*com;
	FILE *rp;
	rp = fopen("buddylist.txt","w+");
	
		if ( (nread = Readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */
		
		line[nread]=0;
		if( (sscanf(line,"%s",bname)) > 0 ){
			com = strcat("grep ",bname);
			com = strcat(*com," buddylist.txt > result");
			system(com);
			if( (fscanf(rp,"%s",bip)) == 0 ){
				Writen(sockfd, "n", strlen("n"));
			}else {
				result = strcat("y ",bip);
				Writen(sockfd, result, strlen(result));			
			}			

		} else {
			Writen(sockfd,"Error reading buddy name \n", strlen("Error reading buddy name \n"));
			return ;	
		}
	
}
