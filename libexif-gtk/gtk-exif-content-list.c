/* gtk-exif-content-list.c
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
#include "gtk-exif-content-list.h"

#include <gtk/gtkmenu.h>
#include <gtk/gtkmenuitem.h>

#include "gtk-exif-tag-menu.h"

struct _GtkExifContentListPrivate {
};

#define PARENT_TYPE GTK_TYPE_CLIST
static GtkCListClass *parent_class;

enum {
	ENTRY_ADDED,
	ENTRY_REMOVED,
	ENTRY_CHANGED,
	ENTRY_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_content_list_destroy (GtkObject *object)
{
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (object);

	if (list->content) {
		exif_content_unref (list->content);
		list->content = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_content_list_finalize (GtkObject *object)
{
	GtkExifContentList *list = GTK_EXIF_CONTENT_LIST (object);

	g_free (list->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_content_list_class_init (GtkExifContentListClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_content_list_destroy;
	object_class->finalize = gtk_exif_content_list_finalize;

	signals[ENTRY_SELECTED] = gtk_signal_new ("entry_selected",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkExifContentListClass, entry_selected),
		gtk_marshal_NONE__POINTER, GTK_TYPE_NONE, 1, GTK_TYPE_POINTER);
	signals[ENTRY_ADDED] = gtk_signal_new ("entry_added",
		GTK_RUN_FIRST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkExifContentListClass, entry_added),
		gtk_marshal_NONE__POINTER, GTK_TYPE_NONE, 1, GTK_TYPE_POINTER);
	signals[ENTRY_CHANGED] = gtk_signal_new ("entry_changed",
		GTK_RUN_FIRST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkExifContentListClass, entry_changed),
		gtk_marshal_NONE__POINTER, GTK_TYPE_NONE, 1, GTK_TYPE_POINTER);
	signals[ENTRY_REMOVED] = gtk_signal_new ("entry_removed",
		GTK_RUN_FIRST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkExifContentListClass, entry_removed),
		gtk_marshal_NONE__POINTER, GTK_TYPE_NONE, 1, GTK_TYPE_POINTER);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_content_list_init (GtkExifContentList *list)
{
	list->priv = g_new0 (GtkExifContentListPrivate, 1);
}

GtkType
gtk_exif_content_list_get_type (void)
{
	static GtkType content_list_type = 0;

	if (!content_list_type) {
		static const GtkTypeInfo list_info = {
			"GtkExifContentList",
			sizeof (GtkExifContentList),
			sizeof (GtkExifContentListClass),
			(GtkClassInitFunc)  gtk_exif_content_list_class_init,
			(GtkObjectInitFunc) gtk_exif_content_list_init,
			NULL, NULL, NULL};
		content_list_type = gtk_type_unique (PARENT_TYPE, &list_info);
	}

	return (content_list_type);
}

static void
on_hide (GtkWidget *widget, GtkMenu *menu)
{
	gtk_object_unref (GTK_OBJECT (menu));
}

static void
on_remove_activate (GtkMenuItem *item, GtkExifContentList *list)
{
	ExifEntry *entry;
	guint row;

	while (GTK_CLIST (list)->selection) {
		row = GPOINTER_TO_INT (GTK_CLIST (list)->selection->data);
		entry = gtk_clist_get_row_data (GTK_CLIST (list), row);
		exif_entry_ref (entry);
		gtk_clist_remove (GTK_CLIST (list), row);
		gtk_signal_emit (GTK_OBJECT (list), signals[ENTRY_REMOVED],
				 entry);
		exif_entry_unref (entry);
	}
}

static void
on_tag_selected (GtkExifTagMenu *menu, ExifTag tag, GtkExifContentList *list)
{
	ExifEntry *entry;

	entry = exif_entry_new ();
	exif_content_add_entry (list->content, entry);
	exif_entry_initialize (entry, tag);
	gtk_exif_content_list_add_entry (list, entry);
	exif_entry_unref (entry);
}

static gint
on_button_press_event (GtkWidget *widget, GdkEventButton *event, 
		       GtkExifContentList *list)
{
	GtkWidget *menu, *item, *smenu;

	g_return_val_if_fail (GTK_EXIF_IS_CONTENT_LIST (list), FALSE);

	switch (event->button) {
	case 3:
		menu = gtk_menu_new ();
		gtk_widget_show (menu);
		gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
				event->button, event->time);
		gtk_signal_connect (GTK_OBJECT (menu), "hide",
				    GTK_SIGNAL_FUNC (on_hide), menu);

		/* Add */
		item = gtk_menu_item_new_with_label ("Add");
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU (menu), item);
		smenu = gtk_exif_tag_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), smenu);
		gtk_signal_connect (GTK_OBJECT (smenu), "tag_selected",
				GTK_SIGNAL_FUNC (on_tag_selected), list);

		/* Remove */
		item = gtk_menu_item_new_with_label ("Remove");
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU (menu), item);
		gtk_signal_connect (GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (on_remove_activate), list);

		return (TRUE);
	default:
		return (FALSE);
	}
}

static void
on_select_row (GtkCList *list, gint row, gint col, GdkEvent *event)
{
	gtk_signal_emit (GTK_OBJECT (list), signals[ENTRY_SELECTED],
			 gtk_clist_get_row_data (list, row));
}

GtkWidget *
gtk_exif_content_list_new (void)
{
	GtkExifContentList *list;
	const gchar *titles[] = {"Tag", "Value"};

	list = gtk_type_new (GTK_EXIF_TYPE_CONTENT_LIST);
	gtk_clist_construct (GTK_CLIST (list), 2, (gchar **) titles);
	gtk_clist_set_sort_column (GTK_CLIST (list), 0);
	gtk_clist_set_auto_sort (GTK_CLIST (list), TRUE);

	gtk_signal_connect (GTK_OBJECT (list), "select_row",
			    GTK_SIGNAL_FUNC (on_select_row), list);
	gtk_signal_connect (GTK_OBJECT (list), "button_press_event",
			    GTK_SIGNAL_FUNC (on_button_press_event), list);

	return (GTK_WIDGET (list));
}

static void
row_destroy_notify (gpointer data)
{
	exif_entry_unref ((ExifEntry *) data);
}

void
gtk_exif_content_list_add_entry (GtkExifContentList *list, ExifEntry *entry)
{
	gint row;
	const gchar *text[2];

	text[0] = exif_tag_get_name (entry->tag);
	text[1] = exif_entry_get_value (entry);
	row = gtk_clist_append (GTK_CLIST (list), (gchar **) text);
	exif_entry_ref (entry);
	gtk_clist_set_row_data_full (GTK_CLIST (list), row, entry,
				     row_destroy_notify);
	gtk_signal_emit (GTK_OBJECT (list), signals[ENTRY_ADDED], entry);
}

void
gtk_exif_content_list_set_content (GtkExifContentList *list,
				   ExifContent *content)
{
	guint i;

	g_return_if_fail (GTK_EXIF_IS_CONTENT_LIST (list));
	g_return_if_fail (content != NULL);

	if (list->content)
		exif_content_unref (list->content);
	list->content = content;
	exif_content_ref (content);

	gtk_clist_clear (GTK_CLIST (list));

	for (i = 0; i < content->count; i++)
		gtk_exif_content_list_add_entry (list, content->entries[i]);
	gtk_clist_set_column_auto_resize (GTK_CLIST (list), 0, TRUE);
	gtk_clist_set_column_auto_resize (GTK_CLIST (list), 1, TRUE);
	gtk_clist_sort (GTK_CLIST (list));
}
