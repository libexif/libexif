/* exif-canon-note.c
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
#include "exif-canon-note.h"

#include <stdlib.h>
#include <string.h>

static void
exif_canon_note_free (ExifNote *n)
{
	ExifCanonNote *note = (ExifCanonNote *) n;

	note = NULL;
}

static char **
exif_canon_note_get_value (ExifNote *n)
{
	ExifCanonNote *note = (ExifCanonNote *) n;

	note = NULL;

	return (NULL);
}

static void
exif_canon_note_save_data (ExifNote *n, unsigned char **data,
			   unsigned int *size)
{
	ExifCanonNote *note = (ExifCanonNote *) n;

	note = NULL;
}

static void
exif_canon_note_load_data (ExifNote *n, const unsigned char *data,
			   unsigned int size)
{
	ExifCanonNote *note = (ExifCanonNote *) n;

	note = NULL;
}

ExifNote *
exif_canon_note_new (void)
{
	ExifCanonNote *note;

	note = malloc (sizeof (ExifCanonNote));
	if (!note)
		return (NULL);
	exif_note_construct ((ExifNote *) note);
	((ExifNote *) note)->methods.free = exif_canon_note_free;
	((ExifNote *) note)->methods.load_data = exif_canon_note_load_data;
	((ExifNote *) note)->methods.save_data = exif_canon_note_save_data;
	((ExifNote *) note)->methods.get_value = exif_canon_note_get_value;

	return ((ExifNote *) note);
}
