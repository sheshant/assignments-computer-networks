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
#include <stdlib.h>
#include <iostream>


#define BUFFER 1024

using namespace std;
int _BDMAFT = 2500;
int _TDMAFT = 5000;

void error(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void get_file( const char* ip_ser,int portno,const char * name)
{
    // here the job is simply to get ip and port and make connection to the server first

    // connection process starts
    int sockfd, n,file_size;
    struct sockaddr_in serv_addr;
  
    char buffer[BUFFER];
    bzero(buffer,BUFFER);
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("ERROR opening socket");

   
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    if (inet_aton(ip_ser, &serv_addr.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        
    }
    serv_addr.sin_port = htons(portno); 
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR connecting");
    }
    // now here the client has send the file name to the server 
    n = write(sockfd,name,strlen(name));
    if (n < 0) 
    {
        error("ERROR writing to socket");
    }
    // he will receive the size of the file

    int fdes = open(name,O_WRONLY | O_CREAT| O_TRUNC,0777);
    int num=0,d = 0,total= 0;
    // first the client will read the size of the file
    bzero(buffer,BUFFER);
    num = read(sockfd,buffer,BUFFER);
    file_size = atoi(buffer);
    cout<<file_size<<endl;
    // done now start download after giving conformation 
    do
    {
        bzero(buffer,BUFFER);
        num = read(sockfd,buffer,BUFFER);
        sleep(1);
        d = write(fdes,buffer,num);
        if(d>=0)
        {
            total += d;
            printf("%d",(total*100)/file_size );
            printf(" percent\n");
        }
        
    }while(num >0);
    printf("\n");

    close(fdes);
    close(sockfd);
    return ;
}

int main(int argc, char *argv[])
{
    char portn[100];
    string client_id,file_name;
    int file_size;
    // here client id has the IP and the port
    cout<<"Enter client id"<<endl;
    cin>>client_id;
    cout<<"Enter file name"<<endl;
    cin>>file_name;
    // now we need to get the ip and port
    string ip_no,po_no;
    istringstream iss(client_id);
    getline(iss,ip_no,':');
    getline(iss,po_no,'\0');
    int portno = atoi(po_no.c_str());
    // got it
    get_file(ip_no.c_str(),portno,file_name.c_str());

    return 0; 
}
