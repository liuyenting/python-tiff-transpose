#include "stdafx.h"
#include <tiffio.h>

#include "tiff.hpp"

#ifdef DEBUG
#define ERR_SHOW true
#else
#define ERR_SHOW false
#endif

static int cpData(TIFF *in, TIFF *out, uint32_t nrow) {
	tsize_t scSize = TIFFScanlineSize(in);
	tdata_t buf = _TIFFmalloc(scSize);
	if (!buf)
		return 0;
	_TIFFmemset(buf, 0, scSize);
	for (uint32_t irow = 0; irow < nrow; irow++) {
        try {
    		if (TIFFReadScanline(in, buf, irow, 0) < 0) {
    			throw -1;
    		}
    		if (TIFFWriteScanline(out, buf, irow, 0) < 0) {
    			throw -2;
    		}
        } catch (int err) {
            if (err == -1) {
                std::cerr << "Cannot read scanline " << irow;
                std::cerr << " from " << TIFFFileName(in) << std::endl;
            } else if (err == -2) {
                std::cerr << "Cannot write scanline " << irow;
                std::cerr << " to " << TIFFFileName(out) << std::endl;
            } else {
                std::cerr << "Unknown error during row copy" << std::endl;
            }
            _TIFFfree(buf);
            return 0;
        }
	}
	_TIFFfree(buf);
	return 1;
}

#define	CopyField(tag, v) \
    if (TIFFGetField(in, tag, &v)) \
        TIFFSetField(out, tag, v)
#define	CopyField2(tag, v1, v2) \
    if (TIFFGetField(in, tag, &v1, &v2)) \
        TIFFSetField(out, tag, v1, v2)
#define	CopyField3(tag, v1, v2, v3) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3)) \
        TIFFSetField(out, tag, v1, v2, v3)
#define	CopyField4(tag, v1, v2, v3, v4) \
    if (TIFFGetField(in, tag, &v1, &v2, &v3, &v4)) \
        TIFFSetField(out, tag, v1, v2, v3, v4)

#define CP_BY_PTR (uint16_t)(-1)

static void cpTag(TIFF* in, TIFF* out,
                  uint16_t tag, uint16_t count, TIFFDataType type) {
	switch (type) {
	case TIFF_SHORT:
		if (count == 1) {
			uint16_t shortv;
			CopyField(tag, shortv);
		} else if (count == 2) {
			uint16_t shortv1, shortv2;
			CopyField2(tag, shortv1, shortv2);
		} else if (count == 4) {
			uint16_t *tr, *tg, *tb, *ta;
			CopyField4(tag, tr, tg, tb, ta);
		} else if (count == CP_BY_PTR) {
			uint16_t shortv1;
			uint16_t* shortav;
			CopyField2(tag, shortv1, shortav);
		}
		break;
	case TIFF_LONG:
		uint32_t longv;
		CopyField(tag, longv);
		break;
	case TIFF_RATIONAL:
		if (count == 1) {
			float floatv;
			CopyField(tag, floatv);
		} else if (count == CP_BY_PTR) {
			float* floatav;
			CopyField(tag, floatav);
		}
		break;
	case TIFF_ASCII:
		char* stringv;
		CopyField(tag, stringv);
		break;
	case TIFF_DOUBLE:
		if (count == 1) {
			double doublev;
			CopyField(tag, doublev);
		} else if (count == CP_BY_PTR) {
			double* doubleav;
			CopyField(tag, doubleav);
		}
		break;
	default:
        std::cerr << "Data type " << type << " is not supported, ";
        std::cerr << "tag " << tag << "skipped" << std::endl;
	}
}
#define	CopyTag(tag, count, type) cpTag(in, out, tag, count, type)

static struct defTagList {
	uint16_t tag;
	uint16_t count;
	TIFFDataType type;
} tags[] = {
    { TIFFTAG_ARTIST,		    1, TIFF_ASCII },
    { TIFFTAG_BITSPERSAMPLE,    1, TIFF_SHORT },
    { TIFFTAG_COLORMAP,         4, TIFF_SHORT },
    { TIFFTAG_COMPRESSION,      1, TIFF_SHORT },
    { TIFFTAG_DATETIME,		    1, TIFF_ASCII },
    { TIFFTAG_DOCUMENTNAME,		1, TIFF_ASCII },
    { TIFFTAG_DOTRANGE,		    2, TIFF_SHORT },
    { TIFFTAG_EXTRASAMPLES,	CP_BY_PTR, TIFF_SHORT },
    { TIFFTAG_FILLORDER,        1, TIFF_SHORT },
    { TIFFTAG_HALFTONEHINTS,    2, TIFF_SHORT },
    { TIFFTAG_HOSTCOMPUTER,		1, TIFF_ASCII },
    { TIFFTAG_IMAGEDESCRIPTION,	1, TIFF_ASCII },
	{ TIFFTAG_MAKE,			    1, TIFF_ASCII },
    { TIFFTAG_MAXSAMPLEVALUE,	1, TIFF_SHORT },
    { TIFFTAG_MINSAMPLEVALUE,	1, TIFF_SHORT },
	{ TIFFTAG_MODEL,		    1, TIFF_ASCII },
    { TIFFTAG_PAGENAME,		    1, TIFF_ASCII },
    { TIFFTAG_PHOTOMETRIC,      1, TIFF_SHORT },
    { TIFFTAG_PLANARCONFIG,     1, TIFF_SHORT },
    { TIFFTAG_PRIMARYCHROMATICITIES, CP_BY_PTR, TIFF_RATIONAL },
    { TIFFTAG_REFERENCEBLACKWHITE, CP_BY_PTR, TIFF_RATIONAL },
    { TIFFTAG_RESOLUTIONUNIT,   1, TIFF_SHORT },
    { TIFFTAG_SAMPLEFORMAT,	    1, TIFF_SHORT },
    { TIFFTAG_SMAXSAMPLEVALUE,	1, TIFF_DOUBLE },
    { TIFFTAG_SMINSAMPLEVALUE,	1, TIFF_DOUBLE },
	{ TIFFTAG_SOFTWARE,		    1, TIFF_ASCII },
    { TIFFTAG_STONITS,		    1, TIFF_DOUBLE },
    { TIFFTAG_SUBFILETYPE,		1, TIFF_LONG  },
    { TIFFTAG_TARGETPRINTER,    1, TIFF_ASCII },
	{ TIFFTAG_THRESHHOLDING,	1, TIFF_SHORT },
    { TIFFTAG_WHITEPOINT, CP_BY_PTR, TIFF_RATIONAL },
    { TIFFTAG_XPOSITION,		1, TIFF_RATIONAL },
    { TIFFTAG_XRESOLUTION,		1, TIFF_RATIONAL },
	{ TIFFTAG_YPOSITION,	    1, TIFF_RATIONAL },
	{ TIFFTAG_YRESOLUTION,		1, TIFF_RATIONAL },
};
#define	NTAGS (sizeof(tags) / sizeof(tags[0]))

static int cpTiff(TIFF* in, TIFF* out,
                  const uint16_t iLayer, const uint16_t nLayer)
{
	uint16_t samplesperpixel;
	uint16_t orientation;
	uint32_t width, length, rowsperstrip;

    // Image size and memory layout.
	CopyField(TIFFTAG_IMAGEWIDTH, width);
	CopyField(TIFFTAG_IMAGELENGTH, length);
    {
		/*
		 * RowsPerStrip is left unspecified: use either the
		 * value from the input image or, if nothing is defined,
		 * use the library default.
		 */
		if (!TIFFGetField(in, TIFFTAG_ROWSPERSTRIP, &rowsperstrip)) {
			rowsperstrip = TIFFDefaultStripSize(out, rowsperstrip);
		}
		if (rowsperstrip > length && rowsperstrip != (uint32_t)-1) {
			rowsperstrip = length;
        }
		TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
	}
    CopyField(TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
	if (samplesperpixel <= 4)
		CopyTag(TIFFTAG_TRANSFERFUNCTION, 4, TIFF_SHORT);

    // Image orientation (per page).
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

    // Colormap profile.
	{
		uint32_t len32;
		void** data;
		if (TIFFGetField(in, TIFFTAG_ICCPROFILE, &len32, &data))
			TIFFSetField(out, TIFFTAG_ICCPROFILE, len32, data);
	}
	{
		uint16_t ninks;
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

    // Page number.
	TIFFSetField(out, TIFFTAG_PAGENUMBER, iLayer, nLayer);

    // Rest of the tags, directly copy them. 
	for (struct defTagList *t = tags; t < &tags[NTAGS]; t++)
		CopyTag(t->tag, t->count,t->type);

	return cpData(in, out, length);
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
               const fs::path &imgPath,
               const uint16_t nLayer) {
	TIFF *in, *out;
    static unsigned short iLayer = 0;

	// Open the file.
	in = TIFFOpen(imgPath.string().c_str(), "r");
	if (in == NULL) {
		std::cerr << "Unable to read " << imgPath.filename() << std::endl;
		return;
	}

    // Identify the read mode.
	static char mode[3] = { 'x', 'b', 0 };
    mode[0] = (mode[0] == 'x') ? 'w' : 'a';
    mode[1] = (TIFFIsBigEndian(in)) ? 'b' : 'l';

    std::cout << "Layer " << iLayer << ", ";
    std::cout << ((mode[0] == 'a') ? "Append" : "Overwrite") << std::endl;

	// Iterate through the directories.
	int iFile = 0;
	do {
        std::string s = genPath(outdir, prefix, iFile);
		out = TIFFOpen(s.c_str(), mode);
		if (out == NULL) {
			std::cerr << "Unable to create output file" << std::endl;
            (void) TIFFClose(in);
            (void) TIFFClose(out);
			return;
		} else if (!cpTiff(in, out, iLayer, nLayer)) {
			std::cerr << "Unable to copy the layer" << std::endl;
            (void) TIFFClose(in);
            (void) TIFFClose(out);
			return;
		}
		TIFFClose(out);
		iFile++;
	} while (TIFFReadDirectory(in));

    // Increment the layer variable for next write.
    iLayer++;

	TIFFClose(in);
}
