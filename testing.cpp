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
#define ALL_USERS_FILE "/all_users.txt"	// corresponds to file with every user's info and index number
#define MAX_INDEX_BYTES 10		// maximum number of user indexes that can be used at creation of files: 10 bytes = 999999999 possible indexes for a cstring
#define MAX_USER_INFO_BYTES 118	// maximum number of bytes each line in all_users.txt will be for each user including: user's data, commas, and '\n' character
#define UN_BYTES 17			// maximum number of characters in username based on value limited in PHP file
#define EMAIL_BYTES 41		// maximum number of characters in email based on value limited in PHP file
#define PW_BYTES 17			// maximum number of characters in password based on value limited in PHP file
#define FN_BYTES 21			// maximum number of characters in first name based on value limited in PHP file
#define LN_BYTES 21			// maximum number of characters in last name based on value limited in PHP file
#define GARBAGE_BYTES 70	// length of bytes to hold characters after username and email fields in file handling
#define CURRENT_DIR "~/Dropbox/Coding/PDC/texty_cpp/"	// directory where C++ files and user file directory is stored


// // Copy a file
// #include <fstream>      // std::ifstream, std::ofstream

// int main () {
//   std::ifstream infile ("test.txt",std::ifstream::binary);
//   std::ofstream outfile ("new.txt",std::ofstream::binary);

//   // get size of file
//   infile.seekg (0,infile.end);
//   long size = infile.tellg();
//   infile.seekg (0);

//   // allocate memory for file content
//   char* buffer = new char[size];

//   // read content of infile
//   infile.read (buffer,size);

//   // write to outfile
//   outfile.write (buffer,size);

//   // release dynamically-allocated memory
//   delete[] buffer;

//   outfile.close();
//   infile.close();
//   return 0;
// }



// bool user_exists(fstream& fh, string& un, string& email)
// {
// 	// test_info is one line from the all_users.txt file
// 	// test_info = un,email,index,pw,fn,ln
// 	char* test_un = (char*) malloc(UN_BYTES * sizeof(char));
// 	char* test_email = (char*) malloc(EMAIL_BYTES * sizeof(char));
// 	char* garbage = (char*) malloc(GARBAGE_BYTES *sizeof(char));

// 	while (!fh.eof()) {
// 		fh.get(test_un, UN_BYTES, ','); 		// comma separated values, so ',' is the delim parameter
// 		if (strcmp(test_un, un.c_str()) == 0)
// 			return true; // the passed in username is equal to a username already created

// 		// ',' is the next character in the stream, so just get that
// 		fh.get(); //garbage, 2);

// 		// gets up to EMAIL_BYTES or until ',' is reached --> if ',' reached it is the next character that will be extracted from the stream
// 		fh.get(test_email, EMAIL_BYTES, ','); 	// comma separated values, so ',' is the delim parameter
// 		if (strcmp(test_email, email.c_str()) == 0)
// 			return true; // the passed in email is equal to an email already created

// 		// get the rest of the line until '\n' is reached
// 		fh.getline(garbage, GARBAGE_BYTES);
// 	}
// 	return false; // the username and email passed in were not found
// }

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
		if (strcmp(test_email, email.c_str()) == 0) {
			free(test_un); free(test_email); free(garbage);
			return true; // the passed in email is equal to an email already created
		}
		// get the rest of the line until '\n' is reached
		fh.getline(garbage, GARBAGE_BYTES);
	}
	free(test_un); free(test_email); free(garbage);
	return false; // the username and email passed in were not found
}


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


bool file_exists(const char* name)
{
	struct stat buf;
	int status;
	status = stat(name, &buf);
	return status == 0;

	// fstream fh(name);
	// if (fh.good()) {
	// 	fh.close();
	// 	return true;
	// } else {
	// 	fh.close();
	// 	return false;
	// }
}





int main() {
	string s;
	fstream fh("files/all_users.txt");
	// while(!fh.eof())
	// 	getline(fh, s);
	char* index = (char*) malloc(MAX_INDEX_BYTES * sizeof(char)); // MAX_INDEX originally set to 1000000 meaning 999999 user indexes could be handled at the creation
	fh.getline(index, MAX_INDEX_BYTES - 1, '\n'); // index will get the first line in the file which contains a number followed by the EOL character
	
	while(!fh.eof()) getline(fh,s);

	fh.clear();


	int next_index;
	next_index = atoi(index);
	next_index++;
	cout << endl << "Next index: " << next_index << endl;

	string un, e, pw, fn, ln;
	un = "SHADOW"; e = "ld@g"; pw = "bye"; fn = "l"; ln = "d";

	string info = un + "," + e + "," + index + "," + pw + "," + fn + "," + ln + '\n';
	const char* buffer = info.c_str();
	fh.seekp(0, ios::end);
	fh.write(buffer, strlen(buffer));


	cout << endl << endl;


	char* i = (char*) malloc(3);
	strcpy(i, "2");
	int i2;
	i2 = atoi(i);
	i2++;
	cout << i2 << endl;
	free(i);

	// char* my_cwd = (char*) malloc(MAX_PATH);
	// strcpy(my_cwd, CURRENT_DIR);
	// chdir(my_cwd);
	// getcwd(my_cwd, MAX_PATH);
	// cout << endl << file_exists("texty_files.cpp") << endl;
	// ofstream f;
	// f.open("hi.txt");

	// char* path = (char*) malloc(MAX_PATH);
	// strcpy(path, my_cwd);
	// strcat(path, USER_DIR);
	// cout << endl << is_dir(path) << endl;
	// strcat(path, ALL_USERS_FILE);

	// cout << endl << path << endl;

	// fstream fh;
	// fh.open(path, ios::in | ios::out);

}

// 	string un = "lk";
// 	string email = "lk@g";
// 	string index = "4";
// 	string pw = "hi";
// 	string fn = "j";
// 	string ln = "b";

// 	//char* buf = (char*) malloc(MAX_USER_INFO_BYTES * sizeof(char));

// 	string info = un + "," + email + "," + index + "," + pw + "," + fn + "," + ln + '\n';
// 	cout << info << endl;
// 	const char* buffer = info.c_str();
// 	// strcat( strcat(buf, ","), email );
// 	cout << strlen(buffer) << endl;

// 	fstream fh;
// 	fh.open("samp.txt");
// 	fh.seekp(0, ios::end);
// 	fh.write(buffer, strlen(buffer));

// 	if (!fh.is_open()) {
// 		// IMPLEMENT WAY TO PASS BACK TO NETWORK THAT ERROR OCCURRED WITH FILE OPENING
// 		cout << endl << "ERROR: could not open all_users.txt" << endl;
// 		return 0;
// 	} else { // file is opened
// 		char* index = (char*) malloc(MAX_INDEX_BYTES * sizeof(char)); // MAX_INDEX originally set to 1000000 meaning 999999 user indexes could be handled at the creation
// 		fh.getline(index, MAX_INDEX_BYTES - 1, '\n'); // index will get the first line in the file which contains a number followed by the EOL character

// 		if (user_exists(fh, un, email)) {
// 			// IMPLEMENT WAY TO PASS BACK ERROR TO NETWORK THAT USER EXISTS ALREADY
// 			cout << endl << "ERROR: username or email already exist" << endl;
// 			return 0;
// 		} else { // fh got to EOF in user_exists so user does not exist
// 			// input the user data into the file stream with comma separation and an EOL character
// 			//fh.write(buffer, strlen(buffer));
// 			//cin >> un >> "," ;//>> email >> "," >> index >> "," >> pw >> "," >> "fn" >> "," >> "ln" >> '\n';
// 		}
// 	}

// }




// // Created for the purpose of the destructor in case a function terminates early, the memory will be freed
// // Constructor takes in a cstring 
// class my_cstr {
// 	friend ostream& operator<<(ostream& os, const my_cstr& this_cstr) {
// 		return os << this_cstr.val;
// 	}
// private:
// 	int length;
// public:
// 	char* val;

// 	// Big 3
// 	my_cstr(int length, const char* str = "") : val((char*) malloc(length * sizeof(char))), length(strlen(str)) { strcpy(val, str); cout << "Constructor" << endl;}
// 	my_cstr(const my_cstr& other_cstr) : val((char*) malloc(other_cstr.length * sizeof(char))), length(other_cstr.length) { strcpy(val, other_cstr.val); cout << "Copy constructor" << endl;}
// 	my_cstr& operator=(const my_cstr& other_cstr) {
// 		cout << "Assignment operator" << endl;
// 		if (this == &other_cstr) {
// 			cout << "Self assignment is weird" << endl;
// 			return *this;
// 		}
// 		free(val);
// 		length = other_cstr.length;
// 		val = (char*) malloc(other_cstr.length * sizeof(char));
// 		strcpy(val, other_cstr.val);
// 		return *this;
// 	}
// 	// Destructor
// 	~my_cstr() {
// 		cout << "In the destructor" << endl;
// 		cout << val << endl;
// 		free(val);
// 		cout << val << endl;
// 	}

// 	// takes a string value to set equal to val
// 	void new_val(const string& str) {
// 		cout << "In new_val" << endl;
// 		free(val);
// 		val = (char*) malloc(str.length() + 1);
// 		strcpy(val, str.c_str());
// 	}
// };





	// // gets up to UN_BYTES characters or until ',' is reached --> if ',' reached it is the next character that will be extracted from the stream
	// fh.get(test_un, UN_BYTES, ','); 		// comma separated values, so ',' is the delim parameter
	// cout << test_un << " ?= " << un << endl;
	// if (strcmp(test_un, un.c_str()) == 0)
	// 	cout << false << endl;
	// else
	// 	cout << true << endl;

	// // ',' is the next character in the stream, so just get that
	// fh.get(); //garbage, 2);

	// // gets up to EMAIL_BYTES or until ',' is reached --> if ',' reached it is the next character that will be extracted from the stream
	// fh.get(test_email, EMAIL_BYTES, ','); 	// comma separated values, so ',' is the delim parameter
	// cout << test_email << " ?= " << email << endl;
	// if (strcmp(test_email, email.c_str()) == 0)
	// 	cout << false << endl;
	// else
	// 	cout << true << endl;

	// // get the rest of the line until '\n' is reached
	// fh.getline(garbage, GARBAGE_BYTES);
	// cout << garbage << endl;


	// fh.get(test_un, UN_BYTES, ',');
	// cout << test_un << " ?= " << un << endl;
	// if (strcmp(test_un, un.c_str()) == 0)
	// 	cout << false << endl;
	// else
	// 	cout << true << endl;

	// fh.get();

	// fh.get(test_email, EMAIL_BYTES, ','); 	// comma separated values, so ',' is the delim parameter
	// cout << test_email << " ?= " << email << endl;
	// if (strcmp(test_email, email.c_str()) == 0)
	// 	cout << false << endl;
	// else
	// 	cout << true << endl;


	// const char* c = "hello";
	// my_cstr c2(10, c);
	// cout << c2 << endl;
	// c2.new_val("bye");
	// cout << c2 << endl;
	// my_cstr c3(c2);
	// cout << c3 << endl;
	// c3 = c3;
	// c3.new_val("hola");
	// cout << c3 << endl;
	// c3 = c2;
	// cout << c3 << endl;

	// my_cstr buf(MAX_PATH);
	// getcwd(buf.val, MAX_PATH);
	// // cout << buf.val << endl;
	// cout << buf.val << endl;
	// my_cstr dir_buf(buf);
	// strcat(dir_buf.val, USER_DIR);
	// cout << dir_buf.val << endl;
	// cout << buf.val << endl;

	// char* buf = (char*) malloc(MAX_PATH * sizeof(char));
	// char* dir_buf = (char*) malloc(MAX_PATH * sizeof(char));
	// buf = getcwd(buf, MAX_PATH);
	// cout << buf << endl;


	// strcpy(dir_buf, buf);
	// dir_buf = strcat(dir_buf, USER_DIR);

	// const char* filename = "all_users.txt";
	// ofstream fh;
	// fh.open(filename);
