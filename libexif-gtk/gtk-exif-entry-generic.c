/* gtk-exif-entry-generic.c
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
#include "gtk-exif-entry-generic.h"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtktable.h>

struct _GtkExifEntryGenericPrivate {
	ExifEntry *entry;

	GtkToggleButton *c;
	GtkToggleButton *r1, *r2, *r3;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_generic_destroy (GtkObject *object)
{
	GtkExifEntryGeneric *entry = GTK_EXIF_ENTRY_GENERIC (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_generic_finalize (GtkObject *object)
{
	GtkExifEntryGeneric *entry = GTK_EXIF_ENTRY_GENERIC (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_generic_class_init (GtkExifEntryGenericClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_generic_destroy;
	object_class->finalize = gtk_exif_entry_generic_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_generic_init (GtkExifEntryGeneric *entry)
{
	entry->priv = g_new0 (GtkExifEntryGenericPrivate, 1);
}

GtkType
gtk_exif_entry_generic_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryGeneric",
			sizeof (GtkExifEntryGeneric),
			sizeof (GtkExifEntryGenericClass),
			(GtkClassInitFunc)  gtk_exif_entry_generic_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_generic_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

GtkWidget *
gtk_exif_entry_generic_new (ExifEntry *e)
{
	GtkExifEntryGeneric *entry;
	GtkWidget *table, *label;
	gchar *txt;

	g_return_val_if_fail (e != NULL, NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_GENERIC);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	table = gtk_table_new (2, 4, FALSE);
	gtk_widget_show (table);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, TRUE, 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);

	label = gtk_label_new ("Format:");
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, 0, 0, 0, 0);
	txt = g_strdup_printf ("%i ('%s')", e->format,
			       exif_format_get_name (e->format));
	label = gtk_label_new (txt);
	g_free (txt);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, 0, 0, 0, 0);

	label = gtk_label_new ("Components:");
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, 0, 0, 0, 0);
	txt = g_strdup_printf ("%i", (int) e->components);
	label = gtk_label_new (txt);
	g_free (txt);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2, 0, 0, 0, 0);

	label = gtk_label_new ("Size:");
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, 0, 0, 0, 0);
	txt = g_strdup_printf ("%i", e->size);
	label = gtk_label_new (txt);
	g_free (txt);
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 2, 3, 0, 0, 0, 0);

	label = gtk_label_new ("Value:");
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, 0, 0, 0, 0);
	label = gtk_label_new (exif_entry_get_value (e));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 1, 2, 3, 4, 0, 0, 0, 0);

	return (GTK_WIDGET (entry));
}
