/* gtk-exif-tree-item.h
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

#ifndef __GTK_EXIF_TREE_ITEM_H__
#define __GTK_EXIF_TREE_ITEM_H__

#include <gtk/gtktreeitem.h>
#include <libexif/exif-entry.h>

#define GTK_EXIF_TYPE_TREE_ITEM     (gtk_exif_tree_item_get_type())
#define GTK_EXIF_TREE_ITEM(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_TREE_ITEM,GtkExifTreeItem))
#define GTK_EXIF_TREE_ITEM_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_TREE_ITEM,GtkExifTreeItemClass))
#define GTK_EXIF_IS_TREE_ITEM(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_TREE_ITEM))

typedef struct _GtkExifTreeItem        GtkExifTreeItem;
typedef struct _GtkExifTreeItemPrivate GtkExifTreeItemPrivate;
typedef struct _GtkExifTreeItemClass   GtkExifTreeItemClass;

struct _GtkExifTreeItem
{
	GtkTreeItem parent;

	ExifEntry *entry;

	GtkExifTreeItemPrivate *priv;
};

struct _GtkExifTreeItemClass
{
	GtkTreeItemClass parent_class;
};

GtkType    gtk_exif_tree_item_get_type (void);
GtkWidget *gtk_exif_tree_item_new      (void);

void       gtk_exif_tree_item_set_entry (GtkExifTreeItem *item,
					 ExifEntry *entry);

#endif /* __GTK_EXIF_TREE_ITEM_H__ */
