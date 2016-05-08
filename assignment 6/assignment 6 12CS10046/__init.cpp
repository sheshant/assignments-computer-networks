#include <iostream>
#include <string>
#include <iostream>
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


int main(int argc, char**argv){
	//initialization 
	cout << "How many nodes:" << endl;
	int n;
	cin >> n;
	Node all_info[n];
	
	for(int i=0;i<n;i++){

		string msg = __recvinfo(9090);
		
		
		string ip,port,nf_s;
		istringstream iss(msg);
		getline(iss,ip,' ');
		getline(iss,port,' ');
		getline(iss,nf_s,' ');
		string ip_port = ip+":"+port;
		all_info[i].set_ip_port(ip_port);

		int nf;
		sscanf(nf_s.c_str(),"%d",&nf);

		for(int j=0;j<nf;j++){
			string file;
			getline(iss,file,' ');
			all_info[i].myFile.push_back(file);
		}
	}

	//sort
	sort(all_info,all_info+n);
	//set pre and succ
	for (int i=0;i<n;i++){
		all_info[i].set_successor(all_info[(i+1)%n].ip_port);
		all_info[i].set_predecessor(all_info[(i-1+n)%n].ip_port);	
	}
	//hash files and update myFileInfo
	for(int i=0;i<n;i++){
		for(int j=0;j<all_info[i].myFile.size();j++){
			char ip_buf[LEN];
			bzero(ip_buf,LEN);
			strcpy(ip_buf,all_info[i].myFile[j].c_str());
			Node *dest = map_file_to_node(oat_hash(ip_buf,strlen(ip_buf)),all_info,n);
			if(dest==NULL){
				error("Hashing fails!");
			}
			dest->myFileInfo[all_info[i].myFile[j]] = all_info[i].ip_port;
		}
	}
	//send pre,succ,myFileInfo
	for(int i=0;i<n;i++){
		string msg = "";
		for(map<string,string>::iterator it = all_info[i].myFileInfo.begin();it!=all_info[i].myFileInfo.end();++it){
			msg = msg + it->first +" " + it->second + " ";
		}
		string msg0 = all_info[i].scr + " " + all_info[i].pdr+" "+msg;

		__sendreq(all_info[i].ip,atoi(all_info[i].port.c_str()),msg0);
		
	}

	return 0;
}