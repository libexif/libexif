/* exif-fuji-entry.c
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
#include "exif-fuji-entry.h"

#include <stdlib.h>
#include <string.h>

struct _ExifFujiEntryPrivate {
	unsigned int ref_count;
};

ExifFujiEntry *
exif_fuji_entry_new (void)
{
	ExifFujiEntry *entry;

	entry = malloc (sizeof (ExifFujiEntry));
	if (!entry)
		return (NULL);
	memset (entry, 0, sizeof (ExifFujiEntry));

	entry->priv = malloc (sizeof (ExifFujiEntry));
	if (!entry->priv) {
		exif_fuji_entry_free (entry);
		return (NULL);
	}
	memset (entry->priv, 0, sizeof (ExifFujiEntryPrivate));

	entry->priv->ref_count = 1;

	return (entry);
}

void
exif_fuji_entry_ref (ExifFujiEntry *entry)
{
	if (!entry || !entry->priv)
		return;

	entry->priv->ref_count++;
}

void
exif_fuji_entry_unref (ExifFujiEntry *entry)
{
	if (!entry || !entry->priv)
		return;

	entry->priv->ref_count--;
	if (!entry->priv->ref_count)
		exif_fuji_entry_free (entry);
}

void
exif_fuji_entry_free (ExifFujiEntry *entry)
{
	if (!entry)
		return;

	if (entry->priv) {
		free (entry->priv);
		entry->priv = NULL;
	}

	if (entry->data) {
		free (entry->data);
		entry->data = NULL;
	}

	free (entry);
}
