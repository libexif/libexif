/* exif-note.c
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
#include "exif-note.h"
#include "canon/exif-canon-note.h"
#include "fuji/exif-fuji-note.h"
#include "olympus/exif-olympus-note.h"

#include <stdlib.h>
#include <string.h>

struct _ExifNotePrivate {
	ExifByteOrder order;

	unsigned int ref_count;
};

void
exif_note_construct (ExifNote *note)
{
	if (!note)
		return;

	note->priv = malloc (sizeof (ExifNotePrivate));
	if (!note->priv)
		return;
	memset (note->priv, 0, sizeof (ExifNotePrivate));

	note->priv->ref_count = 1;
}

void
exif_note_ref (ExifNote *note)
{
	if (!note || !note->priv)
		return;

	note->priv->ref_count++;
}

void
exif_note_unref (ExifNote *note)
{
	if (!note || !note->priv)
		return;

	note->priv->ref_count--;
	if (!note->priv->ref_count)
		exif_note_free (note);
}

void
exif_note_free (ExifNote *note)
{
	if (!note)
		return;

	if (!note->priv) {
		free (note->priv);
		note->priv = NULL;
	}

	free (note);
}

static void
exif_note_load_data (ExifNote *note,
		     const unsigned char *data, unsigned int size)
{
	if (!note)
		return;

	if (note->methods.load_data)
		note->methods.load_data (note, data, size);
}

ExifNote *
exif_note_new_from_data (const unsigned char *data, unsigned int size)
{
	ExifNote *note = NULL;

	if (!size || !data)
		return (NULL);

	/* Canon notes begin with 0x0000 */
	if ((size > 1) && (data[0] == 0x00) && (data[1] == 0x00))
		note = exif_canon_note_new ();

	/* Olympus notes begin with "OLYMP" */
	else if ((size >= 5) && !memcmp (data, "OLYMP", 5)) {
		note = exif_olympus_note_new ();

	} else {
		note = NULL;
	}

	if (note)
		exif_note_load_data (note, data, size);

	return (note);
}

char **
exif_note_get_value (ExifNote *note)
{
	if (!note || !note->methods.get_value)
		return (NULL);

	return (note->methods.get_value (note));
}

void
exif_note_set_byte_order (ExifNote *note, ExifByteOrder order)
{
	if (!note || !note->priv)
		return;
	
	note->priv->order = order;
	
	if (!note->methods.set_order)
		return;

	note->methods.set_order (note, order);
}

ExifByteOrder
exif_note_get_byte_order (ExifNote *note)
{
	if (!note || !note->priv)
		return (0);

	return (note->priv->order);
}
