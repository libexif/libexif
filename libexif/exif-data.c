/* exif-data.c
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
#include <config.h>
#include "exif-data.h"

#include <stdlib.h>
#include <stdio.h>

#include <libjpeg/jpeg-marker.h>

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

struct _ExifDataPrivate
{
	ExifByteOrder order;

	unsigned int ref_count;
};

ExifData *
exif_data_new (void)
{
	ExifData *data;

	data = malloc (sizeof (ExifData));
	if (!data)
		return (NULL);
	memset (data, 0, sizeof (ExifData));
	data->priv = malloc (sizeof (ExifDataPrivate));
	if (!data->priv) {
		free (data);
		return (NULL);
	}
	memset (data->priv, 0, sizeof (ExifDataPrivate));
	data->priv->ref_count = 1;

	data->content = exif_content_new ();
	if (!data->content) {
		exif_data_free (data);
		return (NULL);
	}

	return (data);
}

ExifData *
exif_data_new_from_data (const unsigned char *data, unsigned int size)
{
	ExifData *edata;
	unsigned int o, len;
	ExifByteOrder order;

	if (!data)
		return (NULL);

	/* Search the exif marker */
	for (o = 0; o < size; o++)
		if (data[o] == JPEG_MARKER_APP1)
			break;
	if (o == size)
		return (NULL);
	o++;
	data += o;
	size -= o;

	/* Size of exif data? */
	if (size < 2)
		return (NULL);
	len = (data[0] << 8) | data[1];

	/*
	 * Verify the exif header
	 * (offset 2, length 6).
	 */
	if (size < 2 + 6)
		return (NULL);
	if (memcmp (data + 2, ExifHeader, 6))
		return (NULL);

	/*
	 * Byte order (offset 8, length 2) plus two values for
	 * correctness check (offset 10, length 2 and 4).
	 */
	if (size < 16)
		return (NULL);
	if (!memcmp (data + 8, "II", 2))
		order = EXIF_BYTE_ORDER_INTEL;
	else if (!memcmp (data + 8, "MM", 2))
		order = EXIF_BYTE_ORDER_MOTOROLA;
	else
		return (NULL);

	edata = exif_data_new ();
	edata->priv->order = order;

	/* Parse the actual exif data (offset 16) */
	exif_content_parse (edata->content, data + 8, size - 8, 8, order);

	return (edata);
}

ExifData *
exif_data_new_from_file (const char *path)
{
	FILE *f;
	unsigned int size;
	unsigned char *data;
	ExifData *edata;

	f = fopen (path, "r");
	if (!f)
		return (NULL);

	/* For now, we read the data into memory. Patches welcome... */
	fseek (f, 0, SEEK_END);
	size = ftell (f);
	fseek (f, 0, SEEK_SET);
	data = malloc (sizeof (char) * size);
	if (!data)
		return (NULL);
	if (fread (data, 1, size, f) != size) {
		free (data);
		return (NULL);
	}

	edata = exif_data_new_from_data (data, size);
	free (data);

	fclose (f);

	return (edata);
}

void
exif_data_ref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count++;
}

void
exif_data_unref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count--;
	if (!data->priv->ref_count)
		exif_data_free (data);
}

void
exif_data_free (ExifData *data)
{
	if (data->content)
		exif_content_unref (data->content);
	free (data->priv);
	free (data);
}

void
exif_data_dump (ExifData *data)
{
	if (!data)
		return;

	printf ("Dumping EXIF data...\n");
	exif_content_dump (data->content, 0);
}
