#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void file_transfer(int portno)
{
	char* name;
	name = (char *)malloc(100*sizeof(char));
	printf("Enter file name\n");
	scanf("%s",name);
	printf("%s\n", name);

	int sockfd, n;
    struct sockaddr_in serv_addr; 
    char buffer[256];
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // 
    if (sockfd < 0) 
        error("ERROR opening socket");

    // for local host
    // server = gethostbyname("127.0.0.1");
    //
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; // always
    // this function copy memeory regions of arbitrary length

    // bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    if (inet_aton("127.0.0.1", &serv_addr.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    serv_addr.sin_port = htons(portno); // we know
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR connecting");
    }
    n = write(sockfd,name,strlen(name));
    if (n < 0) 
    {
        error("ERROR writing to socket");
    }
    char* ne;
    ne = (char *)malloc(100*sizeof(char));
    ne[0] = 'n';ne[1] = 'e';ne[2] = 'w';ne[3] = ' ';ne[4] = '\0';
    strcat(ne,name);
    int fdes = open(ne,O_WRONLY | O_CREAT| O_TRUNC);
    int num=0,d = 0;
    do
    {
        bzero(buffer,256);
        num = read(sockfd,buffer,256);
        d = write(fdes,buffer,num);
        printf("is it here %d  %d\n",d,num);
        // if(num < 256)
        // {
        // 	break;
        // }
    }while(num >0);
    close(fdes);
    close(sockfd);
    printf("done\n");
}


int main(int argc, char *argv[])
{
	int portno = atoi(argv[1]); // we got the port through it
	file_transfer(portno);
    // char c;
    // while(scanf("%c",&c)!=EOF){
    //     switch(c){
    //         case '\n':
    //         //
    //         break;
    //         case 
    //     }
    // }
    return 0; 
     
}