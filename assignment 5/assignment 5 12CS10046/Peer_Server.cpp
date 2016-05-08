#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <arpa/inet.h>
#include <sys/select.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    pid_t pid;
	int sockfd, newsockfd, portno;
    socklen_t clilen; 
    // clilen stores the size of the address of the client. This is needed for the accept system call.
    char buffer[256]; // The server reads characters from the socket connection into this buffer.
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) 
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // the domain name and stream
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr)); // why this is needed
    portno = atoi(argv[1]); // we got the port through it
    serv_addr.sin_family = AF_INET; // always this
    serv_addr.sin_addr.s_addr = INADDR_ANY; // 32 bit IP address
    serv_addr.sin_port = htons(portno); // these functions convert from your native byte order to network byte order
    // and this attribute must be in network byte order
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) // very important
    {
        error("ERROR on binding");
    }
    listen(sockfd,5); // 5 means total munber of connections in waiting queue
    clilen = sizeof(cli_addr); // socket_len
    

    // where to right

    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }
        if((pid = fork()) == 0)
        {
            bzero(buffer,256);
            n = read(newsockfd,buffer,256); // first parameter is always file descriptor
            if (n < 0) 
            {
                error("ERROR reading from socket");
            }
            else
            {
                int fdes = open(buffer,O_RDONLY);
                int num=0,d=0;//,no_of_data=0,sum_n = 0,m = 0;
                printf("%d\n",fdes);
                do
                {
                    bzero(buffer,256);
                    num = read(fdes,buffer,256);
                    d = write(newsockfd,buffer,num);
                    // printf("num = %d   d = %d\n",num,d);
                }while(num > 0);
                close(fdes);
            }
            exit(0);
        }
       close(newsockfd);
    	
    }
    // where to read

    
    close(sockfd);
    return 0; 
     
}