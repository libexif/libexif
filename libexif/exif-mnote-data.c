/* exif-mnote-data.c
 *
 * Copyright (C) 2003 Lutz Müller <lutz@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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
#include "exif-mnote-data.h"
#include "exif-mnote-data-priv.h"

#include <stdlib.h>
#include <string.h>

struct _ExifMnoteDataPriv
{
	unsigned int ref_count;
};

void
exif_mnote_data_construct (ExifMnoteData *d)
{
	if (!d) return;
	if (d->priv) return;
	d->priv = malloc (sizeof (ExifMnoteDataPriv));
	if (!d->priv) return;
	memset (d->priv, 0, sizeof (ExifMnoteDataPriv));
	d->priv->ref_count = 1;
}

void
exif_mnote_data_ref (ExifMnoteData *d)
{
	if (d && d->priv) d->priv->ref_count++;
}

static void
exif_mnote_data_free (ExifMnoteData *d)
{
	if (!d) return;
	if (d->priv) {
		if (d->methods.free)
			d->methods.free (d);
		free (d->priv);
		d->priv = NULL;
	}
}

void
exif_mnote_data_unref (ExifMnoteData *d)
{
	if (!d || !d->priv)
		return;
	if (d->priv->ref_count > 0) d->priv->ref_count--;
	if (!d->priv->ref_count)
		exif_mnote_data_free (d);
}

void
exif_mnote_data_load (ExifMnoteData *d, const unsigned char *buf,
		      unsigned int buf_size)
{
	if (!d || !d->methods.load) return;
	d->methods.load (d, buf, buf_size);
}

void
exif_mnote_data_save (ExifMnoteData *d, unsigned char **buf,
		      unsigned int *buf_size)
{
	if (!d || !d->methods.save) return;
	d->methods.save (d, buf, buf_size);
}

void
exif_mnote_data_set_byte_order (ExifMnoteData *d, ExifByteOrder o)
{
	if (!d || !d->methods.set_byte_order) return;
	d->methods.set_byte_order (d, o);
}

void
exif_mnote_data_set_offset (ExifMnoteData *d, unsigned int o)
{
	if (!d || !d->methods.set_offset) return;
	d->methods.set_offset (d, o);
}

unsigned int
exif_mnote_data_count (ExifMnoteData *d)
{
	if (!d || !d->methods.count) return 0;
	return d->methods.count (d);
}

const char *
exif_mnote_data_get_name (ExifMnoteData *d, unsigned int n)
{
	if (!d || !d->methods.get_name) return NULL;
	return d->methods.get_name (d, n);
}

const char *
exif_mnote_data_get_title (ExifMnoteData *d, unsigned int n)
{
	if (!d || !d->methods.get_title) return NULL;
	return d->methods.get_title (d, n);
}
	
const char *
exif_mnote_data_get_description (ExifMnoteData *d, unsigned int n)
{
	if (!d || !d->methods.get_description) return NULL;
	return d->methods.get_description (d, n);
}
	
char *
exif_mnote_data_get_value (ExifMnoteData *d, unsigned int n)
{
	if (!d || !d->methods.get_value) return NULL;
	return d->methods.get_value (d, n);
}
