/* exif-fuji-entry.h
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

#ifndef __EXIF_FUJI_ENTRY_H__
#define __EXIF_FUJI_ENTRY_H__

#include <libexif/exif-format.h>
#include <libexif/fuji/exif-fuji-tag.h>

typedef struct _ExifFujiEntry        ExifFujiEntry;
typedef struct _ExifFujiEntryPrivate ExifFujiEntryPrivate;

#include <libexif/fuji/exif-fuji-note.h>

struct _ExifFujiEntry {
	ExifFujiTag tag;
	ExifFormat format;
	unsigned long components;

	unsigned char *data;
	unsigned int size;

	/* Note containing this entry */
	ExifFujiNote *parent;

	ExifFujiEntryPrivate *priv;
};

/* Lifecycle */
ExifFujiEntry *exif_fuji_entry_new   (void);
void            exif_fuji_entry_ref   (ExifFujiEntry *entry);
void            exif_fuji_entry_unref (ExifFujiEntry *entry);
void            exif_fuji_entry_free  (ExifFujiEntry *entry);

#endif /* __EXIF_FUJI_ENTRY_H__ */
