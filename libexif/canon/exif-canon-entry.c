/* exif-canon-entry.c
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
#include "exif-canon-entry.h"

#include <stdlib.h>
#include <string.h>

struct _ExifCanonEntryPrivate {
	unsigned int ref_count;
};

ExifCanonEntry *
exif_canon_entry_new (void)
{
	ExifCanonEntry *entry;

	entry = malloc (sizeof (ExifCanonEntry));
	if (!entry)
		return (NULL);
	memset (entry, 0, sizeof (ExifCanonEntry));

	entry->priv = malloc (sizeof (ExifCanonEntry));
	if (!entry->priv) {
		exif_canon_entry_free (entry);
		return (NULL);
	}
	memset (entry->priv, 0, sizeof (ExifCanonEntryPrivate));

	entry->priv->ref_count = 1;

	return (entry);
}

void
exif_canon_entry_ref (ExifCanonEntry *entry)
{
	if (!entry || !entry->priv)
		return;

	entry->priv->ref_count++;
}

void
exif_canon_entry_unref (ExifCanonEntry *entry)
{
	if (!entry || !entry->priv)
		return;

	entry->priv->ref_count--;
	if (!entry->priv->ref_count)
		exif_canon_entry_free (entry);
}

void
exif_canon_entry_free (ExifCanonEntry *entry)
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
