#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <map>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sys/wait.h>
#include <climits>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

using namespace std;
#define BUFFER 1024
int _BDMAFT = 2500;

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char**argv)
{
	pid_t pid;
	time_t start,end;
    int sockfd, newsockfd, portno;
    socklen_t clilen; 
    // clilen stores the size of the address of the client. This is needed for the accept system call.
    char buffer[BUFFER]; // The server reads characters from the socket connection into this buffer.
    struct sockaddr_in serv_addr, cli_addr;
    char* file_name;
    int file_size;
    file_name = (char *)malloc(100*sizeof(char));
    int n;
   
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // the domain name and stream
    if (sockfd < 0) 
    {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr)); // why this is needed
    portno = _BDMAFT;
    serv_addr.sin_family = AF_INET; // always this
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 32 bit IP address
    serv_addr.sin_port = htons(portno); // these functions convert from your native byte order to network byte order
    // and this attribute must be in network byte order
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) // very important
    {
        error("ERROR on binding");
    }
    listen(sockfd,5); // 5 means total munber of connections in waiting queue
    clilen = sizeof(cli_addr); // socket_len

    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }
        if((pid = fork()) == 0)
        {
            bzero(buffer,BUFFER);
            n = read(newsockfd,buffer,BUFFER); // first parameter is always file descriptor
            // it is first going to read file name to be send 
            if (n < 0) 
            {
                error("ERROR reading from socket");
            }
            else
            {
            	cout<<buffer<<endl;// print the file name
                strcpy(file_name,buffer);
                // now we need to get the file size using stat
                struct stat st;
                if(stat(file_name, &st) != 0) 
                {
                    perror("stat error");
                    exit(EXIT_FAILURE);
                }
                file_size = st.st_size; 
            	string s = "md5sum ";
            	s = s + string(file_name);
            	printf("%s\n",s.c_str() );
            	system(s.c_str());
                // open file in read
                int fdes = open(file_name,O_RDONLY);
                int num=0,d=0;
                // send file size to client
                bzero(buffer,BUFFER);
                sprintf(buffer,"%d\n",file_size);
                d = write(newsockfd,buffer,BUFFER);
                // file size send 
                start = time(&start);
                cout<<(int)start<<endl;
                do
                {
                    bzero(buffer,BUFFER);
                    num = read(fdes,buffer,BUFFER);
                    d = write(newsockfd,buffer,num);
                    
                }while(num > 0);
                close(fdes);
                end = time(&end);
                cout<<(int)end<<endl;
                cout<<"Total time = "<<(int)end-(int)start<<endl;
            }
            exit(0);
        }
        close(newsockfd);   
    }
    close(sockfd);
	return 0; 
}