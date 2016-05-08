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

#define LEN 2048
#define ull unsigned long long

using namespace std;

//useful funtions
void error(const char *msg){
    perror(msg);
    exit(1);
}
unsigned long long oat_hash(const char *p, int len){
	
	unsigned long long h = 0;
    int i;

    for (i = 0; i < len; i++){
    	h += p[i];
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}
int to_char_array(char *op,string ip){
	bzero(op,sizeof(op));
	strcpy(op,ip.c_str());
	return strlen(op);
}


class Node {
public:
	string ip,port,ip_port;// ip,port,ip:port

	ull node_id;//hashed node id

	string scr,pdr;
	vector <string> myFile;
	map<string,string> myFileInfo;
	Node (){
		return;
	}
	
	Node(string ip_port){//constructor

		if (ip_port==""){
			this->ip="";
			this->port="";
			this->ip_port="";
			this->node_id=0;
			this->scr = "";
			this->pdr = "";
			return;
		}
		else {

			//ip:port,ip,port
			this->ip_port = ip_port;
			istringstream iss(ip_port);
			getline(iss,this->ip , ':');
			getline(iss, this->port, ':');
			//node_id
			char ip[LEN];
			to_char_array(ip,this->ip_port);
			this->node_id = oat_hash(ip,strlen(ip));
			this->scr = "";
			this->pdr = "";
			
			return;
		}
	}
	
	void set_ip_port(string ip_port){
		
			//ip:port,ip,port
			this->ip_port = ip_port;
			istringstream iss(ip_port);
			getline(iss,this->ip , ':');
			getline(iss, this->port, ':');
			//node_id
			char ip[LEN];
			to_char_array(ip,this->ip_port);
			this->node_id = oat_hash(ip,strlen(ip));
			this->scr = "";
			this->pdr = "";
		
			return;
	}
	void set_successor(string ip){
		this->scr = ip;
		return;
	}
	void set_predecessor(string ip){
		this->pdr = ip;
		return;
	}
	string successor() {
		return this->scr;
	}
 
	string predecessor()  {
		return this->pdr;
	}
	bool operator<(const Node& ip) const{
    	return this->node_id < ip.node_id;
	}
};

void __sendreq(string ip,int port,string msg){

	int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;

    
    char sendline[LEN];
    bzero(sendline,LEN);
   
    to_char_array(sendline,msg);

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(ip.c_str()); 
    servaddr.sin_port=htons(port);

        //sending msg
        sendline[strlen(sendline)] = 0;
        sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
   	    
    close(sockfd);
    return ;
}

string __recvinfo(int port){
	int sockfd,n;

	struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char mesg[LEN];
  
    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(port);
    int x = bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

  
   	len = sizeof(cliaddr);
    n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
    mesg[n] = 0;
    string op(mesg);
   
    close(sockfd);
    return op;
}

Node* map_file_to_node(ull file_id_x,Node *all_info,int n){
	vector<ull> poss;
	for (int i=0;i<n;i++){
		if(all_info[i].node_id >=file_id_x){
			poss.push_back(all_info[i].node_id);
		}
	}
	sort(poss.begin(),poss.end());
	if(poss.size()==0){
		return all_info;
	}

	for(int i=0;i<n;i++){
		if(poss[0]==all_info[i].node_id){
			return (all_info+i);
		}
	}
	return NULL;
}

void search_func(string req, Node node){
	istringstream iss(req);
	string fileName,op_str,q_ip_port;
	getline(iss,fileName,' ');
	getline(iss,fileName,' ');
	getline(iss,q_ip_port,' ');
	getline(iss,q_ip_port,' ');

	if(node.myFileInfo.find(fileName)!=node.myFileInfo.end()){
		string to_snd = "owner_of "+fileName+" is "+node.myFileInfo[fileName];
		istringstream qip(q_ip_port);
		string ip,port;
		getline(qip,ip,':');
		getline(qip,port,':');
		__sendreq(ip,atoi(port.c_str()),to_snd);
	}
	else{
		istringstream succ(node.scr);
		string ip,port;
		getline(succ,ip,':');
		getline(succ,port,':');
		__sendreq(ip,atoi(port.c_str()),req);
	}
	return ;
}

void download_func(string file,Node node, string ip,int port){
	int sockfd, n;
    struct sockaddr_in serv_addr;
  	char file_name[LEN];
  	bzero(file_name,LEN);
  	strcpy(file_name,file.c_str());
    char buffer[256];
    bzero(buffer,256);
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("ERROR opening socket");

   
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    char *ip_arr;
    ip_arr = (char *)malloc(100*sizeof(char));
    int m = to_char_array(ip_arr,ip);
    if (inet_aton(ip_arr, &serv_addr.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    serv_addr.sin_port = htons(port); 
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        error("ERROR connecting");
    }
    n = write(sockfd,file_name,strlen(file_name));
    if (n < 0) 
    {
        error("ERROR writing to socket");
    }
    char* ne;
    ne = (char *)malloc(100*sizeof(char));
    bzero(ne,sizeof(ne));
    ne[0] = 'n';ne[1] = 'e';ne[2] = 'w';ne[3] = ' ';ne[4] = '\0';
    strcat(ne,file_name);
    int fdes = open(ne,O_WRONLY | O_CREAT| O_TRUNC,0777);
    int num=0,d = 0;
    do
    {
        bzero(buffer,256);
        num = read(sockfd,buffer,256);
        d = write(fdes,buffer,num);
        
    }while(num >0);

    close(fdes);
    close(sockfd);
	return;
}

int main(int argc, char**argv){
	
	//initialization 
	Node my_node("");
	cout << "Feed the ip address:\n";
	string ip;
	cin >> ip;
	cout << "Feed the port:\n";
	string port;
	cin >> port;
	int my_port = atoi(port.c_str());
	//store in node
	string ip_port = ip+":"+port;
	my_node.set_ip_port(ip_port);

	cout << "How many files you want share:\n" ;
	int nf;
	cin >> nf;
	char buff[1024];
	bzero(buff,1024);
	sprintf(buff,"%d",nf);
	string nf_s(buff);
	string msg1 = ip+" "+port+" "+nf_s+" ";
	
	cout << "Feed the file names:\n";
	for (int i=0;i<nf;i++){
		string str;
		cin >> str;
		my_node.myFile.push_back(str);
		msg1 = msg1+str+" ";
	}
	
	__sendreq("127.0.0.1",9090,msg1);

	string msg3 = __recvinfo(my_port);
	
	
	istringstream iss_1(msg3);
	
	getline(iss_1,my_node.scr,' ');
	getline(iss_1,my_node.pdr,' ');
	string key,val;
	while(getline(iss_1,key,' ')){
		getline(iss_1,val,' ');
		my_node.myFileInfo[key] = val;
	}
	cout << "Give command...(search 'file') or (download 'file' from 'ip:port')" << endl;
	
	// topology ready now and child handle i/o and parent run udp server
	cin.ignore(100,'\n');
	pid_t pid;
	if((pid = fork()) == 0){
		string req;
		while(1){
			getline(cin,req);
			
			if (req.substr(0,6)=="search") {
				req = req + " for "+my_node.ip_port;
    			search_func(req,my_node);
			}
		
			else if(req.substr(0,8)=="download") {// "download batman.mp3 from 10.7.10:8080"
				istringstream she(req);//convert in stream
				string file,ip,dum; int port;
				getline(she,dum,' ');// feed download
				getline(she,file,' ');// feed batman.mp3
				getline(she,dum,' ');// feeed from
				getline(she,ip,':');//ip
				getline(she,dum,':');
				port = atoi(dum.c_str());
				
				download_func(file,my_node,ip,port);
			}
			else {
				cout << "invalid command" << endl;
			}
			
		}
		exit(0);
	}
	//parent handle infinite server and request processing
	else{
		// tcp server 
		if((pid = fork()) == 0){
			pid_t cpid;
    		int sockfd, newsockfd, portno;
    		socklen_t clilen; 
    		char buffer[256];
    		struct sockaddr_in serv_addr, cli_addr;
    		int n;
    		sockfd = socket(AF_INET, SOCK_STREAM, 0); // the domain name and stream
    		if (sockfd < 0) 
    		{
        		error("ERROR opening socket");
    		}
    		bzero((char *) &serv_addr, sizeof(serv_addr)); // why this is needed
    		portno = atoi(my_node.port.c_str());
	    	serv_addr.sin_family = AF_INET; // always this
    		serv_addr.sin_addr.s_addr = INADDR_ANY; // 32 bit IP address
    		serv_addr.sin_port = htons(portno); // these functions convert from your native byte order to network byte order
    
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
        		if((cpid = fork()) == 0)
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
        	    	    //printf("%d\n",fdes);
                		do
                		{
                    		bzero(buffer,256);
                    		num = read(fdes,buffer,256);
                    		d = write(newsockfd,buffer,num);
                    
                		}while(num > 0);
                		close(fdes);
            		}
            		exit(0);
        		}
       			close(newsockfd);
    		}	 
    		close(sockfd);
			exit(0);
		}
		else{
			while(1){
				string req = __recvinfo(my_port);
				if(req.find("owner_of")!=string::npos){
					cout << req << endl;
				}
				else if (req.find("search")!=string::npos){
					search_func(req,my_node);
				}
			}
			wait(NULL);
		}	
	}
	return 0;
}