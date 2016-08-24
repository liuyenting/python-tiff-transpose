#include "stdafx.h"
#include <tiffio.h>

#include "tiff.hpp"

#define	CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) TIFFSetField(out, tag, v)
#define	CopyField2(tag, v1, v2) \
    if (TIFFGetField(in, tag, &v1, &v2)) TIFFSetField(out, tag, v1, v2)
#define	CopyField3(tag, v1, v2, v3) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3)) TIFFSetField(out, tag, v1, v2, v3)

static int cpStrips(TIFF *in, TIFF *out)
{
    tmsize_t bufsize = TIFFStripSize(in);
	unsigned char *buf = (unsigned char *)_TIFFmalloc(bufsize);

	if (buf) {
		tstrip_t ns = TIFFNumberOfStrips(in);
		uint64 *bytecounts;

		if (!TIFFGetField(in, TIFFTAG_STRIPBYTECOUNTS, &bytecounts)) {
            std::cerr << "Strip bytes counts are missing" << std::endl;
            _TIFFfree(buf);
			return 0;
		}
		for (tstrip_t s = 0; s < ns; s++) {
			if (bytecounts[s] > (uint64)bufsize) {
				buf = (unsigned char *)_TIFFrealloc(buf,
                                                    (tmsize_t)bytecounts[s]);
				if (!buf) {
					return 0;
                }
				bufsize = (tmsize_t)bytecounts[s];
			}
            // Copy the strip.
			if ((TIFFReadRawStrip(in, s, buf, (tmsize_t)bytecounts[s]) < 0) ||
			    (TIFFWriteRawStrip(out, s, buf, (tmsize_t)bytecounts[s]) < 0)) {
				_TIFFfree(buf);
				return 0;
			}
		}
		_TIFFfree(buf);
		return 1;
	}
	return 0;
}

static int cpTiles(TIFF *in, TIFF *out)
{
    tmsize_t bufsize = TIFFTileSize(in);
	unsigned char *buf = (unsigned char *)_TIFFmalloc(bufsize);

	if (buf) {
		ttile_t nt = TIFFNumberOfTiles(in);
		uint64 *bytecounts;

		if (!TIFFGetField(in, TIFFTAG_TILEBYTECOUNTS, &bytecounts)) {
            std::cerr << "Tile bytes counts are missing" << std::endl;
            _TIFFfree(buf);
			return 0;
		}
		for (ttile_t t = 0; t < nt; t++) {
			if (bytecounts[t] > (uint64) bufsize) {
				buf = (unsigned char *)_TIFFrealloc(buf,
                                                    (tmsize_t)bytecounts[t]);
				if (!buf) {
					return 0;
                }
				bufsize = (tmsize_t)bytecounts[t];
			}
            // Copy the tile.
			if ((TIFFReadRawTile(in, t, buf, (tmsize_t)bytecounts[t]) < 0) ||
			    (TIFFWriteRawTile(out, t, buf, (tmsize_t)bytecounts[t]) < 0)) {
				_TIFFfree(buf);
				return 0;
			}
		}
		_TIFFfree(buf);
		return 1;
	}
	return 0;
}

static int cpTiff(TIFF *in, TIFF *out) {
    uint16 bitspersample, samplesperpixel, compression, shortv, *shortav;
	uint32 w, l;
	float floatv;
	char *stringv;
	uint32 longv;

	CopyField(TIFFTAG_SUBFILETYPE, longv);
	CopyField(TIFFTAG_TILEWIDTH, w);
	CopyField(TIFFTAG_TILELENGTH, l);
	CopyField(TIFFTAG_IMAGEWIDTH, w);
	CopyField(TIFFTAG_IMAGELENGTH, l);
	CopyField(TIFFTAG_BITSPERSAMPLE, bitspersample);
	CopyField(TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
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
	CopyField(TIFFTAG_XPOSITION, floatv);
	CopyField(TIFFTAG_YPOSITION, floatv);
	CopyField(TIFFTAG_IMAGEDEPTH, longv);
	CopyField(TIFFTAG_TILEDEPTH, longv);
	CopyField(TIFFTAG_SAMPLEFORMAT, shortv);
	CopyField2(TIFFTAG_EXTRASAMPLES, shortv, shortav);
	{
        uint16 *red, *green, *blue;
        CopyField3(TIFFTAG_COLORMAP, red, green, blue);
	}
	{
        uint16 shortv2;
        CopyField2(TIFFTAG_PAGENUMBER, shortv, shortv2);
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
	CopyField(TIFFTAG_BADFAXLINES, longv);
	CopyField(TIFFTAG_CLEANFAXDATA, longv);
	CopyField(TIFFTAG_CONSECUTIVEBADFAXLINES, longv);
	CopyField(TIFFTAG_FAXRECVPARAMS, longv);
	CopyField(TIFFTAG_FAXRECVTIME, longv);
	CopyField(TIFFTAG_FAXSUBADDRESS, stringv);
	CopyField(TIFFTAG_FAXDCS, stringv);
	if (TIFFIsTiled(in)) {
		return cpTiles(in, out);
	} else {
		return cpStrips(in, out);
    }
}

std::string genPath(const fs::path &outdir, const std::string &prefix,
                    const int idx) {
    fs::path fname(prefix + std::to_string(idx) + ".tif");
    fs::path fpath = outdir / fname;
	return fpath.string();
}

/*
 * Deal the stacks into separated files and append them.
 */
void dealStack(const fs::path &outdir, const std::string &prefix,
               const fs::path &p) {
	TIFF *in, *out;

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

	// Iterate through the directories.
	int idx = 1;
	do {
        std::string s = genPath(outdir, prefix, idx);
		out = TIFFOpen(s.c_str(), mode);
		if (out == NULL) {
			std::cerr << "Unable to create output file" << std::endl;
			return;
		} else if (!cpTiff(in, out)) {
			std::cerr << "Unable to copy the layer" << std::endl;
			return;
		}
		TIFFClose(out);

		idx++;
	} while (TIFFReadDirectory(in));

	TIFFClose(in);
}
