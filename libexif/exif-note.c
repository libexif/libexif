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

#include <config.h>
#include "exif-note.h"

#include <stdlib.h>
#include <string.h>

struct _ExifNotePrivate {
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
};

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
