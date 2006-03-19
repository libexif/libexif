/*

Copyright © 2000 Matthias Wandel, The PHP Group, Curtis Galloway

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <sys/time.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <math.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <fcntl.h>

#include "exif.h"

typedef unsigned char uchar;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* 
   This structure stores global state for an EXIF image file.
*/
typedef struct {
	exif_data_t *d;
	int MotorolaOrder;
	const char *filename;

	char *Thumbnail;
	int ThumbnailSize;
} ImageInfoType;

void *(*exif_malloc_fn)(int);
void *(*exif_realloc_fn)(void *, int);
void (*exif_free_fn)(void *);

static char *
exif_strndup(char *str, int len)
{
	char *rval = (*exif_malloc_fn)(len+1);
	strncpy(rval, str, len);
	rval[len] = '\0';
	return rval;
}

struct exif_data *
exif_alloc(void)
{
	exif_data_t *d;

	d = (*exif_malloc_fn)(sizeof(exif_data_t));
	bzero(d, sizeof(*d));
	return d;
}

static void
exif_error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}


/* This structure is used to store a section of a Jpeg file. */
typedef struct {
    uchar *Data;
    int      Type;
    unsigned Size;
} Section_t;

#define EXIT_FAILURE  1
#define EXIT_SUCCESS  0


/* 
   JPEG markers consist of one or more 0xFF bytes, followed by a marker
   code byte (which is not an FF).  Here are the marker codes of interest
   in this program.  (See jdmarker.c for a more complete list.)
*/

#define M_SOF0  0xC0            /* Start Of Frame N                        */
#define M_SOF1  0xC1            /* N indicates which compression process   */
#define M_SOF2  0xC2            /* Only SOF0-SOF2 are now in common use    */
#define M_SOF3  0xC3
#define M_SOF5  0xC5            /* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8            /* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9            /* End Of Image (end of datastream)         */
#define M_SOS   0xDA            /* Start Of Scan (begins compressed data)   */
#define M_EXIF  0xE1            
#define M_COM   0xFE            /* COMment                                  */


#define PSEUDO_IMAGE_MARKER 0x123; /* Extra value. */

#define EXIF_ALLOC_SIZE 16

/*
 * The name gets copied, so you can pass a static string;
 * the data is not copied, so if it is a string,
 * you must allocate it yourself.
 */
static int
exif_append_data(exif_data_t **d_p,
				 char *name,
				 char rec_type,
				 int exif_format,
				 exif_rec_data_t *data)
{
	exif_data_t *d = *d_p;

	if (rec_type == '\0')
		return EXIT_FAILURE;

	if (d->n_alloc <= d->n_recs) {
		d->n_alloc += EXIF_ALLOC_SIZE;
		d = (*exif_realloc_fn)(d, sizeof(exif_data_t) +
							   sizeof(exif_record_t) * d->n_alloc);
		*d_p = d;
	}
	d->recs[d->n_recs].rec_type = rec_type;
	bcopy(data, &d->recs[d->n_recs].rec_data, sizeof(exif_rec_data_t));
	d->recs[d->n_recs].rec_name = strdup(name);
	d->n_recs++;
	return EXIT_SUCCESS;
}

/*
   Get 16 bits motorola order (always) for jpeg header stuff.
*/
static int
Get16m(void *Short)
{
    return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
}


/*
   Process a COM marker.
   We want to print out the marker contents as legible text;
   we must guard against random junk and varying newline representations.
*/
static void
process_COM (ImageInfoType *ImageInfo, uchar *Data, int length)
{
    int ch;
    char *Comment;
    int nch;
    int a;
    exif_rec_data_t rd;

    nch = 0;
    Comment = (*exif_malloc_fn)(length+1);

    for (a=2;a<length;a++) {
        ch = Data[a];

        if (ch == '\r' && Data[a+1] == '\n') continue; /* Remove cr followed by lf. */

        if (isprint(ch) || ch == '\n' || ch == '\t') {
            Comment[nch++] = (char)ch;
        } else {
            Comment[nch++] = '?';
        }
    }

    Comment[nch] = '\0'; /* Null terminate */

    rd.s = Comment;
    exif_append_data(&ImageInfo->d, "Comment", 's', EXIF_FMT_COMPUTED, &rd);
}
 
/* Process a SOFn marker.  This is useful for the image dimensions. */
static void
process_SOFn (ImageInfoType *ImageInfo, uchar *Data, int marker)
{
    int data_precision, num_components;
    const char *process;
    exif_rec_data_t rd;

    data_precision = Data[2];
    rd.l = Get16m(Data+3);
    exif_append_data(&ImageInfo->d,
					 "Height",
					 'l',
					 EXIF_FMT_COMPUTED,
					 &rd);
    rd.l = Get16m(Data+5);
    exif_append_data(&ImageInfo->d,
					 "Width",
					 'l',
					 EXIF_FMT_COMPUTED,
					 &rd);
    num_components = Data[7];

    if (num_components == 3) {
		rd.l = 1;
    } else {
		rd.l = 0;
    }
    exif_append_data(&ImageInfo->d, "IsColor", 'l', EXIF_FMT_COMPUTED, &rd);

    switch (marker) {
	case M_SOF0:  process = "Baseline";  break;
	case M_SOF1:  process = "Extended sequential";  break;
	case M_SOF2:  process = "Progressive";  break;
	case M_SOF3:  process = "Lossless";  break;
	case M_SOF5:  process = "Differential sequential";  break;
	case M_SOF6:  process = "Differential progressive";  break;
	case M_SOF7:  process = "Differential lossless";  break;
	case M_SOF9:  process = "Extended sequential, arithmetic coding";  break;
	case M_SOF10: process = "Progressive, arithmetic coding";  break;
	case M_SOF11: process = "Lossless, arithmetic coding";  break;
	case M_SOF13: process = "Differential sequential, arithmetic coding";  break;
	case M_SOF14: process = "Differential progressive, arithmetic coding"; break;
	case M_SOF15: process = "Differential lossless, arithmetic coding";  break;
	default:      process = "Unknown";  break;
    }
}

/*
   Describes format descriptor
*/
static int ExifBytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};
#define NUM_FORMATS EXIF_FMT_DOUBLE

/*
   Describes tag values
*/

#define TAG_EXIF_OFFSET       0x8769
#define TAG_INTEROP_OFFSET    0xa005

#define TAG_COMPRESSION       0x0103

#define TAG_MAKE              0x010F
#define TAG_MODEL             0x0110
#define TAG_ORIENTATION       0x0112

#define TAG_SOFTWARE          0x0131

/* Olympus specific tags */
#define TAG_SPECIALMODE       0x0200
#define TAG_JPEGQUAL          0x0201
#define TAG_MACRO             0x0202
#define TAG_DIGIZOOM          0x0204
#define TAG_SOFTWARERELEASE   0x0207
#define TAG_PICTINFO          0x0208
#define TAG_CAMERAID          0x0209
/* end Olympus specific tags */

#define TAG_COPYRIGHT         0x8298

#define TAG_EXPOSURETIME      0x829A
#define TAG_FNUMBER           0x829D

#define TAG_GPSINFO           0x8825
#define TAG_ISOSPEED          0x8827
#define TAG_EXIFVERSION       0x9000

#define TAG_SHUTTERSPEED      0x9201
#define TAG_APERTURE          0x9202
#define TAG_MAXAPERTURE       0x9205
#define TAG_FOCALLENGTH       0x920A

#define TAG_DATETIME_ORIGINAL 0x9003
#define TAG_USERCOMMENT       0x9286

#define TAG_SUBJECT_DISTANCE  0x9206
#define TAG_LIGHT_SOURCE      0x9208
#define TAG_FLASH             0x9209

#define TAG_FOCALPLANEXRES    0xa20E
#define TAG_FOCALPLANEUNITS   0xa210
#define TAG_IMAGEWIDTH        0xA002

struct ExifTag {
    unsigned short Tag;
    char *Desc;
	void (*Func)();
};



/* Convert a 16 bit unsigned value from file's native byte order */
static int
Get16u(void *Short, int MotorolaOrder)
{
    if (MotorolaOrder) {
        return (((uchar *)Short)[0] << 8) | ((uchar *)Short)[1];
    } else {
        return (((uchar *)Short)[1] << 8) | ((uchar *)Short)[0];
    }
}

/* Convert a 32 bit signed value from file's native byte order */
static int
Get32s(void *Long, int MotorolaOrder)
{
    if (MotorolaOrder) {
        return  ((( char *)Long)[0] << 24) | (((uchar *)Long)[1] << 16)
			| (((uchar *)Long)[2] << 8 ) | (((uchar *)Long)[3] << 0 );
    } else {
        return  ((( char *)Long)[3] << 24) | (((uchar *)Long)[2] << 16)
			| (((uchar *)Long)[1] << 8 ) | (((uchar *)Long)[0] << 0 );
    }
}

/* Convert a 32 bit unsigned value from file's native byte order */
static unsigned
Get32u(void *Long, int MotorolaOrder)
{
    return (unsigned)Get32s(Long, MotorolaOrder) & 0xffffffff;
}


/* Evaluate number, be it int, rational, or float from directory. */
static double
ConvertAnyFormat(void *ValuePtr, int Format, int MotorolaOrder)
{
    double Value;
    Value = 0;

    switch(Format) {
	case EXIF_FMT_SBYTE:     Value = *(signed char *)ValuePtr;  break;
	case EXIF_FMT_BYTE:      Value = *(uchar *)ValuePtr;        break;

	case EXIF_FMT_USHORT:    Value = Get16u(ValuePtr,MotorolaOrder);          break;
	case EXIF_FMT_ULONG:     Value = Get32u(ValuePtr,MotorolaOrder);          break;

	case EXIF_FMT_URATIONAL:
	case EXIF_FMT_SRATIONAL: 
		{
			int Num,Den;
			Num = Get32s(ValuePtr,MotorolaOrder);
			Den = Get32s(4+(char *)ValuePtr,MotorolaOrder);
			if (Den == 0) {
				Value = 0;
			} else {
				Value = (double)Num/Den;
			}
			break;
		}

	case EXIF_FMT_SSHORT:    Value = (signed short)Get16u(ValuePtr,MotorolaOrder);  break;
	case EXIF_FMT_SLONG:     Value = Get32s(ValuePtr,MotorolaOrder);                break;

        /* Not sure if this is correct (never seen float used in Exif format) */
	case EXIF_FMT_SINGLE:    Value = (double)*(float *)ValuePtr;      break;
	case EXIF_FMT_DOUBLE:    Value = *(double *)ValuePtr;             break;
    }
    return Value;
}

/* Evaluate number, be it int, rational, or float from directory. */
static char
ConvertAnyFormat2(void *ValuePtr, int ByteCount, int Format, int MotorolaOrder, exif_rec_data_t *data_p)
{
	char *str, *p;
	char r_type;
	unsigned char c;
	static char hexdigits[] = "0123456789ABCDEF";

    switch(Format) {
	case EXIF_FMT_STRING:
		data_p->s = exif_strndup(ValuePtr, ByteCount);
		r_type = 's';
		break;

	case EXIF_FMT_SBYTE:
		data_p->l = (long)*(signed char *)ValuePtr;
		r_type = 'l';
		break;

	case EXIF_FMT_BYTE:
		data_p->l = (long)*(uchar *)ValuePtr;
		r_type = 'l';
        break;

	case EXIF_FMT_USHORT:
		data_p->l = (long)Get16u(ValuePtr,MotorolaOrder);
		r_type = 'l';
		break;
	case EXIF_FMT_ULONG:
		data_p->l = (long)Get32u(ValuePtr,MotorolaOrder);
		r_type = 'l';
		break;

	case EXIF_FMT_URATIONAL:
	case EXIF_FMT_SRATIONAL: 
		{
			int Num,Den;
			data_p->r.num = Get32s(ValuePtr,MotorolaOrder);
			data_p->r.denom = Get32s(4+(char *)ValuePtr,MotorolaOrder);
			r_type = 'r';
			break;
		}

	case EXIF_FMT_SSHORT:
		data_p->l = (signed short)Get16u(ValuePtr,MotorolaOrder);
		r_type = 'l';
		break;
	case EXIF_FMT_SLONG:
		data_p->l = (long)Get32s(ValuePtr,MotorolaOrder);
		r_type = 'l';
		break;

        /* Not sure if this is correct (never seen float used in Exif format) */
	case EXIF_FMT_SINGLE:
		data_p->f = *(float *)ValuePtr;
		r_type = 'f';
		break;

	case EXIF_FMT_DOUBLE:
		data_p->g = *(double *)ValuePtr;
		r_type = 'f';
		break;

	default:
		/* unknown type */
		p = str = (*exif_malloc_fn)(ByteCount*2 + 1);
		while (ByteCount--) {
			c = *(unsigned char *)ValuePtr++;
			*p++ = hexdigits[c / 16];
			*p++ = hexdigits[c % 16];
		}
		*p++ = '\0';
		data_p->s = str;
		r_type = 's';
		break;
    }
    return r_type;
}


static void
ProcessFocalPlaneUnits(ImageInfoType *ImageInfo,
					   void *ValuePtr,
					   int ByteCount,
					   int Format,
					   struct ExifTag *tag_p)
{
    exif_rec_data_t rd;
	float FocalPlaneUnits;

	switch((int)ConvertAnyFormat(ValuePtr, Format, ImageInfo->MotorolaOrder)) {
	case 1:
		FocalPlaneUnits = 25.4;
		break; /* inch */
	case 2: 
		/* According to the information I was using, 2 means meters.
		   But looking at the Canon PowerShot's files, inches is the only
		   sensible value. */
		FocalPlaneUnits = 25.4;
		break;

	case 3:
		FocalPlaneUnits = 10;
		break;  /* centimeter */
	case 4:
		FocalPlaneUnits = 1;
		break;  /* milimeter  */
	case 5:
		FocalPlaneUnits = .001;
		break;  /* micrometer */
	}

	rd.f = FocalPlaneUnits;
	exif_append_data(&ImageInfo->d,
					 "FocalPlaneUnits",
					 'f',
					 Format,
					 &rd);
}

static void
ProcessVersion(ImageInfoType *ImageInfo,
				   void *ValuePtr,
				   int ByteCount,
				   int Format,
				   struct ExifTag *tag_p)
{
    exif_rec_data_t rd;
	rd.s = exif_strndup(ValuePtr, ByteCount);
	exif_append_data(&ImageInfo->d,
					 tag_p->Desc,
					 's',
					 Format,
					 &rd);
}

static void
ProcessUserComment(ImageInfoType *ImageInfo,
				   void *_ValuePtr,
				   int ByteCount,
				   int Format,
				   struct ExifTag *tag_p)
{
	char *ValuePtr = (char *)_ValuePtr;
    exif_rec_data_t rd;
	int a;

	/* Olympus has this padded with trailing spaces.  Remove these first. */
	for (a=ByteCount;;) {
		a--;
		if ((ValuePtr)[a] == ' ') {
			(ValuePtr)[a] = '\0';
		} else {
			break;
		}
		if (a == 0) break;
	}

	/* Copy the comment */
	if (memcmp(ValuePtr, "ASCII",5) == 0) {
		for (a=5;a<10;a++) {
			int c;
			c = (ValuePtr)[a];
			if (c != '\0' && c != ' ') {
				rd.s = exif_strndup(a+ValuePtr, ByteCount - a);
				exif_append_data(&ImageInfo->d,
								 "UserComment",
								 's',
								 Format,
								 &rd);
				break;
			}
		}
                    
	} else {
		rd.s = exif_strndup(ValuePtr, ByteCount);
		exif_append_data(&ImageInfo->d,
						 "UserComment",
						 's',
						 Format,
						 &rd);
	}
}

static void
ProcessShutterSpeed(ImageInfoType *ImageInfo,
					void *ValuePtr,
					int ByteCount,
					int Format,
					struct ExifTag *tag_p)
{
    exif_rec_data_t rd;
	char rec_type;

	rec_type = ConvertAnyFormat2(ValuePtr, ByteCount, Format,
								 ImageInfo->MotorolaOrder,
								 &rd);
	exif_append_data(&ImageInfo->d,
					 tag_p->Desc,
					 rec_type,
					 Format,
					 &rd);

	/* Convert shutter speed value to shutter speed;
	 * shutter speed is 1/(2**ShutterSpeedValue)
     */
	rd.r.denom = (int)pow(2.0, ((double)rd.r.num)/((double)rd.r.denom));
	rd.r.num = 1;
	exif_append_data(&ImageInfo->d,
					 "ShutterSpeed",
					 'r',
					 EXIF_FMT_COMPUTED,
					 &rd);
	
}

static void
ProcessAperture(ImageInfoType *ImageInfo,
				void *ValuePtr,
				int ByteCount,
				int Format,
				struct ExifTag *tag_p)
{
    exif_rec_data_t rd;
	char rec_type;
	double fstop;
	char label[32];

	rec_type = ConvertAnyFormat2(ValuePtr, ByteCount, Format,
								 ImageInfo->MotorolaOrder,
								 &rd);
	exif_append_data(&ImageInfo->d,
					 tag_p->Desc,
					 rec_type,
					 Format,
					 &rd);

	if (exif_find_record(ImageInfo->d, "FNumber") == NULL) {
		/* Convert aperture to F-stop. */
		fstop = pow(sqrt(2), ((double)rd.r.num)/((double)rd.r.denom));
		snprintf(label,32, "f%.1g", fstop);
		rd.s = strdup(label);
		exif_append_data(&ImageInfo->d,
						 "FNumber",
						 's',
						 EXIF_FMT_COMPUTED,
						 &rd);
	}
}

static void
ProcessCanonMakerNote(ImageInfoType *ImageInfo,
				 void *ValuePtr,
				 int ByteCount,
				 int Format,
				 struct ExifTag *tag_p,
				 char *OffsetBase)
{
	
	/* This is for the Canon MakerNote. */
	/* XXX - go by value of Maker tag. */
    exif_rec_data_t rd;
	char rec_type;
	unsigned long n_dir, tag, format, components, offset;
	char label[32];
	void *OffsetPtr;

	n_dir = Get16u(ValuePtr, ImageInfo->MotorolaOrder);
	ValuePtr += 2;
	while (n_dir--) {
		tag = Get16u(ValuePtr, ImageInfo->MotorolaOrder);
		ValuePtr += 2;
		format = Get16u(ValuePtr, ImageInfo->MotorolaOrder);
		ValuePtr += 2;
		components = Get32u(ValuePtr, ImageInfo->MotorolaOrder);
		ValuePtr += 4;
		offset = Get32u(ValuePtr, ImageInfo->MotorolaOrder);
        ByteCount = components * ExifBytesPerFormat[format];
		if (ByteCount > 4) {
			OffsetPtr = OffsetBase + offset;
		} else {
			OffsetPtr = ValuePtr;
		}
		ValuePtr += 4;
		rec_type = ConvertAnyFormat2(OffsetPtr, ByteCount, format,
									 ImageInfo->MotorolaOrder,
									 &rd);
		snprintf(label, 32, "MakerNote%04x", tag);
		exif_append_data(&ImageInfo->d,
						 label,
						 rec_type,
						 format,
						 &rd);
		
	}
}


struct MakerNote {
	char *Make;
	void (*Func)();
};

static struct MakerNote
MakerProcessors[] = {
	{"Canon", ProcessCanonMakerNote},
    {NULL, NULL}
};

static void
ProcessMakerNote(ImageInfoType *ImageInfo,
				 void *ValuePtr,
				 int ByteCount,
				 int Format,
				 struct ExifTag *tag_p,
				 char *OffsetBase)
{
	struct MakerNote *mn_p;
	exif_record_t *rec_p;

	rec_p = exif_find_record(ImageInfo->d, "Make");
	if (rec_p == NULL) {
		return;
	}

	for(mn_p = &MakerProcessors[0]; mn_p->Make != NULL; mn_p++) {
		if (strcmp(mn_p->Make, rec_p->rec_data.s) == 0) {
			(*mn_p->Func)(ImageInfo, ValuePtr, ByteCount, Format, tag_p, OffsetBase);
			break;
		}
	}
}

static struct ExifTag
TagTable[] = {
	{   	0x0001, "InteroperabilityIndex", NULL},
	{   	0x0002, "InteroperabilityVersion", ProcessVersion},
	{	0x0100,	"ImageWidth", NULL},
	{	0x0101,	"ImageLength", NULL},
	{	0x0102,	"BitsPerSample", NULL},
	{	0x0103,	"Compression", NULL},
	{	0x0106,	"PhotometricInterpretation", NULL},
	{	0x010A,	"FillOrder", NULL},
	{	0x010D,	"DocumentName", NULL},
	{	0x010E,	"ImageDescription", NULL},
	{	0x010F,	"Make", NULL},
	{	0x0110,	"Model", NULL},
	{	0x0111,	"StripOffsets", NULL},
	{	0x0112,	"Orientation", NULL},
	{	0x0115,	"SamplesPerPixel", NULL},
	{	0x0116,	"RowsPerStrip", NULL},
	{	0x0117,	"StripByteCounts", NULL},
	{	0x011A,	"XResolution", NULL},
	{	0x011B,	"YResolution", NULL},
	{	0x011C,	"PlanarConfiguration", NULL},
	{	0x0128,	"ResolutionUnit", NULL},
	{	0x012D,	"TransferFunction", NULL},
	{	0x0131,	"Software", NULL},
	{	0x0132,	"DateTime", NULL},
	{	0x013B,	"Artist", NULL},
	{	0x013E,	"WhitePoint", NULL},
	{	0x013F,	"PrimaryChromaticities", NULL},
	{	0x0156,	"TransferRange", NULL},
	{	0x0200,	"JPEGProc", NULL},
	{	0x0201,	"JPEGInterchangeFormat", NULL},
	{	0x0202,	"JPEGInterchangeFormatLength", NULL},
	{	0x0211,	"YCbCrCoefficients", NULL},
	{	0x0212,	"YCbCrSubSampling", NULL},
	{	0x0213,	"YCbCrPositioning", NULL},
	{	0x0214,	"ReferenceBlackWhite", NULL},
	{   	0x1000, "RelatedImageFileFormat", NULL},
	{   	0x1001, "RelatedImageWidth", NULL},
	{   	0x1002, "RelatedImageLength", NULL},
	{	0x828D,	"CFARepeatPatternDim", NULL},
	{	0x828E,	"CFAPattern", NULL},
	{	0x828F,	"BatteryLevel", NULL},
	{	0x8298,	"Copyright", NULL},
	{	0x829A,	"ExposureTime", NULL},
	{	0x829D,	"FNumber", NULL},
	{	0x83BB,	"IPTC/NAA", NULL},
	{	0x8769,	"ExifOffset", NULL},
	{	0x8773,	"InterColorProfile", NULL},
	{	0x8822,	"ExposureProgram", NULL},
	{	0x8824,	"SpectralSensitivity", NULL},
	{	0x8825,	"GPSInfo", NULL},
	{	0x8827,	"ISOSpeedRatings", NULL},
	{	0x8828,	"OECF", NULL},
	{	0x9000,	"ExifVersion", ProcessVersion},
	{	0x9003,	"DateTimeOriginal", NULL},
	{	0x9004,	"DateTimeDigitized", NULL},
	{	0x9101,	"ComponentsConfiguration", NULL},
	{	0x9102,	"CompressedBitsPerPixel", NULL},
	{	0x9201,	"ShutterSpeedValue", ProcessShutterSpeed},
	{	0x9202,	"ApertureValue", ProcessAperture},
	{	0x9203,	"BrightnessValue", NULL},
	{	0x9204,	"ExposureBiasValue", NULL},
	{	0x9205,	"MaxApertureValue", ProcessAperture},
	{	0x9206,	"SubjectDistance", NULL},
	{	0x9207,	"MeteringMode", NULL},
	{	0x9208,	"LightSource", NULL},
	{	0x9209,	"Flash", NULL},
	{	0x920A,	"FocalLength", NULL},
	{	0x927C,	"MakerNote", ProcessMakerNote},
	{	0x9286,	"UserComment", ProcessUserComment},
	{	0x9290,	"SubSecTime", NULL},
	{	0x9291,	"SubSecTimeOriginal", NULL},
	{	0x9292,	"SubSecTimeDigitized", NULL},
	{	0xA000,	"FlashPixVersion", ProcessVersion},
	{	0xA001,	"ColorSpace", NULL},
	{	0xA002,	"ExifImageWidth", NULL},
	{	0xA003,	"ExifImageLength", NULL},
	{	0xA005,	"InteroperabilityOffset", NULL},
	{	0xA20B,	"FlashEnergy", NULL},			        /* 0x920B in TIFF/EP */
	{	0xA20C,	"SpatialFrequencyResponse", NULL},	/* 0x920C    -  -    */
	{	0xA20E,	"FocalPlaneXResolution", NULL},    	/* 0x920E    -  -    */
	{	0xA20F,	"FocalPlaneYResolution", NULL},	    /* 0x920F    -  -    */
	{	0xA210,	"FocalPlaneResolutionUnit", ProcessFocalPlaneUnits},
	                                            /* 0x9210    -  -    */
	{	0xA214,	"SubjectLocation", NULL},		        /* 0x9214    -  -    */
	{	0xA215,	"ExposureIndex", NULL},		        /* 0x9215    -  -    */
	{	0xA217,	"SensingMethod", NULL},		        /* 0x9217    -  -    */
	{	0xA300,	"FileSource", NULL},
	{	0xA301,	"SceneType", NULL},
	{        0, NULL, NULL}
} ;



/* Process one of the nested EXIF directories. */
static int
ProcessExifDir(ImageInfoType *ImageInfo, char *DirStart, char *OffsetBase, unsigned ExifLength, char *LastExifRefd)
{
    int de;
    int NumDirEntries;
    exif_rec_data_t rd;
    char rec_type;
    char label[32];

    NumDirEntries = Get16u(DirStart, ImageInfo->MotorolaOrder);

    if ((DirStart+2+NumDirEntries*12) > (OffsetBase+ExifLength)) {
		exif_error("Illegally sized directory");
		return FALSE;
    }


    for (de=0;de<NumDirEntries;de++) {
        int Tag, Format, Components;
        char *ValuePtr;
        int ByteCount;
        char *DirEntry;
		struct ExifTag *tag_p;

        DirEntry = DirStart+2+12*de;

        Tag = Get16u(DirEntry, ImageInfo->MotorolaOrder);
        Format = Get16u(DirEntry+2, ImageInfo->MotorolaOrder);
        Components = Get32u(DirEntry+4, ImageInfo->MotorolaOrder);

        if ((Format-1) >= NUM_FORMATS) {
            /* (-1) catches illegal zero case as unsigned underflows to positive large. */
			exif_error("Illegal format code in EXIF dir");
			return FALSE;
        }

        ByteCount = Components * ExifBytesPerFormat[Format];

        if (ByteCount > 4) {
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8, ImageInfo->MotorolaOrder);
            /* If its bigger than 4 bytes, the dir entry contains an offset. */
            if (OffsetVal+ByteCount > ExifLength) {
                /* Bogus pointer offset and / or bytecount value */
				/*                printf("Offset %d bytes %d ExifLen %d\n",OffsetVal, ByteCount, ExifLength); */

				exif_error("Illegal pointer offset value in EXIF");
				return FALSE;
            }
            ValuePtr = OffsetBase+OffsetVal;
        } else {
            /* 4 bytes or less and value is in the dir entry itself */
            ValuePtr = DirEntry+8;
        }

        if (LastExifRefd < ValuePtr+ByteCount) {
            /* 
			   Keep track of last byte in the exif header that was actually referenced.
               That way, we know where the discardable thumbnail data begins.
			*/
            LastExifRefd = ValuePtr+ByteCount;
        }

        if (Tag == TAG_EXIF_OFFSET || Tag == TAG_INTEROP_OFFSET) {
            char *SubdirStart;
            SubdirStart = OffsetBase + Get32u(ValuePtr, ImageInfo->MotorolaOrder);
            if (SubdirStart < OffsetBase || SubdirStart > OffsetBase+ExifLength) {
				exif_error("Illegal subdirectory link");
				return FALSE;
            }
            ProcessExifDir(ImageInfo, SubdirStart, OffsetBase, ExifLength, LastExifRefd);
            continue;
        }

		/* Search through tag table */
		for (tag_p = &TagTable[0]; tag_p->Desc != NULL; tag_p++) {
			if (tag_p->Tag == Tag) {
				if (tag_p->Func != NULL) {
					(*tag_p->Func)(ImageInfo, ValuePtr, ByteCount, Format, tag_p, OffsetBase);
				} else {
					rec_type = ConvertAnyFormat2(ValuePtr, ByteCount, Format,
												 ImageInfo->MotorolaOrder,
												 &rd);
					exif_append_data(&ImageInfo->d,
									 tag_p->Desc,
									 rec_type,
									 Format,
									 &rd);
				}
				break;
			}
		}
		if (tag_p->Desc == NULL) {
			rec_type = ConvertAnyFormat2(ValuePtr, ByteCount, Format,
										 ImageInfo->MotorolaOrder,
										 &rd);
			snprintf(label, 32, "0x%04x", (unsigned int) Tag);
			exif_append_data(&ImageInfo->d,
							 label,
							 rec_type,
							 Format,
							 &rd);
		}
	}
    return TRUE;
}

/* 
   Process an EXIF marker
   Describes all the drivel that most digital cameras include...
*/
static int
process_EXIF (ImageInfoType *ImageInfo, char *CharBuf, unsigned int length, char *LastExifRefd)
{
	int cc;
	LastExifRefd = CharBuf;

	{   /* Check the EXIF header component */
		static const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
		if (memcmp(CharBuf+2, ExifHeader,6)) {
			exif_error("Incorrect Exif header");
			return FALSE;
		}
	}

	if (memcmp(CharBuf+8,"II",2) == 0) {
		ImageInfo->MotorolaOrder = 0;
	} else {
		if (memcmp(CharBuf+8,"MM",2) == 0) {
			ImageInfo->MotorolaOrder = 1;
		} else {
			exif_error("Invalid Exif alignment marker.");
			return FALSE;
		}
	}

	/* Check the next two values for correctness. */
	if (Get16u(CharBuf+10,ImageInfo->MotorolaOrder) != 0x2a
		|| Get32u(CharBuf+12,ImageInfo->MotorolaOrder) != 0x08) {
		exif_error("Invalid Exif start (1, NULL)");
		return FALSE;
	}

	/* First directory starts 16 bytes in.  Offsets start at 8 bytes in. */
	cc = ProcessExifDir(ImageInfo, CharBuf+16, CharBuf+8, length-6, LastExifRefd);
	if (cc != TRUE) {
		return cc;
	}
	return TRUE;
}
 
/* Parse the marker stream until SOS or EOI is seen; */
static int 
scan_JPEG_header (ImageInfoType *ImageInfo, FILE *infile, Section_t *Sections, int *SectionsRead, int ReadAll, char *LastExifRefd)
{
    int a;
    int HaveCom = FALSE;

    a = fgetc(infile);
    if (a != 0xff || fgetc(infile) != M_SOI) {
      return FALSE;
  }

    for(*SectionsRead=0;*SectionsRead < 19;) {
        int itemlen;
        int marker = 0;
        int ll,lh;
	size_t got;
        uchar *Data;

        for (a=0;a<7;a++) {
            marker = fgetc(infile);
            if (marker != 0xff) break;
        }
        if (marker == 0xff) {
            /* 0xff is legal padding, but if we get that many, something's wrong. */
			exif_error("too many padding bytes!");
			return FALSE;
        }

        Sections[*SectionsRead].Type = marker;
  
        /* Read the length of the section. */
        lh = fgetc(infile);
        ll = fgetc(infile);

        itemlen = (lh << 8) | ll;

        if (itemlen < 2) {
			exif_error("invalid marker");
			return FALSE;
        }

        Sections[*SectionsRead].Size = (unsigned int) itemlen;

        Data = (uchar *)(*exif_malloc_fn)(itemlen+1); /* Add 1 to allow sticking a 0 at the end. */
        Sections[*SectionsRead].Data = Data;

        /* Store first two pre-read bytes. */
        Data[0] = (uchar)lh;
        Data[1] = (uchar)ll;

        got = fread(Data+2, 1, (size_t) itemlen-2, infile); /* Read the whole section. */
        if ((int) got != itemlen-2) {
			exif_error("reading from file");
			return FALSE;
        }
        *SectionsRead += 1;

        switch(marker) {
		case M_SOS:   /* stop before hitting compressed data  */
			/* If reading entire image is requested, read the rest of the data. */
			if (ReadAll) {
			  	signed long cp, ep;
				unsigned int size;
				/* Determine how much file is left. */
				cp = ftell(infile);
				if (cp == -1) {
					exif_error ("ftell failed");
					return FALSE;
				}
				if (fseek(infile, 0, SEEK_END) == -1) {
				  	exif_error ("fseek failed");
					return FALSE;
				}
				ep = ftell(infile);
				if (ep == -1) {
 					exif_error ("ftell failed");
					return FALSE;
				}
				if (fseek(infile, cp, SEEK_SET) == -1) {
					exif_error ("fseek failed");
					return FALSE;
				}

				/* a very basic range check */
				if (ep <= cp ) {
					exif_error ("range check failed");
					return FALSE;
				}
				
				size = (unsigned int) (ep-cp);
				Data = (uchar *)(*exif_malloc_fn)((int)size);
				if (Data == NULL) {
					exif_error("could not allocate data for entire image");
					return FALSE;
				}

				got = fread(Data, 1, size, infile);
				if (got != size) {
					exif_error("could not read the rest of the image");
					return FALSE;
				}

				Sections[*SectionsRead].Data = Data;
				Sections[*SectionsRead].Size = size;
				Sections[*SectionsRead].Type = PSEUDO_IMAGE_MARKER;
				(*SectionsRead)++;
				/*
				 *HaveAll = 1;
				 */
			}
			return TRUE;

		case M_EOI:   /* in case it's a tables-only JPEG stream */
			exif_error("No image in jpeg!");
			return FALSE;

		case M_COM: /* Comment section */
			if (HaveCom) {
				(*SectionsRead) -= 1;
				(*exif_free_fn)(Sections[*SectionsRead].Data);
			} else {
				process_COM(ImageInfo, Data, itemlen);
				HaveCom = TRUE;
			}
			break;

		case M_EXIF:
			if (*SectionsRead <= 2) {
				/* Seen files from some 'U-lead' software with Vivitar scanner
				   that uses marker 31 later in the file (no clue what for!) */
				process_EXIF(ImageInfo, (char *)Data, (unsigned int) itemlen, LastExifRefd);
			}
			break;

		case M_SOF0: 
		case M_SOF1: 
		case M_SOF2: 
		case M_SOF3: 
		case M_SOF5: 
		case M_SOF6: 
		case M_SOF7: 
		case M_SOF9: 
		case M_SOF10:
		case M_SOF11:
		case M_SOF13:
		case M_SOF14:
		case M_SOF15:
			process_SOFn(ImageInfo, Data, marker);
			break;
		default:
			/* skip any other marker silently. */
			break;
        }
    }
    return TRUE;
}

/* 
   Discard read data.
*/
static void
DiscardData(Section_t *Sections, int *SectionsRead)
{
    int a;
    for (a=0;a<*SectionsRead-1;a++) {
        (*exif_free_fn)(Sections[a].Data);
    }
    *SectionsRead = 0;
}

/* 
   Read image data.
*/
static int
ReadJpegFile(ImageInfoType *ImageInfo, Section_t *Sections, 
			 int *SectionsRead, int fd, 
			 int ReadAll, char *LastExifRefd)
{
    FILE *infile;
    int ret;

    infile = fdopen(fd, "rb"); /* Unix ignores 'b', windows needs it. */

    if (infile == NULL) {
		exif_error("Unable to open '%s'", ImageInfo->filename);
		return FALSE;
    }

    /* Start with an empty image information structure. */
    memset(ImageInfo, 0, sizeof(*ImageInfo));
    memset(Sections, 0, sizeof(*Sections));

    ImageInfo->d = exif_alloc();

    /* Scan the JPEG headers. */
    ret = scan_JPEG_header(ImageInfo, infile, Sections, SectionsRead, ReadAll, LastExifRefd);
    if (!ret) {
		exif_error("Invalid Jpeg file: '%s'",ImageInfo->filename);
		return FALSE;
    }

    fclose(infile);

    return ret;
}

static int
read_jpeg_exif(ImageInfoType *ImageInfo, int fd, int ReadAll)
{
	Section_t Sections[20];
	int SectionsRead;
	char *LastExifRefd=NULL;
	int ret;

	ret = ReadJpegFile(ImageInfo, Sections, &SectionsRead, fd, ReadAll, LastExifRefd); 
#if 0
	/*
	 * Thought this might pick out the embedded thumbnail, but it doesn't work.   -RL
     */
	for (i=0;i<SectionsRead-1;i++) {
		if (Sections[i].Type == M_EXIF) {
			thumbsize = Sections[i].Size;
			if(thumbsize>0) {
				ImageInfo->Thumbnail = (*exif_malloc_fn)(thumbsize+5);
				ImageInfo->ThumbnailSize = thumbsize;
				ImageInfo->Thumbnail[0] = 0xff;
				ImageInfo->Thumbnail[1] = 0xd8;
				ImageInfo->Thumbnail[2] = 0xff;
				memcpy(ImageInfo->Thumbnail+4, Sections[i].Data, thumbsize+4);
			}
		}
	}
#endif
    if (ret != FALSE) {
        DiscardData(Sections, &SectionsRead);
    }
	return(ret);
}

exif_data_t *
exif_parse_fd(int fd)
{
	ImageInfoType ImageInfo;

	ImageInfo.filename = "<file stream>";
	if (read_jpeg_exif(&ImageInfo, fd, 1) != TRUE) {
		return NULL;
	}
	return ImageInfo.d;
}

exif_data_t *
exif_parse_file(const char *filename)
{
	ImageInfoType ImageInfo;
	int fd;
	
	ImageInfo.filename = filename;
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		return NULL;
	}

	if (read_jpeg_exif(&ImageInfo, fd, 1) != TRUE) {
		return NULL;
	}
	return ImageInfo.d;
}

void
exif_free_data(struct exif_data *d)
{
	int i;
	for (i=0; i<d->n_recs; i++) {
		(*exif_free_fn)(d->recs[i].rec_name);
		if (d->recs[i].rec_type == 's') {
			(*exif_free_fn)(d->recs[i].rec_data.s);
		}
	}
	(*exif_free_fn)(d);
}

void
exif_init(void *(*malloc_fn)(int),
		  void (*free_fn)(void *),
		  void *(*realloc_fn)(void *, int))
{
	if (malloc_fn == NULL) {
		malloc_fn = (void *(*)(int))malloc;
	}
	exif_malloc_fn = malloc_fn;
	if (free_fn == NULL) {
		free_fn = (void (*)(void *))free;
	}
	exif_free_fn = free_fn;
	if (realloc_fn == NULL) {
		realloc_fn = (void *(*)(void *, int))realloc;
	}
	exif_realloc_fn = realloc_fn;
}

extern exif_record_t *
exif_find_record(exif_data_t *d, const char *rec_name)
{
	int i;
	for (i=0; i<d->n_recs; i++) {
		if (strcmp(d->recs[i].rec_name, rec_name) == 0) {
			return &d->recs[i];
		}
	}
	return NULL;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */


