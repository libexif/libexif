/* exif-hp-note.c
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
#include "exif-hp-note.h"

#include <stdlib.h>
#include <string.h>

static void
exif_hp_note_free (ExifNote *n)
{
	ExifHPNote *note = (ExifHPNote *) n;

	note = NULL;
}

static char **
exif_hp_note_get_value (ExifNote *n)
{
	ExifHPNote *note = (ExifHPNote *) n;

	note = NULL;

	return (NULL);
}

static void
exif_hp_note_save_data (ExifNote *n, unsigned char **data,
			   unsigned int *size)
{
	ExifHPNote *note = (ExifHPNote *) n;

	note = NULL;
}

static void
exif_hp_note_load_data (ExifNote *n, const unsigned char *data,
			   unsigned int size)
{
	ExifHPNote *note = (ExifHPNote *) n;

	/*
	 * An example for such a note: 
	 * 0x48 0x50
	 * [0x1b,0x2f,0x0a]
	 * [0x00,0x00,0x00]
	 * [0x1f,0x31,0x09]
	 * 0x00
	 * [0x00,0x00,0x09]
	 * 0x00
	 * [0x00,0x00,0x09]
	 * 0x00
	 * [0x1f,0x31,0x0a]
	 * 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	 * 0x01 0x00 0x03 0x3c 0x04 0x02 0x00 0x00
	 * 0xff 0xff
	 */

	/* Notes start with 'HP'. */
	if ((size < 2) || memcmp (data, "HP", 2))
		return;

	note = NULL;
}

ExifNote *
exif_hp_note_new (void)
{
	ExifHPNote *note;

	note = malloc (sizeof (ExifHPNote));
	if (!note)
		return (NULL);
	exif_note_construct ((ExifNote *) note);
	((ExifNote *) note)->methods.free = exif_hp_note_free;
	((ExifNote *) note)->methods.load_data = exif_hp_note_load_data;
	((ExifNote *) note)->methods.save_data = exif_hp_note_save_data;
	((ExifNote *) note)->methods.get_value = exif_hp_note_get_value;

	return ((ExifNote *) note);
}
