#include "stdafx.h"
#include <conio.h>		// _kbhit

#include "files.hpp"
#include "tiff.hpp"

inline void waitForKeypress() {
	std::cout << "Press any key to continue..." << std::endl;
	while (!_kbhit());
}

static void processFileList(std::vector<fs::path> &fileList,
						    const uint16_t nLayer) {
	for (const fs::path &file : fileList) {
		dealStack(outdir, "layer_", file, nLayer);
	}
}

static void retrieveFileList(const fs::path& indir,
							 std::vector<fs::path> &fileList) {
	std::cout << "Scanning " << indir << "... ";
 	std::vector<fs::path> fileList;
 	listTiffFiles(indir, fileList);
 	std::cout << fileList.size() << " stack(s) found" << std::endl;

 	int rmCnt = sortSpimStacks(fileList);
 	if (rmCnt > 0) {
 		std::cout << "... " << rmCnt << " file(s) removed from the list" << std::endl;
 	}
}

int main(void) {
	fs::path p("G:\\dummy_cell");
	fs::path outdir("G:\\dummy_cell_out");

	// Transposed stack will contain as much layer as the amount of stacks.
 	size_t nLayer = fileList.size();
	processFileList(fileList, static_cast<uint16_t>(nLayer));

	waitForKeypress();

	return 0;
}
