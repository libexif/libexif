/* gtk-exif-content-list.h
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

#ifndef __GTK_EXIF_CONTENT_LIST_H__
#define __GTK_EXIF_CONTENT_LIST_H__

#include <gtk/gtkclist.h>
#include <libexif/exif-data.h>

#define GTK_EXIF_TYPE_CONTENT_LIST     (gtk_exif_content_list_get_type())
#define GTK_EXIF_CONTENT_LIST(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_CONTENT_LIST,GtkExifContentList))
#define GTK_EXIF_CONTENT_LIST_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_CONTENT_LIST,GtkExifContentListClass))
#define GTK_EXIF_IS_CONTENT_LIST(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_CONTENT_LIST))

typedef struct _GtkExifContentList        GtkExifContentList;
typedef struct _GtkExifContentListPrivate GtkExifContentListPrivate;
typedef struct _GtkExifContentListClass   GtkExifContentListClass;

struct _GtkExifContentList
{
	GtkCList parent;

	ExifContent *content;

	GtkExifContentListPrivate *priv;
};

struct _GtkExifContentListClass
{
	GtkCListClass parent_class;

	/* Signals */
	void (* entry_selected)  (GtkExifContentList *list, ExifEntry *);
	void (* content_changed) (GtkExifContentList *list, ExifContent *);
};

GtkType    gtk_exif_content_list_get_type (void);
GtkWidget *gtk_exif_content_list_new      (void);

void       gtk_exif_content_list_add_entry   (GtkExifContentList *list,
					      ExifEntry *entry);
void       gtk_exif_content_list_set_content (GtkExifContentList *list,
					      ExifContent *content);

#endif /* __GTK_EXIF_CONTENT_LIST_H__ */
