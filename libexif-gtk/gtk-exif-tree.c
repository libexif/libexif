/* gtk-exif-tree.c
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

#include <config.h>
#include "gtk-exif-tree.h"

#include <gtk/gtksignal.h>

#include "gtk-exif-tree-item.h"

struct _GtkExifTreePrivate {
	ExifData *data;
};

#define PARENT_TYPE GTK_TYPE_TREE
static GtkTreeClass *parent_class;

enum {
	ENTRY_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_tree_destroy (GtkObject *object)
{
	GtkExifTree *tree = GTK_EXIF_TREE (object);

	if (tree->priv->data) {
		exif_data_unref (tree->priv->data);
		tree->priv->data = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_tree_finalize (GtkObject *object)
{
	GtkExifTree *tree = GTK_EXIF_TREE (object);

	g_free (tree->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_tree_class_init (GtkExifTreeClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_tree_destroy;
	object_class->finalize = gtk_exif_tree_finalize;

	signals[ENTRY_SELECTED] = gtk_signal_new ("entry_selected",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkExifTreeClass, entry_selected),
		gtk_marshal_NONE__POINTER, GTK_TYPE_NONE, 1,
		GTK_TYPE_POINTER);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_tree_init (GtkExifTree *tree)
{
	tree->priv = g_new0 (GtkExifTreePrivate, 1);
}

GtkType
gtk_exif_tree_get_type (void)
{
	static GtkType tree_type = 0;

	if (!tree_type) {
		static const GtkTypeInfo tree_info = {
			"GtkExifTree",
			sizeof (GtkExifTree),
			sizeof (GtkExifTreeClass),
			(GtkClassInitFunc)  gtk_exif_tree_class_init,
			(GtkObjectInitFunc) gtk_exif_tree_init,
			NULL, NULL, NULL};
		tree_type = gtk_type_unique (PARENT_TYPE, &tree_info);
	}

	return (tree_type);
}

static void
on_select_child (GtkTree *tree, GtkWidget *item, GtkExifTree *etree)
{
	GtkExifTreeItem *eitem = GTK_EXIF_TREE_ITEM (item);

	gtk_signal_emit (GTK_OBJECT (etree), signals[ENTRY_SELECTED],
			 eitem->entry);
}

GtkWidget *
gtk_exif_tree_new (void)
{
	GtkExifTree *tree;

	tree = gtk_type_new (GTK_EXIF_TYPE_TREE);
	gtk_signal_connect (GTK_OBJECT (tree), "select_child",
			    GTK_SIGNAL_FUNC (on_select_child), tree);

	return (GTK_WIDGET (tree));
}

void
gtk_exif_tree_set_data (GtkExifTree *tree, ExifData *data)
{
	guint i;
	GtkWidget *item;

	g_return_if_fail (data != NULL);

	if (tree->priv->data)
		exif_data_unref (tree->priv->data);
	tree->priv->data = data;
	exif_data_ref (data);

	gtk_tree_remove_items (GTK_TREE (tree), GTK_TREE (tree)->children);
	for (i = 0; i < data->content->count; i++) {
		item = gtk_exif_tree_item_new ();
		gtk_widget_show (item);
		gtk_tree_append (GTK_TREE (tree), item);
		gtk_exif_tree_item_set_entry (
			GTK_EXIF_TREE_ITEM (item), data->content->entries[i]);
	}
}
