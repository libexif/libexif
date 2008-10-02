/* exif-mnote-data-pentax.c
 *
 * Copyright (c) 2002, 2003 Lutz Mueller <lutz@users.sourceforge.net>
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

#include "config.h"
#include "exif-mnote-data-pentax.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libexif/exif-byte-order.h>
#include <libexif/exif-utils.h>

/* #define DEBUG */

static void
exif_mnote_data_pentax_clear (ExifMnoteDataPentax *n)
{
	ExifMnoteData *d = (ExifMnoteData *) n;
	unsigned int i;

	if (!n) return;

	if (n->entries) {
		for (i = 0; i < n->count; i++)
			if (n->entries[i].data) {
				exif_mem_free (d->mem, n->entries[i].data);
				n->entries[i].data = NULL;
			}
		exif_mem_free (d->mem, n->entries);
		n->entries = NULL;
		n->count = 0;
	}
}

static void
exif_mnote_data_pentax_free (ExifMnoteData *n)
{
	if (!n) return;

	exif_mnote_data_pentax_clear ((ExifMnoteDataPentax *) n);
}

static char *
exif_mnote_data_pentax_get_value (ExifMnoteData *d, unsigned int i, char *val, unsigned int maxlen)
{
	ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) d;

	if (!n) return NULL;
	if (n->count <= i) return NULL;
	return mnote_pentax_entry_get_value (&n->entries[i], val, maxlen);
}

static void
exif_mnote_data_pentax_load (ExifMnoteData *en,
		const unsigned char *buf, unsigned int buf_size)
{
	ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) en;
	size_t i, o, s, datao = 6 + n->offset, base = 0;
	ExifShort c;

	/* Number of entries */
	if (buf_size < datao + (4 + 2) + 2) return;
	if (!memcmp(buf + datao, "AOC", 4)) {
		if ((buf[datao + 4] == 'I') && (buf[datao + 5] == 'I')) {
			n->version = pentaxV3;
			n->order = EXIF_BYTE_ORDER_INTEL;
		} else if ((buf[datao + 4] == 'M') && (buf[datao + 5] == 'M')) {
			n->version = pentaxV3;
			n->order = EXIF_BYTE_ORDER_MOTOROLA;
		} else {
			/* Uses Casio v2 tags */
			n->version = pentaxV2;
		}
		datao += 4 + 2;
		base = MNOTE_PENTAX2_TAG_BASE;
	} if (!memcmp(buf + datao, "QVC", 4)) {
		n->version = casioV2;
		base = MNOTE_CASIO2_TAG_BASE;
		datao += 4 + 2;
	} else {
		n->version = pentaxV1;
	}
	c = exif_get_short (buf + datao, n->order);
	n->entries = exif_mem_alloc (en->mem, sizeof (MnotePentaxEntry) * c);
	if (!n->entries) return;

	for (i = 0; i < c; i++) {
		o = datao + 2 + 12 * i;
		if (o + 8 > buf_size) return;

		n->count = i + 1;
		n->entries[i].tag        = exif_get_short (buf + o + 0, n->order) + base;
		n->entries[i].format     = exif_get_short (buf + o + 2, n->order);
		n->entries[i].components = exif_get_long  (buf + o + 4, n->order);
		n->entries[i].order      = n->order;

		/*
		 * Size? If bigger than 4 bytes, the actual data is not
		 * in the entry but somewhere else (offset).
		 */
		s = exif_format_get_size (n->entries[i].format) *
                                      n->entries[i].components;
		if (s > 65536) {
			/* Corrupt data: EXIF data size is limited to the
			 * maximum size of a JPEG segment (64 kb). 
			 */
			continue;
		}
		if (!s) return;
		o += 8;
		if (s > 4) o = exif_get_long (buf + o, n->order) + 6;
		if (o + s > buf_size) return;
                                                                                
		/* Sanity check */
		n->entries[i].data = exif_mem_alloc (en->mem, s);
		if (!n->entries[i].data) return;
		n->entries[i].size = s;
		memcpy (n->entries[i].data, buf + o, s);
        }
}

static unsigned int
exif_mnote_data_pentax_count (ExifMnoteData *n)
{
	return n ? ((ExifMnoteDataPentax *) n)->count : 0;
}

static unsigned int
exif_mnote_data_pentax_get_id (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;

	if (!note) return 0;
	if (note->count <= n) return 0;
	return note->entries[n].tag;
}

static const char *
exif_mnote_data_pentax_get_name (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;

	if (!note) return NULL;
	if (note->count <= n) return NULL;
	return mnote_pentax_tag_get_name (note->entries[n].tag);
}

static const char *
exif_mnote_data_pentax_get_title (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;

	if (!note) return NULL;
	if (note->count <= n) return NULL;
	return mnote_pentax_tag_get_title (note->entries[n].tag);
}

static const char *
exif_mnote_data_pentax_get_description (ExifMnoteData *d, unsigned int n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) d;
	
	if (!note) return NULL;
	if (note->count <= n) return NULL;
	return mnote_pentax_tag_get_description (note->entries[n].tag);
}

static void
exif_mnote_data_pentax_set_offset (ExifMnoteData *d, unsigned int o)
{
	if (d) ((ExifMnoteDataPentax *) d)->offset = o;
}

static void
exif_mnote_data_pentax_set_byte_order (ExifMnoteData *d, ExifByteOrder o)
{
	ExifByteOrder o_orig;
	ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) d;
	unsigned int i;

	if (!n) return;

	o_orig = n->order;
	n->order = o;
	for (i = 0; i < n->count; i++) {
		n->entries[i].order = o;
		exif_array_set_byte_order (n->entries[i].format, n->entries[i].data,
				n->entries[i].components, o_orig, o);
	}
}

ExifMnoteData *
exif_mnote_data_pentax_new (ExifMem *mem)
{
	ExifMnoteData *d;

	if (!mem) return NULL;

	d = exif_mem_alloc (mem, sizeof (ExifMnoteDataPentax));
	if (!d) return NULL;

	exif_mnote_data_construct (d, mem);

	/* Set up function pointers */
	d->methods.free            = exif_mnote_data_pentax_free;
	d->methods.set_byte_order  = exif_mnote_data_pentax_set_byte_order;
	d->methods.set_offset      = exif_mnote_data_pentax_set_offset;
	d->methods.load            = exif_mnote_data_pentax_load;
	d->methods.count           = exif_mnote_data_pentax_count;
	d->methods.get_id          = exif_mnote_data_pentax_get_id;
	d->methods.get_name        = exif_mnote_data_pentax_get_name;
	d->methods.get_title       = exif_mnote_data_pentax_get_title;
	d->methods.get_description = exif_mnote_data_pentax_get_description;
	d->methods.get_value       = exif_mnote_data_pentax_get_value;

	return d;
}
