/* gtk-exif-entry-flash.h
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

#ifndef __GTK_EXIF_ENTRY_FLASH_H__
#define __GTK_EXIF_ENTRY_FLASH_H__

#include <libexif-gtk/gtk-exif-entry.h>

#define GTK_EXIF_TYPE_ENTRY_FLASH     (gtk_exif_entry_flash_get_type())
#define GTK_EXIF_ENTRY_FLASH(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_ENTRY_FLASH,GtkExifEntryFlash))
#define GTK_EXIF_ENTRY_FLASH_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_ENTRY_FLASH,GtkExifEntryFlashClass))
#define GTK_EXIF_IS_ENTRY_FLASH(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_ENTRY_FLASH))

typedef struct _GtkExifEntryFlash        GtkExifEntryFlash;
typedef struct _GtkExifEntryFlashPrivate GtkExifEntryFlashPrivate;
typedef struct _GtkExifEntryFlashClass   GtkExifEntryFlashClass;

struct _GtkExifEntryFlash
{
	GtkExifEntry parent;

	GtkExifEntryFlashPrivate *priv;
};

struct _GtkExifEntryFlashClass
{
	GtkExifEntryClass parent_class;
};

GtkType    gtk_exif_entry_flash_get_type (void);
GtkWidget *gtk_exif_entry_flash_new      (ExifEntry *entry);

#endif /* __GTK_EXIF_ENTRY_FLASH_H__ */
