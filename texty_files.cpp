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
#define ALL_USERS_FILE "all_users.txt"	// corresponds to file with every user's info and index number
#define MAX_INDEX 1000000	// maximum number of user indexes that can be used at creation of files

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

// file_exists checks whether the filename passed to it 
bool file_exists(const char* name)
{
	ifstream fh(name);
	if (fh.good()) {
		fh.close();
		return true;
	} else {
		fh.close();
		return false;
	}
}

// Create 3 text files in the passed in directory: followees, followers, texts
void create_user_files(const char* dir)
{
	chdir(dir);
	ofstream f1.open("followees.txt"); f1.close();
	ofstream f2.open("followers.txt"); f2.close();
	ofstream f3.open("texts.txt"); f3.close();
}

// A new user needs to be created
// Each user has a directory corresponding to their index number
// Each user's directory has 3 text files: followees, followers, texts
void create_user_dir(const char* user_index, const char* dir)
{
	chdir(dir);
	status = mkdir(user_index, S_IRWXU|S_IRWXG|S_IRWXO);
	if (status == -1) {
		// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED
		// mkdir was not successful
		cout << endl << "ERROR: mkdir could not create this new user's directory" << endl;
		// exit(1); ??????
		// return -1; ????
	}
	chmod(user_index, S_IRWXU|S_IRWXG|S_IRWXO); // give everyone RWX permissions
	char* dir_buf = (char*) malloc(MAX_PATH * sizeof(char));
	strcpy(dir_buf, dir);
	dir_buf = strcat(dir_buf, "/");
	dir_buf = strcat(dir_buf, user_index);
	create_user_files(dir_buf);
	chdir(dir);
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
			// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED
			// mkdir was not successful
			cout << endl << "ERROR: mkdir could not create files directory" << endl;
			// exit(1); ?????
			// return -1; ???
		}
		chmod(dir_buf, S_IRWXU|S_IRWXG|S_IRWXO); // give everyone RWX permissions
	}

	chdir(dir_buf); // access data inside USER_DIR directory, so change to that directory
	// Check if username or email are taken, if not taken {add this new user's info}
	const char* file_path = (char*) malloc(MAX_PATH * sizeof(char));
	stcpy(file_path, dir_buf);
	file_path = strcat(file_path, ALL_USERS_FILE);
	/*
	Structure of all_users.txt:
	N
	username,email,index_i,password,firstname,lastname
	username,email,index_j,password,firstname,lastname
	username,email,index_k,password,firstname,lastname
	... continues on for however many users are in the file
	Where N is index of the next user (this can go to infinity since numbers are not reused after a user deactivates their account)
	Each line contains strings representing user info separated by commas and ending with the '\n' character
	*/

	if (!file_exists(file_path)) {
		// If the file doesn't exist, create it
		ofstream fh;
		fh.open(file_path);
		// 1st user created will have an index of 1, which changes N to 2 for the next user to register
		fh << "2" << '\n';
		fh << un << "," << email << "," << "1" << "," << pw << "," << fn << "," << ln << '\n';
		// Create the new user's directory and followees.txt, followers.txt, texts.txt files
		create_user_dir("1", dir_buf);
		fh.close();
	} else {
		ifstream fh;
		fh.open(file_path);
		char* index = (char*) malloc(MAX_INDEX * sizeof(char)); // MAX_INDEX originally set to 1000000 meaning 999999 user indexes could be handled at the creation
		fh.getline(index, MAX_INDEX - 1, '\n'); // index will get the first line in the file which contains a number followed by the EOL character

		fh.close();
	}


	chdir(buf); // change the CWD back to its initial position at the beginning of the function

	free(buf);
	free(dir_buf);
	free(file_path);
}


int main() {

}