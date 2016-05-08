#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <dirent.h> 
#define MAX_SIZE 100

using namespace std;

char* txt = (char *)(".txt");
char* cpp = (char *)(".cpp");
char* py = (char *)(".py");
char* c_prog = (char *)(".c");

void list_of_files(map <string,string> &table)
{
	DIR           *d;
	struct dirent *dir;
	struct stat buffer;
	char * aa;
	char bb[MAX_SIZE];
 	aa = getcwd(bb,MAX_SIZE);
	d = opendir(aa);
	char* file_type;
	file_type = (char *)malloc(100 * sizeof(char));
	int i,j;
   	if (d)
   	{
   		int c,len;
   		while ((dir = readdir(d)) != NULL)
   		{
   			c = dir->d_type;
 		  	if(dir->d_name[0] != '.')
 		  	{
 		  		if(c != 4)
 		  		{
 		  			len = strlen(dir->d_name);
 		  			i = len-1;
 		  			while(dir->d_name[i]!= '.')
 		  			{
 		  				i--;
 		  			}
 		  			j = 0;
 		  			while(i<len)
 		  			{
 		  				file_type[j]= dir->d_name[i];
 		  				j++;
 		  				i++;
 		  			}
 		  			file_type[j] = '\0';
 		  			if(strcmp(file_type,txt) == 0 || strcmp(file_type,cpp) == 0 || strcmp(file_type,py) == 0 || strcmp(file_type,c_prog) == 0)
 		  			{
 		  				table.insert(pair<string,string>(,));
 		  			}

 		  		}
 		  	}
   		}
   	}
}
int main(int argc, char *argv[])
{
	map <string,string> table = map <string,string>();
	return 0;
}