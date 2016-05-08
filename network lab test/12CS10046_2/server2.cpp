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
#include <sys/select.h>
#define BUFFER 1024

using namespace std;

int _TDMAFT = 5000;

int main(int argc, char**argv)
{
	struct timeval tv;
	tv.tv_sec = 4;
	tv.tv_usec = 500000;
	fd_set master; // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    struct sockaddr_in myaddr; // server address
    struct sockaddr_in remoteaddr; // client address
    int fdmax; // maximum file descriptor number
    int listener; // listening socket descriptor
    int newsockfd; // newly accept()ed socket descriptor
    char buffer[BUFFER]; // bufferfer for client data
    int nbytes;
    int yes=1; // for setsockopt() SO_REUSEADDR, below
    socklen_t addrlen;
    int i, j;
    FD_ZERO(&master); // clear the master and temp sets
    FD_ZERO(&read_fds);

    char* file_name;
    int file_size;
    file_name = (char *)malloc(100*sizeof(char));
    int n;
    time_t start,end;

    // get the listener
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // lose the pesky "address already in use" error message
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // bind
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(_TDMAFT);
    memset(&(myaddr.sin_zero), '\0', 8);
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) 
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(listener, 10) == -1) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // add the listener to the master set
    FD_SET(listener, &master);
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it’s this one
    // main loop
    while(1) 
    {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1) 
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) 
        {
            if (FD_ISSET(i, &read_fds)) 
            { 
                // we got one!!
                if (i == listener) 
                {
                    // handle new connections
                    addrlen = sizeof(remoteaddr);
                    if ((newsockfd = accept(listener, (struct sockaddr *)&remoteaddr,&addrlen)) == -1) 
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    } 
                    else 
                    {
                        FD_SET(newsockfd, &master); // add to master set
                        if (newsockfd > fdmax) 
                        { 
                            // keep track of the maximum
                            fdmax = newsockfd;
                        }
                        printf("selectserver: new connection from %s on socket %d\n", inet_ntoa(remoteaddr.sin_addr), newsockfd);
                    }
                } 
                else 
                {
                    // handle data from a client
                    bzero(buffer,BUFFER);
                    if ((nbytes = read(i, buffer, BUFFER)) <= 0) 
                    {
                        // got error or connection closed by client
                        if (nbytes == 0) 
                        {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } 
                        else
                        {
                            perror("read");
                            exit(EXIT_FAILURE);
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } 
                    else 
                    {
                        // we got some data from a client

                        cout<<buffer<<" buffer size is = "<<sizeof(buffer)<<endl;// print the file name
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

                } // it’s SO UGLY!
            }
        }
    }
    return 0;
}