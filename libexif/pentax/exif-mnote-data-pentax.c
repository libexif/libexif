/* exif-mnote-data-pentax.c
 *
 * Copyright © 2002, 2003 Lutz Mueller <lutz@users.sourceforge.net>
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
exif_mnote_data_pentax_free (ExifMnoteData *n)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) n;
	unsigned int i;

	if (note->entries) {
		for (i = 0; i < note->count; i++) {
			free (note->entries[i].data);
			note->entries[i].data = NULL;
		}
		free (note->entries);
		note->entries = NULL;
		note->count = 0;
	}
}

static char *
exif_mnote_data_pentax_get_value (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataPentax *n = (ExifMnoteDataPentax *) d;

	if (!n) return NULL;
	if (n->count >= i) return NULL;
	return mnote_pentax_entry_get_value (&n->entries[i]);
}

static void
exif_mnote_data_pentax_load_entry (ExifMnoteDataPentax *note,
				  MnotePentaxEntry *entry,
				  const unsigned char *d,
				  unsigned int size, unsigned int offset)
{
	unsigned int s, doff;

	entry->tag        = exif_get_short (d + offset + 0, note->order);
	entry->format     = exif_get_short (d + offset + 2, note->order);
	entry->components = exif_get_long  (d + offset + 4, note->order);
	entry->order      = note->order;

        /*
         * Size? If bigger than 4 bytes, the actual data is not
         * in the entry but somewhere else (offset).
         */
        s = exif_format_get_size (entry->format) * entry->components;
        if (!s)
                return;
        if (s > 4)
                doff = exif_get_long (d + offset + 8, note->order);
        else
                doff = offset + 8;

        /* Sanity check */
        if (size < doff + s)
                return;

        entry->data = malloc (sizeof (char) * s);
        if (!entry->data)
                return;
        entry->size = s;
        memcpy (entry->data, d + doff, s);
}

static void
exif_mnote_data_pentax_load (ExifMnoteData *en,
		const unsigned char *data, unsigned int size)
{
	ExifMnoteDataPentax *note = (ExifMnoteDataPentax *) en;
	MnotePentaxTag tag;
	unsigned int i, n;

	/* Number of entries */
	if (size < 2)
		return;
	n = exif_get_short (data, note->order);
#ifdef DEBUG
	printf ("Reading %i entries...\n", n);
#endif
	data += 2;
	size -= 2;

	for (i = 0; i < n; i++) {
		tag = exif_get_short (data + 12 * i, note->order);
#ifdef DEBUG
		printf ("Loading entry '%s' (%i)...\n",
			mnote_pentax_tag_get_name (tag), i + 1);
#endif
		note->count++;
		note->entries = realloc (note->entries,
				sizeof (MnotePentaxEntry) * note->count);
		exif_mnote_data_pentax_load_entry (note,
			&note->entries[note->count - 1], data, size, 12 * i);
	}
}

static unsigned int
exif_mnote_data_pentax_count (ExifMnoteData *n)
{
	return n ? ((ExifMnoteDataPentax *) n)->count : 0;
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
	if (d) ((ExifMnoteDataPentax *) d)->order = o;
}

ExifMnoteData *
exif_mnote_data_pentax_new (void)
{
	ExifMnoteData *n;

	n = malloc (sizeof (ExifMnoteDataPentax));
	if (!n) return NULL;
	memset (n, 0, sizeof (ExifMnoteDataPentax));

	exif_mnote_data_construct (n);

	/* Set up function pointers */
	n->methods.free            = exif_mnote_data_pentax_free;
	n->methods.set_byte_order  = exif_mnote_data_pentax_set_byte_order;
	n->methods.set_offset      = exif_mnote_data_pentax_set_offset;
	n->methods.load            = exif_mnote_data_pentax_load;
	n->methods.count           = exif_mnote_data_pentax_count;
	n->methods.get_name        = exif_mnote_data_pentax_get_name;
	n->methods.get_title       = exif_mnote_data_pentax_get_title;
	n->methods.get_description = exif_mnote_data_pentax_get_description;
	n->methods.get_value       = exif_mnote_data_pentax_get_value;

	return n;
}
