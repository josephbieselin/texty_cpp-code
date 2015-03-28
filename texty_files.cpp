/*
	Texty Web App
	C++ File Handling
*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>	// provide stat functionality for directory checking
#include <string.h>		// provide c string capabilities
#include <unistd.h>		// provide functionality for UNIX calls
#include <stdlib.h>		// malloc, calloc, free

using namespace std;

#define MAX_PATH 1000		// maximum file path is probably not more than 1000 chars
#define USER_DIR "/files"	// directory (relative to CWD) where data on all users for texty will be stored

bool is_dir(const char* path)
{
	struct stat buf;
	int status;
	status = stat(path, &buf);
	if (status == -1) {
		// If stat does not return 0, there was an error
		cout << endl << "ERROR: is_dir could not check path" << endl;
		exit(1);
	}

	if (buf->st_mode & S_IFDIR == 0)
		// Directory was not created -- creating "files" directory now
		return false;

	return true; // no errors and path was determined to be a directory
}

// register takes in 5 strings: first name, last name, email, username, password
// register stores those strings in a CSV line in all_users.txt in the directory files
void register(string& fn, string& ln, string& email, string& un, string& pw)
{
	// buf = current working directory; dir_buf = "CWD" + "USER_DIR" (USER_DIR is directory with all users of texty data)
	char* buf = (char*) malloc(MAX_PATH * sizeof(char));
	char* dir_buf = (char*) malloc(MAX_PATH * sizeof(char));
	buf = getcwd(buf, MAX_PATH);
	strcpy(dir_buf, buf);
	dir_buf = strcat(dir_buf, USER_DIR);

	if (!is_dir(dir_buf)) {
		// If the files directory doesn't exist, create it with RWX permissions for everyone
		int status;
		status = mkdir(dir_buf, S_IRWXU|S_IRWXG|S_IRWXO);
		if (status == -1) {
			// mkdir was not successful
			cout << endl << "ERROR: mkdir could not create directory" << endl;
			exit(1);
		}
		chmod(dir_buf, S_IRWXU|S_IRWXG|S_IRWXO); // give everyone RWX permissions
	}

	chdir(dir_buf); // access data inside USER_DIR directory, so change to that directory

	chdir(buf); // change the CWD back to its initial position at the beginning of the function
}




int main() {

}