#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>	// provide stat functionality for directory checking
#include <string.h>		// provide c string capabilities
#include <unistd.h>		// provide functionality for UNIX calls
#include <stdlib.h>		// malloc, calloc, free

using namespace std;

#define MAX_PATH 1000




int main() {
	char* buf = (char*) malloc(MAX_PATH * sizeof(char));
	char* dir_buf = (char*) malloc(MAX_PATH * sizeof(char));
	buf = getcwd(buf, MAX_PATH);
	strcpy(dir_buf, buf);
	dir_buf = strcat(dir_buf, "/files");
	cout << buf << endl;
	cout << dir_buf << endl;
}