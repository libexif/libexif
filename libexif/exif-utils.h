/* exif-utils.h
 *
 * Copyright (c) 2001 Lutz Mueller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __EXIF_UTILS_H__
#define __EXIF_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libexif/exif-byte-order.h>
#include <libexif/exif-format.h>
#include <libexif/_stdint.h>


/* If these definitions don't work for you, please let us fix the 
 * macro generating _stdint.h */
	
typedef unsigned char	ExifByte;          /* 1 byte  */
typedef signed char	ExifSByte;         /* 1 byte  */
typedef char *		ExifAscii;
typedef uint16_t	ExifShort;         /* 2 bytes */
typedef int16_t         ExifSShort;        /* 2 bytes */
typedef uint32_t	ExifLong;          /* 4 bytes */

/** EXIF Unsigned Rational */
typedef struct {ExifLong numerator; ExifLong denominator;} ExifRational;

typedef char		ExifUndefined;     /* 1 byte  */
typedef int32_t		ExifSLong;         /* 4 bytes */

/** EXIF Signed Rational. */
typedef struct {ExifSLong numerator; ExifSLong denominator;} ExifSRational;


ExifShort     exif_get_short     (const unsigned char *b, ExifByteOrder order);
ExifSShort    exif_get_sshort    (const unsigned char *b, ExifByteOrder order);
ExifLong      exif_get_long      (const unsigned char *b, ExifByteOrder order);
ExifSLong     exif_get_slong     (const unsigned char *b, ExifByteOrder order);
ExifRational  exif_get_rational  (const unsigned char *b, ExifByteOrder order);
ExifSRational exif_get_srational (const unsigned char *b, ExifByteOrder order);

void exif_set_short     (unsigned char *b, ExifByteOrder order,
			 ExifShort value);
void exif_set_sshort    (unsigned char *b, ExifByteOrder order,
			 ExifSShort value);
void exif_set_long      (unsigned char *b, ExifByteOrder order,
			 ExifLong value);
void exif_set_slong     (unsigned char *b, ExifByteOrder order,
			 ExifSLong value);
void exif_set_rational  (unsigned char *b, ExifByteOrder order,
			 ExifRational value);
void exif_set_srational (unsigned char *b, ExifByteOrder order,
			 ExifSRational value);

void exif_convert_utf16_to_utf8 (char *out, const unsigned short *in, int maxlen);

/* Please do not use this function outside of the library. */
void exif_array_set_byte_order (ExifFormat, unsigned char *, unsigned int,
		ExifByteOrder o_orig, ExifByteOrder o_new);

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef  MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

/* For compatibility with older versions */
#define EXIF_TAG_SUBSEC_TIME EXIF_TAG_SUB_SEC_TIME

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_UTILS_H__ */
