#include "stdafx.h"
#include <libtiff/tif_config.h>
#include <libtiff/tiffio.h>

#include "tiff.hpp"

void dealStack(const fs::path &p) {
	std::string path = p.string();
	std::cout << "deal -> " << path << std::endl;

	// Open the file.
	in = TIFFOpen(path.c_str(), "r");
	if (in != NULL) {
		std::cout << "opened" << std::endl;
		(void)TIFFClose(in);
	}
}