#include "stdafx.h"
#include <conio.h>		// _kbhit
#include <string>
#include <algorithm>	// std::random_shuffle
#include <random>		// std::default_random_engine

#include "files.hpp"
#include "tiff.hpp"

inline void waitForKeypress() {
	std::cout << "Press any key to continue..." << std::endl;
	while (!_kbhit());
}

int main(void) {
	fs::path p("G:\\dummy_cell");

	std::cout << "Scanning " << p << "... ";
	std::vector<fs::path> fileList;
	listTiffFiles(p, fileList);
	std::cout << fileList.size() << " stack(s) found" << std::endl;

	int rmCnt = sortSpimStacks(fileList);
	if (rmCnt > 0) {
		std::cout << "... " << rmCnt << " file(s) removed from the list" << std::endl;
	}

	for (const fs::path &file : fileList) {
		dealStack(file);
	}

	waitForKeypress();

	return 0;
}