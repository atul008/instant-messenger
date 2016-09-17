#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

enum max_childeren{
    MAX_CHILDEREN = 50
};

int say(int socket)
{
    char buf[513] = {0};
    fgets(buf, sizeof(buf), stdin);
    int result=send(socket, buf, strlen(buf),0);
    return result;
}

int main()
{
    int listener_d;
    struct sockaddr_in name;
    listener_d=socket(PF_INET,SOCK_STREAM,0);
    name.sin_family=PF_INET;
    name.sin_port=(in_port_t)htons(30000);
    name.sin_addr.s_addr=htonl(INADDR_ANY);

    int on = 1;
    if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
        perror("setsockopt()");
    }

    int c = bind(listener_d,(struct sockaddr *)&name,sizeof(name)); //Bind

    if(c== -1)
    {
        printf("\nCan't bind to socket\n");
    }

    if(listen(listener_d,10) == -1) // Listen
    {
        printf("\nCan't listen\n");
    }

    // Edited here
    int number_of_childeren = 0;
    int pipes[2] = {0};
    int child_pipe_write_ends[MAX_CHILDEREN] = {0};

    fd_set select_fds;
    FD_ZERO(&select_fds);

    puts("\nWait for connection\n");
    while(1)
    {
        struct sockaddr_storage client_addr;
        unsigned int address_size = sizeof(client_addr);

        // Edited here, to multiplex IO
        FD_SET(listener_d, &select_fds);
        FD_SET(STDIN_FILENO, &select_fds);
        int maxfd = listener_d + 1;

        int create_new_child = 0;
        int connect_d = -1; // moved here

        select(maxfd, &select_fds, NULL, NULL, NULL);

        if (FD_ISSET(listener_d, &select_fds)){
            connect_d = accept(listener_d, 
                                   (struct sockaddr*)&client_addr,&address_size); //Accept
            if(connect_d== -1)
                {
                    printf("\nCan't open secondary socket\n");
                    exit(EXIT_FAILURE);
                }

            create_new_child = 1;
        }

        char buf[512] ={0};
        char *endptr = NULL;
        if (FD_ISSET(STDIN_FILENO, &select_fds)){
            fgets(buf, sizeof(buf), stdin);
             long int child_num = strtol(buf, &endptr, 10);

             if (child_num > 0 && child_num <= number_of_childeren) {
                 write(child_pipe_write_ends[child_num - 1], endptr, strnlen(buf, sizeof(buf)) - (endptr - buf));
             }
             else {
                 printf("Skipping invalid input: %s\n", buf);
             }
        }

        if (create_new_child != 1)
            continue;

        number_of_childeren++; // Edited here

        int error = pipe(pipes);
        if (error != 0){
            //handle errors
            perror("pipe():");
            exit(EXIT_FAILURE);
        }

        child_pipe_write_ends[number_of_childeren - 1] = pipes[1];

        if(!fork())
        {

            error = dup2(pipes[0], STDIN_FILENO);
            if (error < 0){ // could also test != STDIN_FILENO but thats confusing
                //handle errors
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(pipes[0]);

            close(listener_d);
            char *msg = "welcome Sweetone\n";
            if(send(connect_d,msg,strlen(msg),0))
            {
                printf("send\n");
            }
            int k=0;
            while(k<5)
            {
                say(connect_d);
                ++k;
            }
            close(connect_d);
            exit(0);
        }
            close(connect_d);
            close(pipes[0]);
    }
    close(listener_d);
    return 0;
}
