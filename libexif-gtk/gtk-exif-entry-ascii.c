/* gtk-exif-entry-ascii.c
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
#include "gtk-exif-entry-ascii.h"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>

#include <string.h>

struct _GtkExifEntryAsciiPrivate {
	ExifEntry *entry;

	GtkEntry *gtk_entry;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_ascii_destroy (GtkObject *object)
{
	GtkExifEntryAscii *entry = GTK_EXIF_ENTRY_ASCII (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_ascii_finalize (GtkObject *object)
{
	GtkExifEntryAscii *entry = GTK_EXIF_ENTRY_ASCII (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_ascii_class_init (GtkExifEntryAsciiClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_ascii_destroy;
	object_class->finalize = gtk_exif_entry_ascii_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_ascii_init (GtkExifEntryAscii *entry)
{
	entry->priv = g_new0 (GtkExifEntryAsciiPrivate, 1);
}

GtkType
gtk_exif_entry_ascii_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryAscii",
			sizeof (GtkExifEntryAscii),
			sizeof (GtkExifEntryAsciiClass),
			(GtkClassInitFunc)  gtk_exif_entry_ascii_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_ascii_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
on_text_changed (GtkEditable *editable, GtkExifEntryAscii *entry)
{
	gchar *txt;

	txt = gtk_editable_get_chars (editable, 0, -1);
	g_free (entry->priv->entry->data);
	entry->priv->entry->data = txt;
	entry->priv->entry->size = strlen (txt) + 1;
	entry->priv->entry->components = entry->priv->entry->size;
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

GtkWidget *
gtk_exif_entry_ascii_new (ExifEntry *e)
{
	GtkExifEntryAscii *entry;
	GtkWidget *widget;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_ASCII, NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_ASCII);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	widget = gtk_entry_new ();
	gtk_widget_show (widget);
	gtk_box_pack_start (GTK_BOX (entry), widget, TRUE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (widget), e->data);
	gtk_signal_connect (GTK_OBJECT (widget), "changed",
			    GTK_SIGNAL_FUNC (on_text_changed), entry);

	return (GTK_WIDGET (entry));
}
