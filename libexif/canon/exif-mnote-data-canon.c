/* exif-mnote-data-canon.c
 *
 * Copyright © 2002, 2003 Lutz Müller <lutz@users.sourceforge.net>
 * Copyright © 2003 Matthieu Castet <mat-c@users.sourceforge.net>
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
#include "exif-mnote-data-canon.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libexif/exif-byte-order.h>
#include <libexif/exif-utils.h>
#include <libexif/exif-data.h>

/* #define DEBUG */

static void
exif_mnote_data_canon_free (ExifMnoteData *n)
{
        ExifMnoteDataCanon *note = (ExifMnoteDataCanon *) n;
        unsigned int i;

	if (!n) return;

        if (note->entries) {
                for (i = 0; i < note->count; i++)
			if (note->entries[i].data) {
				free (note->entries[i].data);
				note->entries[i].data = NULL;
			}
                free (note->entries);
                note->entries = NULL;
                note->count = 0;
        }
}

static char *
exif_mnote_data_canon_get_value (ExifMnoteData *note, unsigned int n)
{
        ExifMnoteDataCanon *cnote = (ExifMnoteDataCanon *) note;

	if (!note) return NULL;
	if (cnote->count >= n) return NULL;
	return mnote_canon_entry_get_value (&cnote->entries[n]);
}

static void
exif_mnote_data_canon_load_entry_with_exif (ExifMnoteDataCanon *note,
                                            MnoteCanonEntry *entry,
                                            const unsigned char *d,
                                            unsigned int size, unsigned int offset,
				            const unsigned char *exifdata,
					    unsigned int exifsize)
{
        unsigned int s, doff, sizetmp = size;
	const unsigned char *temp = d;

        entry->tag        = exif_get_short (d + offset + 0, note->order);
        entry->format     = exif_get_short (d + offset + 2, note->order);
        entry->components = exif_get_long  (d + offset + 4, note->order);

        /*
         * Size? If bigger than 4 bytes, the actual data is not
         * in the entry but somewhere else (offset).
         */
        s = exif_format_get_size (entry->format) * entry->components;
        if (!s)
                return;
        if (s > 4)
        {
		doff = exif_get_long (d + offset + 8, note->order) + 0xC;
		sizetmp = exifsize;
		temp = exifdata;
	}
        else
                doff = offset + 8;

#ifdef DEBUG
		printf ("Comp %x %d %d %x %x\n",doff, s, sizetmp,temp[doff],temp[doff+1]);
#endif
        /* Sanity check */
        if (sizetmp < doff + s)
                return;
	entry->data = malloc (sizeof (char) * s);
        if (!entry->data)
                return;
        entry->size = s;
        memcpy (entry->data, temp + doff, s);
	entry->order = note->order;
}

static void
exif_mnote_data_canon_load_entry (ExifMnoteDataCanon *note,
                                   MnoteCanonEntry *entry,
                                   const unsigned char *d,
                                   unsigned int size, unsigned int offset)
{
        unsigned int s, doff;

        entry->tag        = exif_get_short (d + offset + 0, note->order);
        entry->format     = exif_get_short (d + offset + 2, note->order);
        entry->components = exif_get_long  (d + offset + 4, note->order);

        /*
         * Size? If bigger than 4 bytes, the actual data is not
         * in the entry but somewhere else (offset).
         */
        s = exif_format_get_size (entry->format) * entry->components;
        if (!s)
                return;
        if (s > 4)
                /* FIXME works on g3, but other should send exif data... */
		doff = exif_get_long (d + offset + 8, note->order)-0x3B0 ;
        else
                doff = offset + 8;

#ifdef DEBUG
                printf ("Comp %x %d %d %x %x\n",doff, s,size,d[doff],d[doff+1]);
#endif
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
exif_mnote_data_canon_load (ExifMnoteData *ne,
	const unsigned char *buf, unsigned int buf_size)
{
	ExifMnoteDataCanon *data = (ExifMnoteDataCanon *) ne;

	const unsigned char *d = buf;
	ExifData *ed = NULL;
	ExifEntry *e = NULL;
	ExifShort n;
	unsigned int i;
	unsigned int size = buf_size;
	MnoteCanonTag tag;

	if (!data)
		return;
	if (!buf || !buf_size)
		return;

	/* If we got EXIF data, go to the MakerNote tag. */
	ed = exif_data_new_from_data (buf, size);
	if (ed) {
		e = exif_content_get_entry (ed->ifd[EXIF_IFD_EXIF],
					    EXIF_TAG_MAKER_NOTE);
		if (e)
		{
			d = e->data;
			size = e->size;
#ifdef DEBUG
			printf ("Exif data\n");
#endif
		}
	}

#ifdef DEBUG
	printf ("Parsing %i byte(s) Mnote data...\n", size);
#endif
#ifdef DEBUG
        int j;
        for (j=0;j<size;j++)
        {
                if (!(j%16)) printf("\n");
                printf("%02X ",d[j]);}

        printf("\n%d\n",size);
        printf("%d\n",data->order);
#endif

	/* Read the number of entries */
	n = exif_get_short (d ,data->order);
#ifdef DEBUG
	printf ("Loading %i entries...\n", n);
#endif
	d+=2;
	size-=2;

	if ( 12 * n > size )
		return;

	for (i = 0; i < n; i++) {

		tag = exif_get_short (d + 12 * i, data->order);
#ifdef DEBUG
		printf ("Loading entry '%s' (%i of %i)...\n",
			exif_mnote_data_canon_tag_get_name (tag), i + 1, n);
#endif
		data->count++;
		data->entries = realloc (data->entries,
				sizeof (MnoteCanonEntry) * data->count);
		if (e) /* we have exif data */
			exif_mnote_data_canon_load_entry_with_exif (data,
				&data->entries[data->count - 1],
				d, size, 12 * i, buf, buf_size);
		else
			exif_mnote_data_canon_load_entry (data,
				&data->entries[data->count - 1],
				d, size, 12 * i);
	}
	exif_data_unref (ed);

}

static unsigned int
exif_mnote_data_canon_count (ExifMnoteData *n)
{
	return n ? ((ExifMnoteDataCanon *) n)->count : 0;
}

static const char *
exif_mnote_data_canon_get_name (ExifMnoteData *note, unsigned int i)
{
	ExifMnoteDataCanon *cnote = (ExifMnoteDataCanon *) note;

	if (!note) return NULL;
	if (i >= cnote->count) return NULL;
	return mnote_canon_tag_get_name (cnote->entries[i].tag);
}

static const char *
exif_mnote_data_canon_get_title (ExifMnoteData *note, unsigned int i)
{
	ExifMnoteDataCanon *cnote = (ExifMnoteDataCanon *) note;

	if (!note) return NULL;
	if (i >= cnote->count) return NULL;
	return mnote_canon_tag_get_title (cnote->entries[i].tag);
}

static const char *
exif_mnote_data_canon_get_description (ExifMnoteData *note, unsigned int i)
{
	ExifMnoteDataCanon *cnote = (ExifMnoteDataCanon *) note;
	if (!note) return NULL;
	if (i >= cnote->count) return NULL;
	return mnote_canon_tag_get_description (cnote->entries[i].tag);
}

ExifMnoteData *
exif_mnote_data_canon_new (ExifByteOrder order)
{
	ExifMnoteData *d;

	d = malloc (sizeof (ExifMnoteDataCanon));
	if (!d) return NULL;

	((ExifMnoteDataCanon *) d)->order = order;

	exif_mnote_data_construct (d);

	/* Set up function pointers */
	d->methods.free            = exif_mnote_data_canon_free;
	d->methods.load            = exif_mnote_data_canon_load;
	d->methods.count           = exif_mnote_data_canon_count;
	d->methods.get_name        = exif_mnote_data_canon_get_name;
	d->methods.get_title       = exif_mnote_data_canon_get_title;
	d->methods.get_description = exif_mnote_data_canon_get_description;
	d->methods.get_value       = exif_mnote_data_canon_get_value;

	return d;
}
