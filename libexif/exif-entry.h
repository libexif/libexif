/* exif-entry.h
 *
 * Copyright (C) 2001 Lutz Müller <lutz@users.sourceforge.net>
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

#ifndef __EXIF_ENTRY_H__
#define __EXIF_ENTRY_H__

#include <libexif/exif-utils.h>

typedef struct _ExifEntry        ExifEntry;
typedef struct _ExifEntryPrivate ExifEntryPrivate;

#include <libexif/exif-content.h>

typedef enum _ExifFormat ExifFormat;
enum _ExifFormat {
        EXIF_FORMAT_BYTE       =  1,
        EXIF_FORMAT_ASCII      =  2,
        EXIF_FORMAT_SHORT      =  3,
        EXIF_FORMAT_LONG       =  4,
        EXIF_FORMAT_RATIONAL   =  5,
        EXIF_FORMAT_UNDEFINED  =  7,
        EXIF_FORMAT_SLONG      =  9,
        EXIF_FORMAT_SRATIONAL  = 10,
};

struct _ExifEntry {
	ExifByteOrder order;
        ExifTag tag;
        ExifFormat format;
        unsigned long components;

        unsigned char *data;
        unsigned int size;

        ExifContent *content;

	/* Content containing this entry */
	ExifContent *parent;

	ExifEntryPrivate *priv;
};

/* Lifecycle */
ExifEntry  *exif_entry_new   (void);
void        exif_entry_ref   (ExifEntry *entry);
void        exif_entry_unref (ExifEntry *entry);
void        exif_entry_free  (ExifEntry *entry);

/* Notification */
typedef enum _ExifEntryEvent ExifEntryEvent;
enum _ExifEntryEvent {
	EXIF_ENTRY_EVENT_CHANGED = 1 << 0
};
typedef void (* ExifEntryNotifyFunc)  (ExifEntry *entry, void *data);
unsigned int exif_entry_add_notify    (ExifEntry *entry, ExifEntryEvent events,
				       ExifEntryNotifyFunc func, void *data);
void         exif_entry_remove_notify (ExifEntry *entry, unsigned int id);
void         exif_entry_notify        (ExifEntry *entry, ExifEntryEvent event);

void        exif_entry_parse     (ExifEntry *entry, const unsigned char *data,
			          unsigned int size, unsigned int offset,
			          ExifByteOrder order);

void        exif_entry_initialize (ExifEntry *entry, ExifTag tag);

const char *exif_entry_get_value  (ExifEntry *entry);

void        exif_entry_dump      (ExifEntry *entry, unsigned int indent);

#endif /* __EXIF_ENTRY_H__ */
