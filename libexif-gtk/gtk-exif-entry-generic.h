/* gtk-exif-entry-generic.h
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

#ifndef __GTK_EXIF_ENTRY_GENERIC_H__
#define __GTK_EXIF_ENTRY_GENERIC_H__

#include <libexif-gtk/gtk-exif-entry.h>

#define GTK_EXIF_TYPE_ENTRY_GENERIC     (gtk_exif_entry_generic_get_type())
#define GTK_EXIF_ENTRY_GENERIC(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_ENTRY_GENERIC,GtkExifEntryGeneric))
#define GTK_EXIF_ENTRY_GENERIC_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_ENTRY_GENERIC,GtkExifEntryGenericClass))
#define GTK_EXIF_IS_ENTRY_GENERIC(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_ENTRY_GENERIC))

typedef struct _GtkExifEntryGeneric        GtkExifEntryGeneric;
typedef struct _GtkExifEntryGenericPrivate GtkExifEntryGenericPrivate;
typedef struct _GtkExifEntryGenericClass   GtkExifEntryGenericClass;

struct _GtkExifEntryGeneric
{
	GtkExifEntry parent;

	GtkExifEntryGenericPrivate *priv;
};

struct _GtkExifEntryGenericClass
{
	GtkExifEntryClass parent_class;
};

GtkType    gtk_exif_entry_generic_get_type (void);
GtkWidget *gtk_exif_entry_generic_new      (ExifEntry *entry);

#endif /* __GTK_EXIF_ENTRY_GENERIC_H__ */
