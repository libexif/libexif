/* exif-fuji-note.c
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

#include <config.h>
#include "exif-fuji-note.h"

#include <stdlib.h>
#include <string.h>

#include <libexif/exif-byte-order.h>
#include <libexif/exif-utils.h>

struct _ExifFujiNotePrivate {
	ExifByteOrder order;
};

static void
exif_fuji_note_free (ExifNote *n)
{
	ExifFujiNote *note = (ExifFujiNote *) n;
	unsigned int i;

	if (note->entries) {
		for (i = 0; i < note->count; i++)
			exif_fuji_entry_unref (note->entries[i]);
		free (note->entries);
		note->entries = NULL;
		note->count = 0;
	}
}

static char **
exif_fuji_note_get_value (ExifNote *n)
{
	ExifFujiNote *note = (ExifFujiNote *) n;

	note = NULL;

	return (NULL);
}

static void
exif_fuji_note_save_data (ExifNote *n, unsigned char **data,
			   unsigned int *size)
{
	ExifFujiNote *note = (ExifFujiNote *) n;
	ExifByteOrder order = exif_note_get_byte_order (n);

	/*
	 * Header: "FUJIFILM" and 4 bytes offset to the first entry.
	 * As the first entry will start right thereafter, the offset is
	 * 0x000c.
	 */
	*data = malloc (12);
	if (!*data)
		return;
	*size = 12;
	memcpy (*data, "FUJIFILM", 8);
	exif_set_long (*data + 8, order, 0x000c);

	note = NULL;
}

static void
exif_fuji_note_load_data (ExifNote *n, const unsigned char *data,
			   unsigned int size)
{
	ExifFujiNote *note = (ExifFujiNote *) n;

	note = NULL;
}

ExifNote *
exif_fuji_note_new (void)
{
	ExifFujiNote *note;

	note = malloc (sizeof (ExifFujiNote));
	if (!note)
		return (NULL);
	exif_note_construct ((ExifNote *) note);
	((ExifNote *) note)->methods.free = exif_fuji_note_free;
	((ExifNote *) note)->methods.load_data = exif_fuji_note_load_data;
	((ExifNote *) note)->methods.save_data = exif_fuji_note_save_data;
	((ExifNote *) note)->methods.get_value = exif_fuji_note_get_value;

	return ((ExifNote *) note);
}
