/* exif-canon-entry.h
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

#ifndef __EXIF_CANON_ENTRY_H__
#define __EXIF_CANON_ENTRY_H__

#include <libexif/exif-format.h>
#include <libexif/canon/exif-canon-tag.h>

typedef struct _ExifCanonEntry        ExifCanonEntry;
typedef struct _ExifCanonEntryPrivate ExifCanonEntryPrivate;

#include <libexif/canon/exif-canon-note.h>

struct _ExifCanonEntry {
	ExifCanonTag tag;
	ExifFormat format;
	unsigned long components;

	unsigned char *data;
	unsigned int size;

	/* Note containing this entry */
	ExifCanonNote *parent;

	ExifCanonEntryPrivate *priv;
};

/* Lifecycle */
ExifCanonEntry *exif_canon_entry_new   (void);
void            exif_canon_entry_ref   (ExifCanonEntry *entry);
void            exif_canon_entry_unref (ExifCanonEntry *entry);
void            exif_canon_entry_free  (ExifCanonEntry *entry);

#endif /* __EXIF_CANON_ENTRY_H__ */
