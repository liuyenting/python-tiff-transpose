#include "stdafx.h"
#include <conio.h>		// _kbhit
#include <boost/progress.hpp>

#include "files.hpp"
#include "tiff.hpp"

inline void waitForKeypress() {
	std::cout << "Press any key to continue..." << std::endl;
	while (!_kbhit());
}

static void processFileList(std::vector<fs::path> &fileList,
						    const fs::path &outDir,
						    const uint16_t nLayer) {
	boost::progress_display show_progress(fileList.size());
	for (const fs::path &file : fileList) {
		dealStack(outDir, "layer_", file, nLayer);
		++show_progress;
	}
}

static void retrieveFileList(std::vector<fs::path> &fileList,
						 	 const fs::path &inDir) {
	std::cout << "Scanning " << inDir << "... ";
 	listTiffFiles(inDir, fileList);
 	std::cout << fileList.size() << " stack(s) found" << std::endl;

 	int rmCnt = sortSpimStacks(fileList);
 	if (rmCnt > 0) {
 		std::cout << "... " << rmCnt << " file(s) removed from the list" << std::endl;
 	}
}

int main(void) {
	fs::path inDir("G:\\dummy_cell");
	fs::path outDir("G:\\dummy_cell_out");

	std::vector<fs::path> fileList;
	retrieveFileList(fileList, inDir);

	// Transposed stack will contain as much layer as the amount of stacks.
 	size_t nLayer = fileList.size();
	processFileList(fileList, outDir, static_cast<uint16_t>(nLayer));

	waitForKeypress();

	return 0;
}
