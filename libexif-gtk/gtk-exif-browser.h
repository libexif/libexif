/* gtk-exif-browser.h
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

#ifndef __GTK_EXIF_BROWSER_H__
#define __GTK_EXIF_BROWSER_H__

#include <gtk/gtkhpaned.h>
#include <libexif/exif-data.h>

#define GTK_EXIF_TYPE_BROWSER     (gtk_exif_browser_get_type())
#define GTK_EXIF_BROWSER(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_BROWSER,GtkExifBrowser))
#define GTK_EXIF_BROWSER_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_BROWSER,GtkExifBrowserClass))
#define GTK_EXIF_IS_BROWSER(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_BROWSER))

typedef struct _GtkExifBrowser        GtkExifBrowser;
typedef struct _GtkExifBrowserPrivate GtkExifBrowserPrivate;
typedef struct _GtkExifBrowserClass   GtkExifBrowserClass;

struct _GtkExifBrowser
{
	GtkHPaned parent;

	GtkExifBrowserPrivate *priv;
};

struct _GtkExifBrowserClass
{
	GtkHPanedClass parent_class;
};

GtkType    gtk_exif_browser_get_type (void);
GtkWidget *gtk_exif_browser_new      (void);

void       gtk_exif_browser_set_data (GtkExifBrowser *browser, ExifData *data);

#endif /* __GTK_EXIF_BROWSER_H__ */
