/* exif-mnote-data-olympus.c
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

#include <config.h>
#include "exif-mnote-data-olympus.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libexif/exif-utils.h>
#include <libexif/exif-data.h>

#define DEBUG

static void
exif_mnote_data_olympus_free (ExifMnoteData *n)
{
	ExifMnoteDataOlympus *note = (ExifMnoteDataOlympus *) n;
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
exif_mnote_data_olympus_get_value (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;

	if (!d) return NULL;
	if (n->count <= i) return NULL;
	return mnote_olympus_entry_get_value (&n->entries[i]);
}

#ifdef DEBUG
/* Look at memory around p */
static void dump_memory_around(const unsigned char *p)
{
	static char *format = "%xl %xl %xl %xl\n";
	static unsigned buf[4];
	unsigned int i;

	for (i = 0; i < 5; i++) {
		memcpy(buf, p - 32 + (16 * i), sizeof(buf));
		printf(format, buf[0], buf[1], buf[2], buf[3]);
	}

}
#endif


static void
exif_mnote_data_olympus_load_entry (ExifMnoteDataOlympus *note,
	MnoteOlympusEntry *entry, const unsigned char *d,
	unsigned int size, unsigned int offset)
{
	unsigned int s, doff;
	const unsigned char *d_orig = d;

	entry->tag        = exif_get_short (d + offset + 0, note->order);
	entry->format     = exif_get_short (d + offset + 2, note->order);
	entry->components = exif_get_long  (d + offset + 4, note->order);
	entry->order      = note->order;

        /*
         * Size? If bigger than 4 bytes, the actual data is not
         * in the entry but somewhere else (offset).
         */
        s = exif_format_get_size (entry->format) * entry->components;
#ifdef DEBUG
        printf ("exif get size is %i ", exif_format_get_size (entry->format));
        printf ("entry format is %i, #components is %li, ",
		entry->format, entry->components);
        printf ("entry size is %i\n", s);
#endif
        if (!s)
                return;
        if (s > 4) {

        		/*
        			THIS DOES NOT WORK!
        			The problem is that d points to the first MakerNote Tag.
        			According to the EXIF 2.1 spec, it should point to the
        			TIFF header preceding the EXIF information.
        			I have tried rolling it back by 10 bytes (pointing to the
        			OLYMP) and by 18 bytes (assuming the TIFF header immediately
        			precedes the OLYMP). Neither of these hacks produced any
        			meaningful data within 32 bytes of the pointer, for a
        			case in which I know the desired values of the numerator
        			and denominator of a rational.
        		*/
                doff = exif_get_long (d + offset + 8, note->order);
                d_orig = d + doff;
#ifdef DEBUG
                printf ("**** data offset is %i\n", doff);
                dump_memory_around (d_orig);
#endif
        }
        else {
                doff = offset + 8;
               	d_orig = d + doff;
        }


        /* Sanity check */
        /* This doesn't work if the data is outside the tag!! */
        if ((s <= 4) && (size < doff + s))
                return;

        entry->data = malloc (sizeof (char) * s);
        if (!entry->data)
                return;
        entry->size = s;
        memcpy (entry->data, d_orig, s);
}

static void
exif_mnote_data_olympus_load (ExifMnoteData *en,
			      const unsigned char *data, unsigned int size)
{
	ExifMnoteDataOlympus *note = (ExifMnoteDataOlympus *) en;
	MnoteOlympusTag tag;
	const unsigned char *p = data;
	ExifData *ed = NULL;
	ExifEntry *e = NULL;
	short nEntries;
	const unsigned char *tagEntries;
	unsigned int i;

	/* If we got EXIF data, go to the MakerNote tag. */
	ed = exif_data_new_from_data (data, size);
	if (ed) {
		e = exif_content_get_entry (ed->ifd[EXIF_IFD_EXIF],
					    EXIF_TAG_MAKER_NOTE);
		if (e) p = e->data;
	}

	/*
	 * Olympus headers start with "OLYMP" and need to have at least
	 * a size of 22 bytes.
	 */
	if ((size < 22) || memcmp (p, "OLYMP", 5)) {
		exif_data_unref (ed);
		return;
	}

	nEntries = exif_get_short (p + 8, note->order);
	tagEntries = p + 10;

	for (i = 0; i < nEntries; i++) {
		tag = exif_get_short (tagEntries + 12 * i, note->order);
#ifdef DEBUG
		printf ("Loading entry '%s' (%x) (%i)...\n",
			mnote_olympus_tag_get_name (tag), tag, i + 1);
#endif
		note->count++;
		note->entries = realloc (note->entries,
			sizeof (MnoteOlympusEntry) * note->count);
		exif_mnote_data_olympus_load_entry (note,
			&note->entries[note->count - 1],
			tagEntries, size, 12 * i);
	}
}

static unsigned int
exif_mnote_data_olympus_count (ExifMnoteData *n)
{
	return n ? ((ExifMnoteDataOlympus *) n)->count : 0;
}

static const char *
exif_mnote_data_olympus_get_name (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;

	if (!n) return NULL;
	if (i >= n->count) return NULL;
	return mnote_olympus_tag_get_title (n->entries[i].tag);
}

static const char *
exif_mnote_data_olympus_get_title (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;
	
	if (!n) return NULL;
	if (i >= n->count) return NULL;
        return mnote_olympus_tag_get_title (n->entries[i].tag);
}

static const char *
exif_mnote_data_olympus_get_description (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;
	
	if (!n) return NULL;
	if (i >= n->count) return NULL;
        return mnote_olympus_tag_get_title (n->entries[i].tag);
}

ExifMnoteData *
exif_mnote_data_olympus_new (ExifByteOrder order)
{
	ExifMnoteData *n;

	n = malloc (sizeof (ExifMnoteDataOlympus));
	if (!n) return NULL;
	memset (n, 0, sizeof (ExifMnoteDataOlympus));
	exif_mnote_data_construct (n);

	((ExifMnoteDataOlympus *) n)->order = order;

	/* Set up the function pointers */
	n->methods.free            = exif_mnote_data_olympus_free;
	n->methods.load            = exif_mnote_data_olympus_load;
	n->methods.count           = exif_mnote_data_olympus_count;
	n->methods.get_name        = exif_mnote_data_olympus_get_name;
	n->methods.get_title       = exif_mnote_data_olympus_get_title;
	n->methods.get_description = exif_mnote_data_olympus_get_description;
	n->methods.get_value       = exif_mnote_data_olympus_get_value;

	return n;
}
