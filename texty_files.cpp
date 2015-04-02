/*
	Texty Web App
	Twitter Clone

	Programmer:
	Joseph Bieselin

	Networking code interfaces PHP on user machine with a server running C++ code
	"Database" emulated with C++ file handling

	
	Structure of all_users.txt:
	N,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'\n'
	username,email,index_i,password,firstname,lastname,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'\n'
	username,email,index_j,password,firstname,lastname,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'\n'
	username,email,index_k,password,firstname,lastname,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'\n'

	... continues on for however many users are in the file;
	Where N is index of the next user (this can go to infinity since numbers are not reused after a user deactivates their account);
	We assume for now that 10 bytes corresponding to the string "999999999" will suffice for 99999999 possible unique user indexes;
	Each line contains strings representing user info separated by commas;
	Each line is 118 bytes where commas are put at the end of each line before the newline character so lines can be overwritten after created


	Structure of followers.txt & followees.txt:
	index_i,username,,,,,,,,,,,,,,,,,,,,,,,,,,'\n'
	index_j,username,,,,,,,,,,,,,,,,,,,,,,,,,,'\n'

	... continues on for however many users are in the file;
	The user that the person is followed by (follower) or the that is following (followee) has their index and username on a line;
	Each line is 28 bytes where commas are put at the end of each line before the newline character so lines can be overwritten after created


	Structure of texts.txt:
	This is a samaple texty from some user'\n'
	This is a second sample from some user'\n'

	The user can input almost any character they want, excluding new lines;
	New lines delimit the end of one texty from the next;
	A texty cannot be deleted... ever, unless the account is deactivated
*/


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>	// provide stat functionality for directory checking
#include <string.h>		// provide c string capabilities
#include <unistd.h>		// provide functionality for UNIX calls
#include <stdlib.h>		// malloc, calloc, free
#include <typeinfo>

using namespace std;

/* ---------------------------------- CONSTANTS ----------------------------------------------*/
#define MAX_PATH 1000		// maximum file path is probably not more than 1000 chars
#define USER_DIR "/files"	// directory (relative to CWD) where data on all users for texty will be stored
#define ALL_USERS_FILE "/all_users.txt"	// corresponds to file with every user's info and index number
#define USER_DATA_FILENAME "all_users.txt"
#define MAX_INDEX_BYTES 10		// maximum number of user indexes that can be used at creation of files: 10 bytes = 999999999 possible indexes for a cstring
#define MAX_USER_INFO_BYTES 118	// maximum number of bytes each line in all_users.txt will be for each user including: user's data, commas, and '\n' character
#define UN_BYTES 17			// maximum number of characters in username based on value limited in PHP file
#define EMAIL_BYTES 41		// maximum number of characters in email based on value limited in PHP file
#define PW_BYTES 17			// maximum number of characters in password based on value limited in PHP file
#define FN_BYTES 21			// maximum number of characters in first name based on value limited in PHP file
#define LN_BYTES 21			// maximum number of characters in last name based on value limited in PHP file
#define GARBAGE_BYTES 70	// length of bytes to hold characters after username and email fields in file handling
#define CURRENT_DIR "~/Dropbox/Coding/PDC/texty_cpp/"	// directory where C++ files and user file directory is stored
/* ---------------------------------- CONSTANTS ----------------------------------------------*/


// Return true if the passed in cstring is a directory; false otherwise
bool is_dir(const char* path)
{
	struct stat buf;
	int status;
	status = stat(path, &buf);
	if (status == -1) {
		// If stat does not return 0, there was an error
		cout << endl << "ERROR: is_dir could not check path" << endl;
		return false;
	}

	if (buf.st_mode & S_IFDIR == 0)
		// Directory was not created -- creating "files" directory now
		return false;

	return true; // no errors and path was determined to be a directory
}

// Return true if the passed in cstring is a file that exists; false otherwise
bool file_exists(const char* name)
{
	struct stat buf;
	int status;
	status = stat(name, &buf);
	return status == 0;
}

// Create 3 .txt files in the passed in directory: followees, followers, texts
void create_user_files(const char* dir)
{
	chdir(dir);
	ofstream f1("followees.txt"); f1.close();
	ofstream f2("followers.txt"); f2.close();
	ofstream f3("texts.txt"); f3.close();
}

/*
A new user needs to be created;
Each user has a directory corresponding to their index number;
Each user's directory has 3 .txt files: followees, followers, texts
*/
void create_user_dir(const char* user_index, const char* dir)
{
	chdir(dir);
	int status = mkdir(user_index, S_IRWXU|S_IRWXG|S_IRWXO);
	if (status == -1) {
		// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED
		// mkdir was not successful
		cout << endl << "ERROR: mkdir could not create this new user's directory" << endl;
		// exit(1); ??????
		// return -1; ????
		return;
	}
	chmod(user_index, S_IRWXU|S_IRWXG|S_IRWXO); // give everyone RWX permissions
	char* dir_buf = (char*) malloc(MAX_PATH);
	strcpy(dir_buf, dir);
	dir_buf = strcat(dir_buf, "/");
	dir_buf = strcat(dir_buf, user_index);
	create_user_files(dir_buf);
	chdir(dir);
	free(dir_buf);
}

/*
The parameter check will be used in a switch statement and do one of the following 3 checks
1) If no password is passed, return true if either un or email match the username or email of a user in all_users.txt (used for registering a new user)
2) If a password is passed, return true if both un and pw match one user's username and password (used for logging in)
3) Return true if un, email, and pw all match one user's username, email, and password (used for deactivating a user)
*/
bool user_exists(fstream& fh, int check, const string& un, const string& email, const string& pw = "")
{
	// skip the first of the line of the file because it does not contain user info
	string first_line_garbage;
	fh.clear(); fh.seekp(0, ios::beg); getline(fh, garbage);

	// test_info is one line from the all_users.txt file
	// test_info = un,email,index,pw,fn,ln

	char* test_un = (char*) malloc(UN_BYTES);
	char* test_email = (char*) malloc(EMAIL_BYTES);
	char* test_pw = (char*) malloc(PW_BYTES);

	char* garbage = (char*) malloc(GARBAGE_BYTES);


	while (!fh.eof()) {
		// Test username
		fh.get(test_un, UN_BYTES, ','); 		// comma separated values, so ',' is the delim parameter

		// ',' is the next character in the stream, so just get that
		fh.get();

		fh.get(test_email, EMAIL_BYTES, ','); 	// comma separated values, so ',' is the delim parameter

		// ',' is the next character in the stream, so just get that
		fh.get();

		// garbage and .get() will get the index value of the user and the following ','
		fh.get(garbage, GARBAGE_BYTES, ',');
		fh.get();

		fh.get(test_pw, PW_BYTES, ',');			// comma separated values, so ',' is the delim parameter

		// garbage and .get() will get the next ',' along with the rest of the user info that is not need for testing 
		fh.get(); //garbage, 2);
		fh.getline(garbage, GARBAGE_BYTES);

		// 1) test username or email, 2) test username and password, 3) test username, email, and password
		switch (check)
		{
			case 1: // Register: both the username and email should be unique
				if( (strcmp(test_un, un.c_str()) == 0) || (strcmp(test_email, email.c_str()) == 0)  ) {
					free(test_un); free(test_email); free(test_pw); free(garbage);
					return true;
				}
			case 2: // Log in: both the username and password should match
				if( (strcmp(test_un, un.c_str()) == 0) && (strcmp(test_pw, pw.c_str()) == 0) ) {
					free(test_un); free(test_email); free(test_pw); free(garbage);
					return true;
				}
			case 3: // Deactivate: the username, email, and password should match
				if( (strcmp(test_un, un.c_str()) == 0) && (strcmp(test_email, email.c_str()) == 0) && (strcmp(test_pw, pw.c_str()) == 0) ) {
					free(test_un); free(test_email); free(test_pw); free(garbage);
					return true;
				}
		}
	}

	free(test_un); free(test_email); free(test_pw); free(garbage);
	return false; // got to the end of the file so return false
}

// Write over the current line that the fh stream is pointing to with a new index
void write_next_index(fstream& fh, string& next_index)
{
	fh.seekp(0, ios::beg);
	for (size_t i = 0; i < (MAX_USER_INFO_BYTES - next_index.size()); ++i)
		next_index += ",";
	//next_index += "\n";
	fh << next_index;
}

// Takes a file handle and adds the strings of user data to the end of the file with the line structure in the comments at the top
void add_user(fstream& fh, const string& un, const string& email, const string& index, const string& pw, const string& fn, const string& ln)
{
	string user_line = un + "," + email + "," + index + "," + pw + "," + fn + "," + ln + '\n';
	// Clear any flag bits that may impede writing to the file, then set the stream to point to the end of the file
	fh.clear();
	fh.seekp(0, ios::end);
	fh << user_line;
}

/*
The file passed in will be the followees.txt or followers.txt file containing the user we want to remove;
The index corresponds to the index of the user we will be removing
*/
void remove_user_from_files(fstream& fh, ofstream& temp_file, const string& index)
{
	string temp, temp_index;
	while (!fh.eof()) {
		// go through each line getting the index for each user
		getline(fh, temp);
		for (size_t i = 0; i < temp.size(); ++i) {
			if (temp[i] == ',')
				break;
			temp_index += temp[i];
		}

		// if the current line's index is not the user we are removing, add them to the new temp file
		if (temp_index != index && temp != "") {
			temp_file << temp;
			temp_file << "\n";
		}

		temp_index.clear();
	}
}

/*
Remove the user from all_users.txt
Remove the user from the followees.txt file of anyone in the user's followers.txt file
Remove the user from the followers.txt file of anyone in the user's followees.txt file
Delete the user's followees.txt, followers.txt, and texts.txt files and the user's directory
*/
void remove_user(fstream& fh, ofstream& temp_file, const string& un, const string& email, const string& pw, const char* my_cwd)
{
	string user_index;
	/* ---------------------------- REMOVING USER FROM all_users.txt --------------------------------- */
	string temp, temp_username;
	// place the first line of all_users.txt into the new temp file
	fh.clear(); fh.seekp(0, ios::beg);
	getline(fh, temp);
	temp_file << temp; temp_file << "\n";

	// add all lines of all_users.txt that don't match the specified username into the new temp file
	while (!fh.eof()) {
		getline(fh, temp);
		size_t i;
		for (i = 0; i < temp.size(); ++i) {
			if (temp[i] == ',')
				break;
			temp_username += temp[i];
		}

		// if the current line's user is the not the user we are removing, add them to the new temp file
		if (temp_username != un && temp != "") {
			temp_file << temp;
			temp_file << "\n";
		} else if (temp_username == un) { // this is the user we are removing
			// get user's index
			// first we must parse through the next data value which is the email and i is already at the index of email's first char
			for (++i; i < temp.size(); ++i) {
				if (temp[i] == ',')
					break;
			}
			// get user's index value until the next comma is hit
			for (++i; i < temp.size(); ++i) {
				if (temp[i] == ',')
					break;
				user_index += temp[i];
			}
		}

		temp_username.clear();
	}
	fh.close();

	/* ---------- REMOVING USER FROM OTHER PEOPLE'S followers.txt && followees.txt files ------------- */
	// set up variables to change directories, assuming our CWD is where the "files" directory is located
	char* files_cwd = (char*) malloc(MAX_PATH);		// will hold directory of "/files"
	char* user_dir_cwd = (char*) malloc(MAX_PATH);	// will hold directory of "/files/X" where X is a number representing this user's directory
	char* other_user_dir_cwd = (char*) malloc(MAX_PATH); 	// will hold directory of "/files/Y" where Y is a number representing another user's directory
	char* temp_dir = (char*) malloc(MAX_PATH);				// will hold directory of "/files/" as a way to use strcpy to reset other_user_dir_cwd for each new other user's index value
	getcwd(files_cwd, MAX_PATH);
	// Change the current directory to the user's directory who is being removed and set up cstrings to be used
	getcwd(user_dir_cwd, MAX_PATH);
	strcat(user_dir_cwd, "/");
	strcat(user_dir_cwd, user_index.c_str());
	getcwd(other_user_dir_cwd, MAX_PATH);
	strcat(other_user_dir_cwd, "/");
	getcwd(temp_dir, MAX_PATH);
	strcat(temp_dir, "/");
	chdir(user_dir_cwd);
	// delete the user's texts.txt file
	remove("texts.txt");
	// for each other user in this user's followees.txt file, remove this user from the other user's followers.txt file
	fstream followees_file("followees.txt");
	string other_user_index;
	while (!followees_file.eof()) {
		// go through each user in the followees.txt file
		getline(followees_file, temp);
		if (temp != "") {
			// get the other user's index value from the followees.txt file
			for (size_t i = 0; i < temp.size(); ++i) {
				if (temp[i] == ',')
					break;
				other_user_index += temp[i];
			}
			// change to the directory to the other user's directory and open their followers.txt file
			strcat(other_user_dir_cwd, other_user_index.c_str());
			chdir(other_user_dir_cwd);
			fstream other_followers_file("followers.txt");
			ofstream other_followers_temp_file("temp.txt");
			// remove the current user from the other user's followers.txt file
			remove_user_from_files(other_followers_file, other_followers_temp_file, user_index);
			// remove the old file with the user currently in it, and rename the new temp file to its name
			remove("followers.txt");
			rename("temp.txt", "followers.txt");
			other_followers_file.close(); other_followers_temp_file.close();
			other_user_index.clear();
			// reset other_user_dir_cwd to be concatenated with the next other user index
			strcpy(other_user_dir_cwd, temp_dir);
		}
	}
	followees_file.close();

	chdir(user_dir_cwd);	// change directories back to the current user being removed to get his/her followers.txt file

	// for each other user in this user's followers.txt file, remove this user from the other user's followees.txt file
	fstream followers_file("followers.txt");
	while (!followers_file.eof()) {
		// go through each user in the followers.txt file
		getline(followers_file, temp);
		if (temp != "") {
			// Get the other user's index value from the followers.txt file
			for (size_t j = 0; j < temp.size(); ++j) {
				if( temp[j] == ',')
					break;
				other_user_index += temp[j];
			}
			// change to the directory to the other user's directory and open their followers.txt file
			strcat(other_user_dir_cwd, other_user_index.c_str());
			chdir(other_user_dir_cwd);
			fstream other_followees_file("followees.txt");
			ofstream other_followees_temp_file("temp.txt");
			// remove the current user from the other user's followers.txt file
			remove_user_from_files(other_followees_file, other_followees_temp_file, user_index);
			// remove the old file with the user currently in it, and rename the new temp file to its name
			remove("followees.txt");
			rename("temp.txt", "followees.txt");
			other_followees_file.close(); other_followees_temp_file.close();
			other_user_index.clear();
			// reset other_user_dir_cwd to be concatenated with the next other user index
			strcpy(other_user_dir_cwd, temp_dir);
		}
	}
	followers_file.close();

	chdir(user_dir_cwd);

	// For the user being removed, delete his/her files
	remove("followees.txt");
	remove("followers.txt");

	chdir(my_cwd); // setting the working directory back to what it was before the remove_user function was called
	// For the user being removed, delete his/her directory
	rmdir(user_dir_cwd);

	free(files_cwd); free(user_dir_cwd); free(other_user_dir_cwd); free(temp_dir);
}


// register takes in 5 strings: first name, last name, email, username, password
// register stores those strings in a CSV line in all_users.txt in the directory files
void register_user(const string& un, const string& email, const string& pw, const string& fn, const string& ln, char* my_cwd)
{
	chdir(my_cwd); // make sure we are always in the correct directory
	// buf = current working directory; dir_buf = "CWD" + "USER_DIR" (USER_DIR is directory with all users of texty data)
	char* buf = (char*) malloc(MAX_PATH);
	char* dir_buf = (char*) malloc(MAX_PATH);
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
			free(buf); free(dir_buf);
			cout << endl << "ERROR: mkdir could not create files directory" << endl;
			// exit(1); ?????
			// return -1; ???
			return;
		}
		chmod(dir_buf, S_IRWXU|S_IRWXG|S_IRWXO); // give everyone RWX permissions
	}

	chdir(dir_buf); // access data inside USER_DIR directory, so change to that directory
	// Check if username or email are taken, if not taken {add this new user's info}
	char* file_path = (char*) malloc(MAX_PATH);
	strcpy(file_path, dir_buf);
	strcat(file_path, ALL_USERS_FILE);

	if (!file_exists(file_path)) {
		// If the file doesn't exist, create it
		ofstream temp_create_file(file_path);
		temp_create_file.close();
		fstream fh(file_path);
		if (!fh.is_open()) { //.is_open()) {
			// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED WITH FILE OPENING
			cout << endl << "ERROR: could not open all_users.txt" << endl;
			free(buf); free(dir_buf); free(file_path);
			return;
		} else { // 1st user created will have an index of 1, which changes N to 2 for the next user to register
			// The first line of the file is the index number that the next registered user will receieve, ie: 2
			string index_line = "2";
			for(size_t i = 1; i < MAX_USER_INFO_BYTES - 1; ++i)
				index_line += ",";
			index_line += "\n";
			fh << index_line; // Each line is 118 chars long, with commands appending user data to set a standard line length, and ends with a newline char

			string index_1 = "1";
			// Add this new user's info to the end of the all_users.txt file
			add_user(fh, un, email, index_1, pw, fn, ln);
			// Create the new user's directory and followees.txt, followers.txt, texts.txt files
			create_user_dir("1", dir_buf);
		}
	} else { // file does exist
		fstream fh(file_path);
		if (!fh.is_open()) {
			// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED WITH FILE OPENING
			free(buf); free(dir_buf); free(file_path);
			cout << endl << "ERROR: could not open all_users.txt" << endl;
			return;
		} else { // file is opened
			char* index = (char*) malloc(MAX_INDEX_BYTES); // MAX_INDEX originally set to 1000000 meaning 999999 user indexes could be handled at the creation
			fh.getline(index, MAX_INDEX_BYTES, ','); // index will get the first line in the file which contains a number followed by the EOL character

			if (user_exists(fh, 1, un, email)) {
				// IMPLEMENT WAY TO PASS BACK ERROR TO NETWORK THAT USER EXISTS ALREADY
				free(buf); free(dir_buf); free(file_path); free(index);
				cout << endl << "ERROR: username or email already exist" << endl;
				return;
			} else { // fh got to EOF in user_exists so user does not exist
				// Add this new user's info to the end of the all_users.txt file
				add_user(fh, un, email, index, pw, fn, ln);
				// Create the new user's directory and followees.txt, followers.txt, texts.txt files
				create_user_dir(index, dir_buf);
			}

			// The index at the beginning of all_users.txt must be incremented and updated in the file
			int next_index;
			next_index = atoi(index);
			next_index++;
			// Convert the next_index int value into a string to pass to write_next_index
			char next_index_buffer[MAX_INDEX_BYTES];
			sprintf(next_index_buffer, "%d", next_index);
			string next_index_string(next_index_buffer);
			write_next_index(fh, next_index_string); // write_next_index will move the stream to the beginning of the file

			fh.close();
			free(index);
		}
	}

	chdir(buf); // change the CWD back to its initial position at the beginning of the function
	free(buf), free(dir_buf), free(file_path);
}

// Takes a username (that is already known to exist) and returns the index of that user
string get_user_index(const string& un)
{
	string temp, temp_username, user_index;
	fstream fh(USER_DATA_FILENAME); // open the file with every stored user's info
	getline(fh, temp); // index line

	while (!fh.eof()) {
		getline(fh, temp);
		// index is preceded by a username then a ',' then an email then another ','
		for (size_t i = 0; i < temp.size(); ++i) {
			if (temp[i] == ',')
				break;
			temp_username += temp[i];
		}

		// if the current line's user is the one we're looking for, get the index and return it
		if (temp_username == un) {
			fh.close(); // close the file since we found the index
			// parse through the email chars to get to the index
			for (++i; i < temp.size(); ++i) {
				if (temp[i] == ',')
					break;
			}
			// get the index value and return it
			for (++i; i < temp.size(); ++i) {
				if (temp[i] == ',')
					return user_index;
				user_index += temp[i];
			}
		}

		temp_username.clear(); 
	}
}

// Takes a message and appends it (along with a newline character) to the user's texts.txt file
void submit_text(const string& texty, const string& un, char* files_cwd)
{
	chdir(files_cwd); // Change into the files directory
	string user_index = get_user_index(un);
	// change into the user's directory and open the texts.txt file
	char* user_dir = (char*) malloc(MAX_PATH);
	getcwd(user_dir, MAX_PATH);
	strcat(user_dir, "/");
	strcat(user_dir, user_index);
	fstream fh("texts.txt");
	// write the texty to the end of the texts.txt file
	fh.seekp(0, ios::end);
	fh << texty;
	fh << "\n";
	fh.close();

	chdir(files_cwd);

	free (user_dir);
}

// Content for viewing another user's texty homepage
void other_user_page(const string& un, const string& other_un, char* files_cwd)
{

}

/*
Search for the username or email of another user:
Return formatted user data in a string if the username/email is found: string = "username,email,firstname,lastname"
Return an empty string, "", if the username/email is not found
*/
string search(const string& search_str, char* files_cwd)
{
	// change into the files directory and open the file with all of the users' data
	chdir(files_cwd);
	fstream fh(USER_DATA_FILENAME);
	// skip the first of the line of the file because it does not contain user info
	string first_line_garbage;
	getline(fh, first_line_garbage);

	string test_un, test_email, user_data, return_user_data;

	while (!fh.eof()) {
		// get the next line of one user's data
		getline(fh, user_data);
		if (user_data != "") {
			size_t i;
			// get the username to test
			for (i = 0; i < user_data.size(); ++i) {
				if (user_data[i] == ',')
					break;
				test_un += user_data[i];
			}
			// get the email to test
			for (++i; i < user_data.size(); ++i) {
				if (user_data[i] == ',')
					break;
				test_email += user_data[i];
			}

			// if the username or email match, get the first and last name to display also
			if ( (test_un == search_str) || (test_email == search_str) ) {
				fh.close(); // close the file since we have all the data needed
				// get the index but don't store it
				for (++i; i < user_data.size(); ++i) {
					if (user_data[i] == ',')
						break;
				}
				// get the password but don't store it
				for (++i; i < user_data.size(); ++i) {
					if (user_data[i] == ',')
						break;
				}
				// put the username, email, first name, and last name into a comma separted string
				return_user_data = test_un + "," + test_email + ",";
				for (++i; i < user_data.size(); ++i) {
					return_user_data += user_data[i];
				}

				return return_user_data; // return the formatted user data: username,email,firstname,lastname
			}
		}
		test_un.clear(); test_email.clear();
	}

	fh.close();
	chdir(files_cwd);
	return ""; // return an empty string since no username/email matched
}

int main() {
	char* my_cwd = (char*) malloc(MAX_PATH);
	chdir(CURRENT_DIR);
	getcwd(my_cwd, MAX_PATH);
	// All files and data are stored in the directory files, so make that the current directory
	char* files_cwd = (char*) malloc(MAX_PATH);
	getcwd(files_cwd, MAX_PATH);
	strcat(files_cwd, USER_DIR);
	chdir(files_cwd);








	// string un, e, pw, fn, ln;
	// string un2, e2, pw2, fn2, ln2;
	// string un3, e3, pw3, fn3, ln3;
	// string un4, e4, pw4, fn4, ln4;
	// string un5, e5, pw5, fn5, ln5;

	// un = "jb"; e = "jb@g"; pw = "hi"; fn = "j"; ln = "b";
	// un2 = "ld"; e2 = "ld@g"; pw2 = "bye"; fn2 = "l"; ln2 = "d";
	// un3 = "jb"; e3 = "ml@g"; pw3 = "hi"; fn3 = "j"; ln3 = "b";
	// un4 = "ml"; e4 = "ml@g"; pw4 = "yo"; fn4 = "j"; ln4 = "b";
	// un5 = "sh"; e5 = "ld@g"; pw5 = "bark"; fn5 = "s"; ln5 = "h";

	// register_user(un, e, pw, fn, ln, my_cwd);
	// register_user(un2, e2, pw2, fn2, ln2, my_cwd);
	// register_user(un3, e3, pw3, fn3, ln3, my_cwd);
	// register_user(un4, e4, pw4, fn4, ln4, my_cwd);
	// register_user(un5, e5, pw5, fn5, ln5, my_cwd);
	chdir(my_cwd);
	free(my_cwd);
}



void usingRemoveUser()
{
	chdir(CURRENT_DIR);
	string un, email, pw, garbage;
	un = "ld"; email = "ld@g"; pw = "bye";

	char* my_cwd = (char*) malloc(MAX_PATH);
	char* files_cwd = (char*) malloc(MAX_PATH);
	getcwd(my_cwd, MAX_PATH);
	getcwd(files_cwd, MAX_PATH);

	strcat(files_cwd, USER_DIR);
	chdir(files_cwd);
	
	fstream fh(USER_DATA_FILENAME);
	getline(fh, garbage);	// first line is the index of the next user and commas

	if (user_exists(fh, 3, un, email, pw)) {
		ofstream temp_file("temp.txt");

		remove_user(fh, temp_file, un, email, pw, files_cwd);

		remove(USER_DATA_FILENAME);
		rename("temp.txt", USER_DATA_FILENAME);
	} else {
		cout << "Data did not match user -- no deactivation" << endl;
	}
	chdir(my_cwd);

}