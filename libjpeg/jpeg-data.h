/* jpeg-data.h
 *
 * Copyright (C) 2001 Lutz Müller <lutz@users.sourceforge.net>
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

#ifndef __JPEG_DATA_H__
#define __JPEG_DATA_H__

#include <libjpeg/jpeg-marker.h>
#include <libexif/exif-data.h>

typedef ExifData * JPEGContentExif;

typedef struct _JPEGContentSOF JPEGContentSOF;
struct _JPEGContentSOF {
	unsigned int height, width;
	unsigned char precision, components;
};

typedef struct _JPEGContentSOS JPEGContentSOS;
struct _JPEGContentSOS {
	unsigned char *data;
	unsigned int size;
};

typedef union _JPEGContent JPEGContent;
union _JPEGContent
{
	JPEGContentSOF sof;
	JPEGContentSOS sos;
	JPEGContentExif exif;
};

typedef struct _JPEGSection JPEGSection;
struct _JPEGSection
{
	JPEGMarker marker;
	JPEGContent content;
};

typedef struct _JPEGData        JPEGData;
typedef struct _JPEGDataPrivate JPEGDataPrivate;

struct _JPEGData
{
	JPEGSection *sections;
	unsigned int count;
	JPEGDataPrivate *priv;
};

JPEGData *jpeg_data_new           (void);
JPEGData *jpeg_data_new_from_file (const char *path);
JPEGData *jpeg_data_new_from_data (const unsigned char *data,
				   unsigned int size);

void      jpeg_data_ref   (JPEGData *data);
void      jpeg_data_unref (JPEGData *data);
void      jpeg_data_free  (JPEGData *data);

ExifData *jpeg_data_get_exif_data (JPEGData *data);

void      jpeg_data_dump (JPEGData *data);

#endif /* __JPEG_DATA_H__ */
