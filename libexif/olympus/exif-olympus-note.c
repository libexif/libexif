/* exif-olympus-note.c
 *
 * Copyright (C) 2002 Lutz Müller <lutz@users.sourceforge.net>
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
#include "exif-byte-order.h"
#include "exif-olympus-note.h"
#include "exif-utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEBUG

static void
exif_olympus_note_free (ExifNote *n)
{
	ExifOlympusNote *note = (ExifOlympusNote *) n;
	unsigned int i;

	if (note->entries) {
		for (i = 0; i < note->count; i++)
			exif_olympus_entry_unref (note->entries[i]);
		free (note->entries);
		note->entries = NULL;
		note->count = 0;
	}
}

static void
exif_olympus_note_add_entry (ExifOlympusNote *note, ExifOlympusEntry *entry)
{
	if (!note || !entry)
		return;

	entry->parent = note;
	note->entries = realloc (note->entries,
				sizeof (ExifOlympusEntry) * (note->count + 1));
	note->entries[note->count] = entry;
	exif_olympus_entry_ref (entry);
	note->count++;
}

static char **
exif_olympus_note_get_value (ExifNote *n)
{
	char **value = NULL, *v;
	unsigned int i, count;

	ExifOlympusNote *note = (ExifOlympusNote *) n;

	for (count = i = 0; i < note->count; i++) {
		v = exif_olympus_entry_get_value (note->entries[i]);
		if (v) {
			value = realloc (value, sizeof (char *) *
				(count ? count + 2 : 2));
			if (!value)
				return (NULL);
			value[count++] = v;
			value[count] = NULL;
		}
	}

	return (value);
}

static void
exif_olympus_note_load_data_entry (ExifOlympusNote *note,
				   ExifOlympusEntry *entry,
				   const unsigned char *d,
				   unsigned int size, unsigned int offset)
{
	unsigned int s, doff;
	ExifByteOrder order = exif_note_get_byte_order ((ExifNote *) note);

	entry->tag        = exif_get_short (d + offset + 0, order);
	entry->format     = exif_get_short (d + offset + 2, order);
	entry->components = exif_get_long  (d + offset + 4, order);

        /*
         * Size? If bigger than 4 bytes, the actual data is not
         * in the entry but somewhere else (offset).
         */
        s = exif_format_get_size (entry->format) * entry->components;
        if (!s)
                return;
        if (s > 4)
                doff = exif_get_long (d + offset + 8, order);
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
exif_olympus_note_load_data (ExifNote *en, const unsigned char *data,
			     unsigned int size)
{
	ExifOlympusNote *note = (ExifOlympusNote *) en;
	ExifOlympusTag tag;
	ExifOlympusEntry *entry;
	unsigned int i, n;
	ExifByteOrder order = exif_note_get_byte_order (en);

	/* Verify the header */
	if ((size < 5) || memcmp (data, "OLYMP", 5))
		return;
	data += 5;
	size -= 5;

	/* 2 unknown bytes */
	if ((size < 2) || (data[0] != 0x00) || (data[1] != 0x01))
		return;
	data += 2;
	size -= 2;

	/* Number of entries */
	if (size < 2)
		return;
	n = exif_get_short (data, order);
#ifdef DEBUG
	printf ("Reading %i entries...\n", n);
#endif
	data += 2;
	size -= 2;

	/* 2 unknown bytes */
	if ((size < 2) || (data[0] != 0x00) || (data[1] != 0x00))
		return;
	data += 2;
	size -= 2;

	for (i = 0; i < n; i++) {
		tag = exif_get_short (data + 12 * i, order);
#ifdef DEBUG
		printf ("Loading entry '%s' (%i)...\n",
			exif_olympus_tag_get_name (tag), i + 1);
#endif
		entry = exif_olympus_entry_new ();
		exif_olympus_note_add_entry (note, entry);
		exif_olympus_note_load_data_entry (note, entry, data, size,
						   12 * i);
		exif_olympus_entry_unref (entry);
	}
}

ExifNote *
exif_olympus_note_new (void)
{
	ExifOlympusNote *note;

	note = malloc (sizeof (ExifOlympusNote));
	if (!note)
		return (NULL);
	memset (note, 0, sizeof (ExifOlympusNote));
	exif_note_construct ((ExifNote *) note);
	((ExifNote *) note)->methods.free = exif_olympus_note_free;
	((ExifNote *) note)->methods.load_data = exif_olympus_note_load_data;
	((ExifNote *) note)->methods.get_value = exif_olympus_note_get_value;

	return ((ExifNote *) note);
}
