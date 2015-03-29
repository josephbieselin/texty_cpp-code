#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>	// provide stat functionality for directory checking
#include <sys/types.h>	// 
#include <string.h>		// provide c string capabilities
#include <unistd.h>		// provide functionality for UNIX calls
#include <stdlib.h>		// malloc, calloc, free

using namespace std;

#define MAX_PATH 1000

#define USER_DIR "/files"

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


int main() {


	char* dir = (char*) malloc(100);
	//dir = getcwd(dir, MAX_PATH)
	strcat(getcwd(dir, MAX_PATH), "/files");
	chdir(dir);
	const char* name = "test.txt";
	cout << file_exists(name) << endl;








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
}




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
