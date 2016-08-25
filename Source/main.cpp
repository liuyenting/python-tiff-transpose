#include "stdafx.h"
#include <conio.h>		// _kbhit

#include <boost/timer/timer.hpp>
namespace tmr = boost::timer;
#include <boost/chrono.hpp>
namespace chr = boost::chrono;
typedef chr::duration<double> unitsec;
#include <boost/format.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

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

		/*
		unitsec sec = chr::nanoseconds(timer.elapsed().user +
									   timer.elapsed().system);
		*/

		float per = static_cast<float>(iLayer)/nLayer * 100;
		// Print new status.
		std::cout << boost::format("%.2f%%") % per << ", ";
		std::cout << tmr::format(timer.elapsed(), 2, "%ws elapsed");
		std::cout << std::endl;
	}

	std::cout << std::endl;
}

static void retrieveFileList(std::vector<fs::path> &fileList,
						 	 const fs::path &inDir) {
	std::cout << "Scanning " << inDir << "... ";
 	listTiffFiles(inDir, fileList);
 	std::cout << fileList.size() << " stack(s) found" << std::endl;

	std::cout << "Sorting the file sequence... ";
 	int rmCnt = sortSpimStacks(fileList);
 	if (rmCnt > 0) {
 		std::cout << rmCnt << " file(s) removed from the list";
 	}
	std::cout << std::endl;
}

int main(int argc, char **argv) {
	po::options_description desc("Options");
    desc.add_options()
		("help,h",
		 "Print help messages")
      	("indir", po::value<std::string>()->required(),
		 "input directory")
		("outdir", po::value<std::string>()->required(),
		 "output directory");
	po::positional_options_description posOpt;
	posOpt.add("indir", 1);
	posOpt.add("outdir", 1);
    po::variables_map vm;

	try {
		po::store(po::command_line_parser(argc, argv)
		 		  .options(desc).positional(posOpt).run(),
                  vm);
		if (vm.count("help") ) {
			std::cout << "Usage: stacktr INDIR OUTDIR" << std::endl;
			std::cout << std::endl;
			std::cout << "  INDIR is the input directory" << std::endl;
			std::cout << "  OUTDIR is the output directory" << std::endl;
		    return 0;
		}
		po::notify(vm); //
	} catch (po::required_option &e) {
		std::cerr << "Missing a required option" << std::endl;
		return 1;
	} catch (po::error &e) {
		std::cerr << "Something wrong during the parsing" << std::endl;
		return 2;
	}

	std::cout << std::endl;

    fs::path inDir(vm["indir"].as<std::string>());
	fs::path outDir(vm["outdir"].as<std::string>());

	std::vector<fs::path> fileList;
	retrieveFileList(fileList, inDir);

	// Create folder if not exist.
	boost::system::error_code retErr;
	fs::create_directories(outDir, retErr);
	if (!retErr) {
		std::cout << "Output directory created" << std::endl;
	}

	// Transposed stack will contain as much layer as the amount of stacks.
 	size_t nLayer = fileList.size();
	processFileList(fileList, outDir, static_cast<uint16_t>(nLayer));

	waitForKeypress();

	return 0;
}
