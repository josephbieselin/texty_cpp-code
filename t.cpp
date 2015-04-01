#include <string>
#include <vector>
#include <iostream>
// #include <stdio.h>		// itoa
#include <fstream>
#include <sys/stat.h>	// provide stat functionality for directory checking
#include <string.h>		// provide c string capabilities
#include <unistd.h>		// provide functionality for UNIX calls
#include <stdlib.h>		// malloc, calloc, free
#include <typeinfo>

using namespace std;

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

// 1. register, 2. log in, 3. deactivate
bool user_exists(fstream& fh, int check, const string& un, const string& email, const string& pw = "")
{
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

/*
The file passed in will be the followees.txt or followers.txt file containing the user we want to remove;
The index corresponds to the index of the user we will be removing
*/
void remove_user_from_files(fstream& fh, ofstream& temp_file, const string& index)
{
	string temp, temp_index;
	while (!fh.eof()) {
		getline(fh, temp);
		for (size_t i = 0; i < temp.size(); ++i) {
			if (temp[i] == ',')
				break;
			temp_index += temp[i];
		}

		// if the current line's index is not the user we are removing, add them to the new temp file
		if (temp_index != index) {
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
			for ( ; i < temp.size(); ++i) {
				if (temp[i] == ',')
					break;
			}
			// get user's index value until the next comma is hit
			for ( ; i < temp.size(); ++i) {
				if (temp[i] == ',')
					break;
				user_index += temp[i];
			}
		}

		temp_username.clear();
	}
	// I DO NOT THINK THIS IS NEEDED fh.clear(); fh.seekp(0, ios::beg);

	/* ---------- REMOVING USER FROM OTHER PEOPLE'S followers.txt && followees.txt files ------------- */
	// set up variables to change directories, assuming our CWD is where the "files" directory is located
	char* files_cwd = (char*) malloc(MAX_PATH);		// will hold directory of "/files"
	char* user_dir_cwd = (char*) malloc(MAX_PATH);	// will hold directory of "/files/X" where X is a number representing this user's directory
	char* other_user_dir_cwd = (char*) malloc(MAX_PATH); 	// will hold directory of "/files/Y" where Y is a number representing another user's directory
	char* temp_dir = (char*) malloc(MAX_PATH);				// will hold directory of "/files/" as a way to use strcpy to reset other_user_dir_cwd for each new other user's index value
	getcwd(files_cwd, MAX_PATH);
	strcat(files_cwd, USER_DIR);
	chdir(files_cwd);
	// Change the current directory to the user's directory who is being removed
	getcwd(user_dir_cwd, MAX_PATH);
	strcat(user_dir_cwd, "/");
	strcat(user_dir_cwd, user_index.c_str());
	getcwd(other_user_dir_cwd, MAX_PATH);
	strcat(other_user_dir_cwd, "/");
	getcwd(temp_dir, MAX_PATH);
	strcat(temp_dir, "/");
	chdir(user_dir_cwd);
	// delete the user's texts.txt file
	remove("/texts.txt");
	// for each other user in this user's followees.txt file, remove this user from the other user's followers.txt file
	fstream followees_file("/followees.txt");
	string other_user_index;
	while (!followees_file.eof()) {
		getline(followees_file, temp);
		for (size_t i = 0; i < temp.size(); ++i) {
			if (temp[i] == ',')
				break;
			other_user_index += temp[i];
		}
		// change to the directory to the other user's directory and open their followers.txt file
		strcat(other_user_dir_cwd, other_user_index.c_str());
		chdir(other_user_dir_cwd);
		fstream other_followers_file("/followers.txt");
		ofstream other_followers_temp_file("/temp.txt");
		// remove the current user from the other user's followers.txt file
		remove_user_from_files(other_followers_file, other_followers_temp_file, user_index);
		// remove the old file with the user currently in it, and rename the new temp file to its name
		remove("/followers.txt");
		rename("/temp.txt", "/followers.txt");
		other_followers_file.close(); other_followers_temp_file.close();
		other_user_index.clear();
		// reset other_user_dir_cwd to be concatenated with the next other user index
		strcpy(other_user_dir_cwd, temp_dir);
	}

	chdir(user_dir_cwd);	// change directories back to the current user being removed to get his/her followers.txt file

	fstream followers_file("/followers.txt");
	while (!followers_file.eof()) {
		getline(followers_file, temp);
		for (size_t i = 0; i < temp.size(); ++i) {
			if( temp[i] = ',')
				break;
			other_user_index += temp[i];
		}
		// change to the directory to the other user's directory and open their followers.txt file
		strcat(other_user_dir_cwd, other_user_index.c_str());
		chdir(other_user_dir_cwd);
		fstream other_followees_file("/followees.txt");
		ofstream other_followees_temp_file("/temp.txt");
		// remove the current user from the other user's followers.txt file
		remove_user_from_files(other_followees_file, other_followees_temp_file, user_index);
		// remove the old file with the user currently in it, and rename the new temp file to its name
		remove("/followees.txt");
		rename("/temp.txt", "/followees.txt");
		other_followees_file.close(); other_followees_temp_file.close();
		other_user_index.clear();
		// reset other_user_dir_cwd to be concatenated with the next other user index
		strcpy(other_user_dir_cwd, temp_dir);
	}

	chdir(my_cwd); // setting the working directory back to what it was before the remove_user function was called
}


int main()
{

	string un, email, pw;
	un = "ld"; email = "ld@g"; pw = "bye";

	char* my_cwd = (char*) malloc(MAX_PATH);
	char* files_cwd = (char*) malloc(MAX_PATH);
	strcpy(my_cwd, USER_DIR);
	chdir(my_cwd);
	getcwd(my_cwd, MAX_PATH);
	getcwd(files_cwd, MAX_PATH);
	strcat(files_cwd, USER_DIR);
	chdir(files_cwd);
	fstream fh(ALL_USERS_FILE);

	if (user_exists(fh, 3, un, email, pw)) {
		ofstream temp_file("/temp.txt");

		remove_user(fh, temp_file, un, email, pw, my_cwd);

		remove(ALL_USERS_FILE);
		rename("/temp.txt", ALL_USERS_FILE);
	} else {
		cout << "Data did not match user -- no deactivation" << endl;
	}



	// place the first of the all_users.txt into the new temp file

	// fstream fh("all_users.txt");
	// getline(fh, garbage);

	// if (user_exists(fh, 1, un, email))
	// 	cout << "Test username/email already exists" << endl;
	// else
	// 	cout << "That username and password do not already exist, good job" << endl;


	
	// fh.clear(); fh.seekp(0, ios::beg); getline(fh, garbage);



	// if (user_exists(fh, 2, un, "", pw))
	// 	cout << "Logged in" << endl;
	// else
	// 	cout << "Not logged in" << endl;



	// fh.clear(); fh.seekp(0, ios::beg); getline(fh, garbage);



	// if (user_exists(fh, 3, un, email,pw))
	// 	cout << "Deactivated" << endl;
	// else
	// 	cout << "Not deactivated" << endl;





	// // register - 1
	// if (user_exists(fh, 1, un, email))
	// 	cout << "That username/password already exists" << endl;
	// else
	// 	cout << "That username and password do already exist" << endl;

	// fh.close();
	// fstream fh2("all_users.txt");
	// fh.clear();
	// fh.seekp(0, ios::beg);
	// getline(fh, garbage);

	// // 2 - log in
	// if (user_exists(fh2, 2, un, "null", pw))
	// 	cout << "You're logged in" << endl;
	// else
	// 	cout << "Log in info incorrect" << endl;

	// fh.close();
	// fstream fh3("all_users.txt");
	// getline(fh, garbage);

	// // deactivate - 3
	// if (user_exists(fh3, 3, un, email, pw))
	// 	cout << "You're deactived" << endl;
	// else
	// 	cout << "No deactivation for you!" << endl;

}



