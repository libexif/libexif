/* gtk-exif-entry-copyright.c
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
#include "gtk-exif-entry-copyright.h"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtktable.h>
#include <gtk/gtkhbox.h>

#include <libexif/exif-i18n.h>

#include <string.h>

struct _GtkExifEntryCopyrightPrivate {
	ExifEntry *entry;

	GtkEntry *photographer, *editor;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_copyright_destroy (GtkObject *object)
{
	GtkExifEntryCopyright *entry = GTK_EXIF_ENTRY_COPYRIGHT (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_copyright_finalize (GtkObject *object)
{
	GtkExifEntryCopyright *entry = GTK_EXIF_ENTRY_COPYRIGHT (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_copyright_class_init (GtkExifEntryCopyrightClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_copyright_destroy;
	object_class->finalize = gtk_exif_entry_copyright_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_copyright_init (GtkExifEntryCopyright *entry)
{
	entry->priv = g_new0 (GtkExifEntryCopyrightPrivate, 1);
}

GtkType
gtk_exif_entry_copyright_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryCopyright",
			sizeof (GtkExifEntryCopyright),
			sizeof (GtkExifEntryCopyrightClass),
			(GtkClassInitFunc)  gtk_exif_entry_copyright_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_copyright_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
on_text_changed (GtkEditable *editable, GtkExifEntryCopyright *entry)
{
	gchar *photographer, *editor;

	if (entry->priv->entry->data) {
		g_free (entry->priv->entry->data);
		entry->priv->entry->data = NULL;
		entry->priv->entry->size = 0;
	}

	photographer = gtk_editable_get_chars (
			GTK_EDITABLE (entry->priv->photographer), 0, -1);
	editor = gtk_editable_get_chars (
			GTK_EDITABLE (entry->priv->editor), 0, -1);
	entry->priv->entry->data = g_strdup_printf ("%s %s", photographer,
						    editor);
	entry->priv->entry->data[strlen (photographer)] = '\0';
	entry->priv->entry->size = strlen (photographer) + 1 +  
				   strlen (editor) + 1;
	entry->priv->entry->components = entry->priv->entry->size;
	g_free (photographer);
	g_free (editor);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

GtkWidget *
gtk_exif_entry_copyright_new (ExifEntry *e)
{
	GtkExifEntryCopyright *entry;
	GtkWidget *widget, *label, *table;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_ASCII, NULL);
	g_return_val_if_fail (e->tag == EXIF_TAG_COPYRIGHT, NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_COPYRIGHT);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	table = gtk_table_new (2, 2, FALSE);
	gtk_widget_show (table);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, FALSE, 0);
	label = gtk_label_new (_("Photographer:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1,
			  GTK_FILL, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	label = gtk_label_new (_("Editor:"));
	gtk_widget_show (label);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
			  GTK_FILL, 0, 0, 0);
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	widget = gtk_entry_new ();
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE (table), widget, 1, 2, 0, 1,
			  GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_entry_set_text (GTK_ENTRY (widget), e->data);
	gtk_signal_connect (GTK_OBJECT (widget), "changed",
			    GTK_SIGNAL_FUNC (on_text_changed), entry);
	entry->priv->photographer = GTK_ENTRY (widget);
	widget = gtk_entry_new ();
	gtk_widget_show (widget);
	gtk_table_attach (GTK_TABLE (table), widget, 1, 2, 1, 2,
			  GTK_FILL | GTK_EXPAND, 0, 0, 0);
	gtk_entry_set_text (GTK_ENTRY (widget), e->data + strlen (e->data) + 1);
	gtk_signal_connect (GTK_OBJECT (widget), "changed",
			    GTK_SIGNAL_FUNC (on_text_changed), entry);
	entry->priv->editor = GTK_ENTRY (widget);

	return (GTK_WIDGET (entry));
}
