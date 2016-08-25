#include "stdafx.h"
#include <tiffio.h>

#include "tiff.hpp"

static int ignore = false;		/* if true, ignore read errors */

static int cpContig2ContigByRow(TIFF* in, TIFF* out,
                                uint32 imagelength, uint32 imagewidth,
                                tsample_t spp) {
	tsize_t scSize = TIFFScanlineSize(in);
	tdata_t buf;
	uint32 row;

	buf = _TIFFmalloc(scSize);
	if (!buf)
		return 0;
	_TIFFmemset(buf, 0, scSize);
	(void) imagewidth; (void) spp;
	for (row = 0; row < imagelength; row++) {
		if (TIFFReadScanline(in, buf, row, 0) < 0 && !ignore) {
			TIFFError(TIFFFileName(in),
				  "Error, can't read scanline %lu",
				  (unsigned long) row);
			goto bad;
		}
		if (TIFFWriteScanline(out, buf, row, 0) < 0) {
			TIFFError(TIFFFileName(out),
				  "Error, can't write scanline %lu",
				  (unsigned long) row);
			goto bad;
		}
	}
	_TIFFfree(buf);
	return 1;
bad:
	_TIFFfree(buf);
	return 0;
}

/*
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
    std::cerr << "PLANARCONFIG " << shortv << std::endl;
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

    return cpContig2ContigByRow(in, out, l, w, samplesperpixel);
}
*/

#define	CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) TIFFSetField(out, tag, v)
#define	CopyField2(tag, v1, v2) \
    if (TIFFGetField(in, tag, &v1, &v2)) TIFFSetField(out, tag, v1, v2)
#define	CopyField3(tag, v1, v2, v3) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3)) TIFFSetField(out, tag, v1, v2, v3)
#define	CopyField4(tag, v1, v2, v3, v4) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3, &v4)) TIFFSetField(out, tag, v1, v2, v3, v4)

static void
cpTag(TIFF* in, TIFF* out, uint16 tag, uint16 count, TIFFDataType type)
{
	switch (type) {
	case TIFF_SHORT:
		if (count == 1) {
			uint16 shortv;
			CopyField(tag, shortv);
		} else if (count == 2) {
			uint16 shortv1, shortv2;
			CopyField2(tag, shortv1, shortv2);
		} else if (count == 4) {
			uint16 *tr, *tg, *tb, *ta;
			CopyField4(tag, tr, tg, tb, ta);
		} else if (count == (uint16) -1) {
			uint16 shortv1;
			uint16* shortav;
			CopyField2(tag, shortv1, shortav);
		}
		break;
	case TIFF_LONG:
		{ uint32 longv;
		  CopyField(tag, longv);
		}
		break;
	case TIFF_RATIONAL:
		if (count == 1) {
			float floatv;
			CopyField(tag, floatv);
		} else if (count == (uint16) -1) {
			float* floatav;
			CopyField(tag, floatav);
		}
		break;
	case TIFF_ASCII:
		{ char* stringv;
		  CopyField(tag, stringv);
		}
		break;
	case TIFF_DOUBLE:
		if (count == 1) {
			double doublev;
			CopyField(tag, doublev);
		} else if (count == (uint16) -1) {
			double* doubleav;
			CopyField(tag, doubleav);
		}
		break;
	default:
		TIFFError(TIFFFileName(in),
		    "Data type %d is not supported, tag %d skipped.",
		    tag, type);
	}
}

static struct cpTag {
	uint16 tag;
	uint16 count;
	TIFFDataType type;
} tags[] = {
	{ TIFFTAG_SUBFILETYPE,		1, TIFF_LONG },
	{ TIFFTAG_THRESHHOLDING,	1, TIFF_SHORT },
	{ TIFFTAG_DOCUMENTNAME,		1, TIFF_ASCII },
	{ TIFFTAG_IMAGEDESCRIPTION,	1, TIFF_ASCII },
	{ TIFFTAG_MAKE,			1, TIFF_ASCII },
	{ TIFFTAG_MODEL,		1, TIFF_ASCII },
	{ TIFFTAG_MINSAMPLEVALUE,	1, TIFF_SHORT },
	{ TIFFTAG_MAXSAMPLEVALUE,	1, TIFF_SHORT },
	{ TIFFTAG_XRESOLUTION,		1, TIFF_RATIONAL },
	{ TIFFTAG_YRESOLUTION,		1, TIFF_RATIONAL },
	{ TIFFTAG_PAGENAME,		1, TIFF_ASCII },
	{ TIFFTAG_XPOSITION,		1, TIFF_RATIONAL },
	{ TIFFTAG_YPOSITION,		1, TIFF_RATIONAL },
	{ TIFFTAG_RESOLUTIONUNIT,	1, TIFF_SHORT },
	{ TIFFTAG_SOFTWARE,		1, TIFF_ASCII },
	{ TIFFTAG_DATETIME,		1, TIFF_ASCII },
	{ TIFFTAG_ARTIST,		1, TIFF_ASCII },
	{ TIFFTAG_HOSTCOMPUTER,		1, TIFF_ASCII },
	{ TIFFTAG_WHITEPOINT,		(uint16) -1, TIFF_RATIONAL },
	{ TIFFTAG_PRIMARYCHROMATICITIES,(uint16) -1,TIFF_RATIONAL },
	{ TIFFTAG_HALFTONEHINTS,	2, TIFF_SHORT },
	{ TIFFTAG_INKSET,		1, TIFF_SHORT },
	{ TIFFTAG_DOTRANGE,		2, TIFF_SHORT },
	{ TIFFTAG_TARGETPRINTER,	1, TIFF_ASCII },
	{ TIFFTAG_SAMPLEFORMAT,		1, TIFF_SHORT },
	{ TIFFTAG_YCBCRCOEFFICIENTS,	(uint16) -1,TIFF_RATIONAL },
	{ TIFFTAG_YCBCRSUBSAMPLING,	2, TIFF_SHORT },
	{ TIFFTAG_YCBCRPOSITIONING,	1, TIFF_SHORT },
	{ TIFFTAG_REFERENCEBLACKWHITE,	(uint16) -1,TIFF_RATIONAL },
	{ TIFFTAG_EXTRASAMPLES,		(uint16) -1, TIFF_SHORT },
	{ TIFFTAG_SMINSAMPLEVALUE,	1, TIFF_DOUBLE },
	{ TIFFTAG_SMAXSAMPLEVALUE,	1, TIFF_DOUBLE },
	{ TIFFTAG_STONITS,		1, TIFF_DOUBLE },
};
#define	NTAGS	(sizeof (tags) / sizeof (tags[0]))

#define	CopyTag(tag, count, type)	cpTag(in, out, tag, count, type)

static int
cpTiff(TIFF* in, TIFF* out, const unsigned short layer)
{
	uint16 bitspersample, samplesperpixel;
	uint16 compression, config, orientation;
	uint32 width, length, rowsperstrip;
	struct cpTag* p;

	CopyField(TIFFTAG_IMAGEWIDTH, width);
	CopyField(TIFFTAG_IMAGELENGTH, length);
	CopyField(TIFFTAG_BITSPERSAMPLE, bitspersample);
	CopyField(TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
	CopyField(TIFFTAG_COMPRESSION, compression);
	CopyTag(TIFFTAG_PHOTOMETRIC, 1, TIFF_SHORT);
	CopyTag(TIFFTAG_FILLORDER, 1, TIFF_SHORT);
	/*
	 * Will copy `Orientation' tag from input image
	 */
	TIFFGetFieldDefaulted(in, TIFFTAG_ORIENTATION, &orientation);
	switch (orientation) {
		case ORIENTATION_BOTRIGHT:
		case ORIENTATION_RIGHTBOT:	/* XXX */
			TIFFWarning(TIFFFileName(in), "using bottom-left orientation");
			orientation = ORIENTATION_BOTLEFT;
		/* fall thru... */
		case ORIENTATION_LEFTBOT:	/* XXX */
		case ORIENTATION_BOTLEFT:
			break;
		case ORIENTATION_TOPRIGHT:
		case ORIENTATION_RIGHTTOP:	/* XXX */
		default:
			TIFFWarning(TIFFFileName(in), "using top-left orientation");
			orientation = ORIENTATION_TOPLEFT;
		/* fall thru... */
		case ORIENTATION_LEFTTOP:	/* XXX */
		case ORIENTATION_TOPLEFT:
			break;
	}
	TIFFSetField(out, TIFFTAG_ORIENTATION, orientation);
	/*
	 * Choose tiles/strip for the output image according to
	 * the command line arguments (-tiles, -strips) and the
	 * structure of the input image.
	 */
	{
		/*
		 * RowsPerStrip is left unspecified: use either the
		 * value from the input image or, if nothing is defined,
		 * use the library default.
		 */
		if (!TIFFGetField(in, TIFFTAG_ROWSPERSTRIP, &rowsperstrip)) {
			rowsperstrip = TIFFDefaultStripSize(out, rowsperstrip);
		}
		if (rowsperstrip > length && rowsperstrip != (uint32)-1) {
			rowsperstrip = length;
        }
		TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
	}
	CopyField(TIFFTAG_PLANARCONFIG, config);
	if (samplesperpixel <= 4)
		CopyTag(TIFFTAG_TRANSFERFUNCTION, 4, TIFF_SHORT);
	CopyTag(TIFFTAG_COLORMAP, 4, TIFF_SHORT);
	{
		uint32 len32;
		void** data;
		if (TIFFGetField(in, TIFFTAG_ICCPROFILE, &len32, &data))
			TIFFSetField(out, TIFFTAG_ICCPROFILE, len32, data);
	}
	{
		uint16 ninks;
		const char* inknames;
		if (TIFFGetField(in, TIFFTAG_NUMBEROFINKS, &ninks)) {
			TIFFSetField(out, TIFFTAG_NUMBEROFINKS, ninks);
			if (TIFFGetField(in, TIFFTAG_INKNAMES, &inknames)) {
				int inknameslen = strlen(inknames) + 1;
				const char* cp = inknames;
				while (ninks > 1) {
					cp = strchr(cp, '\0');
                                        cp++;
                                        inknameslen += (strlen(cp) + 1);
					ninks--;
				}
				TIFFSetField(out, TIFFTAG_INKNAMES, inknameslen, inknames);
			}
		}
	}
	{
		TIFFSetField(out, TIFFTAG_PAGENUMBER, layer, 0);
	}

	for (p = tags; p < &tags[NTAGS]; p++)
		CopyTag(p->tag, p->count, p->type);

	return cpContig2ContigByRow(in, out, length, width, samplesperpixel);
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
    static unsigned short layer = 0;

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
