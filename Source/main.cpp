#include "stdafx.h"
#include <conio.h>		// _kbhit

#include <boost/timer/timer.hpp>
namespace tmr = boost::timer;
#include <boost/chrono.hpp>
namespace chr = boost::chrono;
typedef chr::duration<double> unitsec;
#include <boost/format.hpp>

#include "files.hpp"
#include "tiff.hpp"

inline void waitForKeypress() {
	std::cout << "Press any key to continue..." << std::endl;
	while (!_kbhit());
}

static void processFileList(std::vector<fs::path> &fileList,
						    const fs::path &outDir,
						    const uint16_t nLayer) {
	std::cout << std::endl;

	tmr::cpu_timer timer;
	tmr::nanosecond_type last(0);
	uint16_t iLayer = 0;
	for (const fs::path &file : fileList) {
		dealStack(outDir, "layer_", file, nLayer);
		iLayer++;

		unitsec sec = chr::nanoseconds(timer.elapsed().user +
									   timer.elapsed().system);

		// Print new status.
		std::cout << boost::format("%.2f%%, %.2f seconds elapsed") %
					 (static_cast<float>(iLayer)/nLayer * 100) %
					 sec.count() << std::endl;
	}

	std::cout << std::endl;
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
