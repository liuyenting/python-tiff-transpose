#include "stdafx.h"
#include <conio.h>		// _kbhit

#include "files.hpp"
#include "tiff.hpp"

inline void waitForKeypress() {
	std::cout << "Press any key to continue..." << std::endl;
	while (!_kbhit());
}

int main(void) {
	fs::path p("G:\\dummy_cell");
	fs::path outdir("G:\\dummy_cell_out");

	std::cout << "Scanning " << p << "... ";
	std::vector<fs::path> fileList;
	listTiffFiles(p, fileList);
	// Transposed stack will contain as much layer as the amount of stacks.
	size_t nLayer = fileList.size();
	std::cout << fileList.size() << " stack(s) found" << std::endl;

	int rmCnt = sortSpimStacks(fileList);
	if (rmCnt > 0) {
		std::cout << "... " << rmCnt << " file(s) removed from the list" << std::endl;
	}

	for (const fs::path &file : fileList) {
		dealStack(outdir, "layer_", file, static_cast<uint16_t>(nLayer));
	}

	waitForKeypress();

	return 0;
}