/* gtk-exif-entry-meter.h
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

#ifndef __GTK_EXIF_ENTRY_METER_H__
#define __GTK_EXIF_ENTRY_METER_H__

#include <libexif-gtk/gtk-exif-entry.h>

#define GTK_EXIF_TYPE_ENTRY_METER     (gtk_exif_entry_meter_get_type())
#define GTK_EXIF_ENTRY_METER(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_ENTRY_METER,GtkExifEntryMeter))
#define GTK_EXIF_ENTRY_METER_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_ENTRY_METER,GtkExifEntryMeterClass))
#define GTK_EXIF_IS_ENTRY_METER(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_ENTRY_METER))

typedef struct _GtkExifEntryMeter        GtkExifEntryMeter;
typedef struct _GtkExifEntryMeterPrivate GtkExifEntryMeterPrivate;
typedef struct _GtkExifEntryMeterClass   GtkExifEntryMeterClass;

struct _GtkExifEntryMeter
{
	GtkExifEntry parent;

	GtkExifEntryMeterPrivate *priv;
};

struct _GtkExifEntryMeterClass
{
	GtkExifEntryClass parent_class;
};

GtkType    gtk_exif_entry_meter_get_type (void);
GtkWidget *gtk_exif_entry_meter_new      (ExifEntry *entry);

#endif /* __GTK_EXIF_ENTRY_METER_H__ */
