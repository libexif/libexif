/* gtk-exif-entry.h
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

#ifndef __GTK_EXIF_ENTRY_H__
#define __GTK_EXIF_ENTRY_H__

#include <gtk/gtkvbox.h>
#include <libexif/exif-data.h>

#define GTK_EXIF_TYPE_ENTRY     (gtk_exif_entry_get_type())
#define GTK_EXIF_ENTRY(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_ENTRY,GtkExifEntry))
#define GTK_EXIF_ENTRY_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_ENTRY,GtkExifEntryClass))
#define GTK_EXIF_IS_ENTRY(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_ENTRY))

typedef struct _GtkExifEntry        GtkExifEntry;
typedef struct _GtkExifEntryPrivate GtkExifEntryPrivate;
typedef struct _GtkExifEntryClass   GtkExifEntryClass;

struct _GtkExifEntry
{
	GtkVBox parent;

	GtkExifEntryPrivate *priv;
};

struct _GtkExifEntryClass
{
	GtkVBoxClass parent_class;

	/* Signals */
	void (* entry_changed) (GtkExifEntry *entry, ExifEntry *e);
	void (* entry_removed) (GtkExifEntry *entry, ExifEntry *e);
	void (* entry_added)   (GtkExifEntry *entry, ExifEntry *e);
};

GtkType    gtk_exif_entry_get_type  (void);
void       gtk_exif_entry_construct (GtkExifEntry *entry, const gchar *name,
				     const gchar *description);

#endif /* __GTK_EXIF_ENTRY_H__ */
