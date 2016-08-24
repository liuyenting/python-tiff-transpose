#include "stdafx.h"
#include <tiffio.h>

#include "tiff.hpp"

#define	CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) TIFFSetField(out, tag, v)
#define	CopyField2(tag, v1, v2) \
    if (TIFFGetField(in, tag, &v1, &v2)) TIFFSetField(out, tag, v1, v2)
#define	CopyField3(tag, v1, v2, v3) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3)) TIFFSetField(out, tag, v1, v2, v3)

int cpStrips(TIFF *in, TIFF *out)
{
    tsize_t bufSize = TIFFStripSize(in);
    std::cout << "StripSize = " << bufSize << std::endl;
	tdata_t buf = _TIFFmalloc(bufSize);
	if (buf) {
		uint64 *byteCnt;
		if (!TIFFGetField(in, TIFFTAG_STRIPBYTECOUNTS, &byteCnt)) {
            std::cerr << "Strip bytes counts are missing" << std::endl;
            _TIFFfree(buf);
			return 0;
		}

        tstrip_t ns = TIFFNumberOfStrips(in);
        std::cout << "NumberOfStrips = " << ns << std::endl;
		for (tstrip_t is = 0; is < ns; is++) {
            std::cerr << "byte count[" << is << "] = " << byteCnt[is] << std::endl;
			if (byteCnt[is] != (uint64)bufSize) {
                std::cout << "...reallocation" << std::endl;
                bufSize = (tsize_t)byteCnt[is];
				buf = _TIFFrealloc(buf, bufSize);
				if (!buf) {
					return 0;
                }
			}
			if ((TIFFReadRawStrip(in, is, buf, bufSize) < 0) ||
			    (TIFFWriteRawStrip(out, is, buf, bufSize) < 0)) {
				_TIFFfree(buf);
				return 0;
			}
		}

		_TIFFfree(buf);
		return 1;
	}
	return 0;
}

int cpTiff(TIFF *in, TIFF *out, const uint16 idx) {
    uint16 bitspersample, samplesperpixel, compression, shortv, *shortav;
	uint32 w, l;
	float floatv;
	char *stringv;
	uint32 longv;

	//CopyField(TIFFTAG_SUBFILETYPE, longv);
    TIFFSetField(out, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
	CopyField(TIFFTAG_IMAGEWIDTH, w);
    std::cerr << "IMAGEWIDTH " << w << std::endl;
	CopyField(TIFFTAG_IMAGELENGTH, l);
    std::cerr << "IMAGELENGTH " << l << std::endl;
	CopyField(TIFFTAG_BITSPERSAMPLE, bitspersample);
    std::cerr << "BITSPERSAMPLE " << bitspersample << std::endl;
	CopyField(TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
    std::cerr << "SAMPLESPERPIXEL " << samplesperpixel << std::endl;
	CopyField(TIFFTAG_COMPRESSION, compression);
	if (compression == COMPRESSION_JPEG) {
		uint32 count = 0;
		void *table = NULL;
		if (TIFFGetField(in, TIFFTAG_JPEGTABLES, &count, &table) &&
            (count > 0) &&
            (table != NULL)) {
		    TIFFSetField(out, TIFFTAG_JPEGTABLES, count, table);
		}
	}
    CopyField(TIFFTAG_PHOTOMETRIC, shortv);
	CopyField(TIFFTAG_PREDICTOR, shortv);
	CopyField(TIFFTAG_THRESHHOLDING, shortv);
	CopyField(TIFFTAG_FILLORDER, shortv);
	CopyField(TIFFTAG_ORIENTATION, shortv);
	CopyField(TIFFTAG_MINSAMPLEVALUE, shortv);
	CopyField(TIFFTAG_MAXSAMPLEVALUE, shortv);
	CopyField(TIFFTAG_XRESOLUTION, floatv);
	CopyField(TIFFTAG_YRESOLUTION, floatv);
	CopyField(TIFFTAG_GROUP3OPTIONS, longv);
	CopyField(TIFFTAG_GROUP4OPTIONS, longv);
	CopyField(TIFFTAG_RESOLUTIONUNIT, shortv);
	CopyField(TIFFTAG_PLANARCONFIG, shortv);
	CopyField(TIFFTAG_ROWSPERSTRIP, longv);
    std::cerr << "ROWSPERSTRIP " << longv << std::endl;
	CopyField(TIFFTAG_XPOSITION, floatv);
	CopyField(TIFFTAG_YPOSITION, floatv);
	CopyField(TIFFTAG_IMAGEDEPTH, longv);
    std::cerr << "IMAGEDEPTH " << longv << std::endl;
	CopyField(TIFFTAG_SAMPLEFORMAT, shortv);
	CopyField2(TIFFTAG_EXTRASAMPLES, shortv, shortav);
	{
        uint16 *red, *green, *blue;
        CopyField3(TIFFTAG_COLORMAP, red, green, blue);
	}
	{
        //uint16 shortv2;
        //CopyField2(TIFFTAG_PAGENUMBER, shortv, shortv2);
        TIFFSetField(out, TIFFTAG_PAGENUMBER, idx, 0);
	}
	CopyField(TIFFTAG_ARTIST, stringv);
	CopyField(TIFFTAG_IMAGEDESCRIPTION, stringv);
	CopyField(TIFFTAG_MAKE, stringv);
	CopyField(TIFFTAG_MODEL, stringv);
	CopyField(TIFFTAG_SOFTWARE, stringv);
	CopyField(TIFFTAG_DATETIME, stringv);
	CopyField(TIFFTAG_HOSTCOMPUTER, stringv);
	CopyField(TIFFTAG_PAGENAME, stringv);
	CopyField(TIFFTAG_DOCUMENTNAME, stringv);
	if (TIFFIsTiled(in)) {
        std::cerr << "Tiled TIFF is not supported" << std::endl;
		return 0;
	}

    for (uint32_t j = 0; j < l; j++)
        TIFFWriteScanline(out, )
}

std::string genPath(const fs::path &outdir, const std::string &prefix,
                    const int idx) {
    fs::path fname(prefix + std::to_string(idx+1) + ".tif");
    fs::path fpath = outdir / fname;
	return fpath.string();
}

/*
 * Deal the stacks into separated files and append them.
 */
void dealStack(const fs::path &outdir, const std::string &prefix,
               const fs::path &p) {
	TIFF *in, *out;
    static uint16 layer = 0;

	// Open the file.
	in = TIFFOpen(p.string().c_str(), "r");
	if (in == NULL) {
		std::cerr << "Unable to read " << p.filename() << std::endl;
		return;
	}

    // Identify the read mode.
	static char mode[3] = { 'x', 'b', 0 };
    mode[0] = (mode[0] == 'x') ? 'w' : 'a';
    mode[1] = (TIFFIsBigEndian(in)) ? 'b' : 'l';

    std::cout << "Layer " << layer << ", ";
    std::cout << ((mode[0] == 'a') ? "Append" : "Overwrite") << std::endl;

	// Iterate through the directories.
	int idx = 0;
	do {
        std::string s = genPath(outdir, prefix, idx);
		out = TIFFOpen(s.c_str(), mode);
		if (out == NULL) {
			std::cerr << "Unable to create output file" << std::endl;
            (void) TIFFClose(in);
            (void) TIFFClose(out);
			return;
		} else if (!cpTiff(in, out, layer)) {
			std::cerr << "Unable to copy the layer" << std::endl;
            (void) TIFFClose(in);
            (void) TIFFClose(out);
			return;
		}
		TIFFClose(out);

		idx++;
	} while (TIFFReadDirectory(in));

    // Increment the layer variable for next write.
    layer++;

	TIFFClose(in);
}
