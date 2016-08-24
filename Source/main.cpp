#include <iostream>
#include <conio.h>		// _kbhit

int main(void) {
	std::cout << "Hello world!" << std::endl;

	std::cout << "Press any key to continue..." << std::endl;
	while(!_kbhit());

	return 0;
}