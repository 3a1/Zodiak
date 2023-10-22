#include <Windows.h>
#include <iostream>
#include <string>

#define CHAR_ARRAY_SIZE 128

using namespace std;

int main() {
	int varInt(123456);
	string varString("DefaultString");
	char arrChar[CHAR_ARRAY_SIZE] = "Long char array right there ->";
	int* ptr2int(&varInt);
	int** ptr2ptr(&ptr2int);
	int*** ptr2ptr2(&ptr2ptr);

	do {
		cout << "Process ID: " << dec << GetCurrentProcessId() << endl;
		cout << "Thread  ID: " << dec << GetCurrentThreadId() << endl;
		cout << endl;
		cout << "varInt       (0x" << hex << uppercase << (uintptr_t)&varInt << ") = " << dec << varInt << endl;
		cout << "varString    (0x" << hex << uppercase << (uintptr_t)&varString << ") = " << varString << endl;
		cout << "arrChar[" << dec << CHAR_ARRAY_SIZE << "] (0x" << hex << uppercase << (uintptr_t)&arrChar << ") = " << arrChar << endl;
		cout << endl;
		cout << "ptr2int      (0x" << hex << uppercase << (uintptr_t)&ptr2int << ") = 0x" << ptr2int << endl;
		cout << "ptr2ptr      (0x" << hex << uppercase << (uintptr_t)&ptr2ptr << ") = 0x" << ptr2ptr << endl;
		cout << "ptr2ptr2     (0x" << hex << uppercase << (uintptr_t)&ptr2ptr2 << ") = 0x" << ptr2ptr2 << endl;
		cout << endl;
		cout << "Press ENTER to print again." << endl;
		getchar();
		cout << endl << "---------------------------------------------------" << endl << endl;
	} while (true);

	return EXIT_SUCCESS;
}