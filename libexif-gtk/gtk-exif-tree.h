/* gtk-exif-tree.h
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

#ifndef __GTK_EXIF_TREE_H__
#define __GTK_EXIF_TREE_H__

#include <gtk/gtktree.h>
#include <libexif/exif-data.h>

#define GTK_EXIF_TYPE_TREE     (gtk_exif_tree_get_type())
#define GTK_EXIF_TREE(o)       (GTK_CHECK_CAST((o),GTK_EXIF_TYPE_TREE,GtkExifTree))
#define GTK_EXIF_TREE_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_EXIF_TYPE_TREE,GtkExifTreeClass))
#define GTK_EXIF_IS_TREE(o)    (GTK_CHECK_TYPE((o),GTK_EXIF_TYPE_TREE))

typedef struct _GtkExifTree        GtkExifTree;
typedef struct _GtkExifTreePrivate GtkExifTreePrivate;
typedef struct _GtkExifTreeClass   GtkExifTreeClass;

struct _GtkExifTree
{
	GtkTree parent;

	GtkExifTreePrivate *priv;
};

struct _GtkExifTreeClass
{
	GtkTreeClass parent_class;

	/* Signals */
	void (* entry_selected)   (GtkExifTree *tree, ExifEntry   *entry);
};

GtkType    gtk_exif_tree_get_type (void);
GtkWidget *gtk_exif_tree_new      (void);

void       gtk_exif_tree_set_data (GtkExifTree *tree, ExifData *data);

#endif /* __GTK_EXIF_TREE_H__ */
