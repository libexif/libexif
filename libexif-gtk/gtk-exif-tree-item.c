/* gtk-exif-tree-item.c
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
#include "gtk-exif-tree-item.h"

#include <gtk/gtklabel.h>
#include <gtk/gtktree.h>
#include <gtk/gtklabel.h>
#include <gtk/gtksignal.h>

struct _GtkExifTreeItemPrivate
{
};

#define PARENT_TYPE GTK_TYPE_TREE_ITEM
static GtkTreeItemClass *parent_class;

static void
gtk_exif_tree_item_destroy (GtkObject *object)
{
	GtkExifTreeItem *item = GTK_EXIF_TREE_ITEM (object);

	if (item->entry) {
		exif_entry_unref (item->entry);
		item->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_tree_item_finalize (GtkObject *object)
{
	GtkExifTreeItem *item = GTK_EXIF_TREE_ITEM (object);

	g_free (item->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_tree_item_class_init (GtkExifTreeItemClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_tree_item_destroy;
	object_class->finalize = gtk_exif_tree_item_finalize;

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_tree_item_init (GtkExifTreeItem *item)
{
	item->priv = g_new0 (GtkExifTreeItemPrivate, 1);
}

GtkType
gtk_exif_tree_item_get_type (void)
{
	static GtkType item_type = 0;

	if (!item_type) {
		static const GtkTypeInfo item_info = {
			"GtkExifTreeItem",
			sizeof (GtkExifTreeItem),
			sizeof (GtkExifTreeItemClass),
			(GtkClassInitFunc) gtk_exif_tree_item_class_init,
			(GtkObjectInitFunc) gtk_exif_tree_item_init,
			NULL, NULL, NULL};
		item_type = gtk_type_unique (PARENT_TYPE, &item_info);
	}

	return (item_type);
}

static void
fill_tree (GtkTree *tree, ExifContent *content)
{
	GtkWidget *item;
	guint i;
	ExifEntry *entry;

	for (i = 0; i < content->count; i++) {
		entry = content->entries[i];
		item = gtk_exif_tree_item_new ();
		gtk_widget_show (item);
		gtk_tree_append (tree, item);
		gtk_exif_tree_item_set_entry (GTK_EXIF_TREE_ITEM (item),
					      entry);
	}
}

GtkWidget *
gtk_exif_tree_item_new (void)
{
	GtkExifTreeItem *item;

	item = gtk_type_new (GTK_EXIF_TYPE_TREE_ITEM);

	return (GTK_WIDGET (item));
}

static void
on_select_child (GtkTree *tree, GtkWidget *item)
{
	GtkExifTreeItem *eitem = GTK_EXIF_TREE_ITEM (item);

	gtk_signal_emit_by_name (GTK_OBJECT (tree->root_tree),
				 "entry_selected", eitem->entry);
}

void
gtk_exif_tree_item_set_entry (GtkExifTreeItem *item, ExifEntry *entry)
{
	GtkWidget *tree, *label;

	g_return_if_fail (GTK_EXIF_IS_TREE_ITEM (item));
	g_return_if_fail (entry != NULL);

	/* Keep a reference to this entry */
	if (item->entry)
		exif_entry_unref (item->entry);
	item->entry = entry;
	exif_entry_ref (entry);

	label = gtk_label_new (exif_tag_get_name (entry->tag));
	gtk_widget_show (label);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
	gtk_container_add (GTK_CONTAINER (item), label);

	if (entry->content->count) {
		tree = gtk_tree_new ();
		gtk_widget_show (tree);
		gtk_tree_item_set_subtree (GTK_TREE_ITEM (item), tree);
		fill_tree (GTK_TREE (tree), entry->content);

		/* Bug in GtkTree? */
		gtk_signal_connect (GTK_OBJECT (tree), "select_child",
				    GTK_SIGNAL_FUNC (on_select_child), NULL);
	}
}
