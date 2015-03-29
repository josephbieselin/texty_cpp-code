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
#define MAX_INDEX_BYTES 10		// maximum number of user indexes that can be used at creation of files: 10 bytes = 999999999 possible indexes for a cstring
#define MAX_USER_INFO_BYTES 118	// maximum number of bytes each line in all_users.txt will be for each user including: user's data, commas, and '\n' character
#define UN_BYTES 17			// maximum number of characters in username based on value limited in PHP file
#define EMAIL_BYTES 41		// maximum number of characters in email based on value limited in PHP file
#define PW_BYTES 17			// maximum number of characters in password based on value limited in PHP file
#define FN_BYTES 21			// maximum number of characters in first name based on value limited in PHP file
#define LN_BYTES 21			// maximum number of characters in last name based on value limited in PHP file
#define GARBAGE_BYTES 70	// length of bytes to hold characters after username and email fields in file handling


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
	fstream fh(name);
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
	fstream f1.open("followees.txt"); f1.close();
	fstream f2.open("followers.txt"); f2.close();
	fstream f3.open("texts.txt"); f3.close();
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

bool user_exists(fstream& fh, string& un, string& email)
{
	// test_info is one line from the all_users.txt file
	// test_info = un,email,index,pw,fn,ln
	char* test_un = (char*) malloc(UN_BYTES * sizeof(char));
	char* test_email = (char*) malloc(EMAIL_BYTES * sizeof(char));
	char* garbage = (char*) malloc(GARBAGE_BYTES *sizeof(char));

	while (!fh.eof()) {
		fh.get(test_un, UN_BYTES, ','); 		// comma separated values, so ',' is the delim parameter
		if (strcmp(test_un, un.c_str()) == 0)
			return true; // the passed in username is equal to a username already created

		// ',' is the next character in the stream, so just get that
		fh.get(); //garbage, 2);

		// gets up to EMAIL_BYTES or until ',' is reached --> if ',' reached it is the next character that will be extracted from the stream
		fh.get(test_email, EMAIL_BYTES, ','); 	// comma separated values, so ',' is the delim parameter
		if (strcmp(test_email, email.c_str()) == 0)
			return true; // the passed in email is equal to an email already created

		// get the rest of the line until '\n' is reached
		fh.getline(garbage, GARBAGE_BYTES);
	}
	return false; // the username and email passed in were not found
}

// register takes in 5 strings: first name, last name, email, username, password
// register stores those strings in a CSV line in all_users.txt in the directory files
void register(string& un, string& email, string& pw, string& fn, string& ln)
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
			return;
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
		fstream fh;
		fh.open(file_path);
		if (!fh.is_open()) {
			// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED WITH FILE OPENING
			cout << endl << "ERROR: could not open all_users.txt" << endl;
		} else {
			// 1st user created will have an index of 1, which changes N to 2 for the next user to register
			fh << "2" << '\n';
			fh << un << "," << email << "," << "1" << "," << pw << "," << fn << "," << ln << '\n';
			// Create the new user's directory and followees.txt, followers.txt, texts.txt files
			create_user_dir("1", dir_buf);
			fh.close();
		}
	} else { // file does exist
		fstream fh;
		fh.open(file_path);
		if (!fh.is_open()) {
			// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED WITH FILE OPENING
			cout << endl << "ERROR: could not open all_users.txt" << endl;
			return;
		} else { // file is opened
			char* index = (char*) malloc(MAX_INDEX_BYTES * sizeof(char)); // MAX_INDEX originally set to 1000000 meaning 999999 user indexes could be handled at the creation
			fh.getline(index, MAX_INDEX_BYTES - 1, '\n'); // index will get the first line in the file which contains a number followed by the EOL character

			if (user_exists(fh, un, email)) {
				// IMPLEMENT WAY TO PASS BACK ERROR TO NETWORK THAT USER EXISTS ALREADY
				cout << endl << "ERROR: username or email already exist" << endl;
				return;
			} else { // fh got to EOF in user_exists so user does not exist
				// input the user data into the file stream with comma separation and an EOL character
				cin >> un >> "," >> email >> "," >> index >> "," >> pw >> "," >> "fn" >> "," >> "ln" >> '\n';
			}

			fh.close();
			free(index);
		}
	}



	chdir(buf); // change the CWD back to its initial position at the beginning of the function

	free(buf);
	free(dir_buf);
	free(file_path);
}


int main() {

}