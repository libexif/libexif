/* gtk-exif-tag-menu.h
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

#ifndef __GTK_EXIF_TAG_MENU_H__
#define __GTK_EXIF_TAG_MENU_H__

#include <gtk-extensions/gtk-menu-option.h>
#include <libexif/exif-tag.h>

#define GTK_EXIF_TYPE_TAG_MENU     (gtk_exif_tag_menu_get_type())
#define GTK_EXIF_TAG_MENU(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_TAG_MENU,GtkExifTagMenu))
#define GTK_EXIF_TAG_MENU_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_TAG_MENU,GtkExifTagMenuClass))
#define GTK_IS_EXIF_TAG_MENU(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_TAG_MENU))

typedef struct _GtkExifTagMenu        GtkExifTagMenu;
typedef struct _GtkExifTagMenuPrivate GtkExifTagMenuPrivate;
typedef struct _GtkExifTagMenuClass   GtkExifTagMenuClass;

struct _GtkExifTagMenu
{
	GtkMenuOption parent;

	GtkExifTagMenuPrivate *priv;
};

struct _GtkExifTagMenuClass
{
	GtkMenuOptionClass parent_class;

	/* Signals */
	void (* tag_selected) (GtkExifTagMenu *menu, ExifTag tag);
};

GtkType    gtk_exif_tag_menu_get_type (void);
GtkWidget *gtk_exif_tag_menu_new      (void);

#endif /* __GTK_EXIF_TAG_MENU_H__ */
