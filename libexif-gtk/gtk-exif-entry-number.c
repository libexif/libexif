/* gtk-exif-entry-number.c
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
#include "gtk-exif-entry-number.h"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtktable.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtktable.h>

#include <libexif/exif-i18n.h>

struct _GtkExifEntryNumberPrivate {
	ExifEntry *entry;

	GPtrArray *a;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_number_destroy (GtkObject *object)
{
	GtkExifEntryNumber *entry = GTK_EXIF_ENTRY_NUMBER (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	if (entry->priv->a) {
		g_ptr_array_free (entry->priv->a, TRUE);
		entry->priv->a = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_number_finalize (GtkObject *object)
{
	GtkExifEntryNumber *entry = GTK_EXIF_ENTRY_NUMBER (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_number_class_init (GtkExifEntryNumberClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_number_destroy;
	object_class->finalize = gtk_exif_entry_number_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_number_init (GtkExifEntryNumber *entry)
{
	entry->priv = g_new0 (GtkExifEntryNumberPrivate, 1);
	entry->priv->a = g_ptr_array_new ();
}

GtkType
gtk_exif_entry_number_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryNumber",
			sizeof (GtkExifEntryNumber),
			sizeof (GtkExifEntryNumberClass),
			(GtkClassInitFunc)  gtk_exif_entry_number_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_number_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
gtk_exif_entry_number_load (GtkExifEntryNumber *entry)
{
	ExifByte v_byte;
	ExifShort v_short;
	ExifLong v_long;
	ExifSLong v_slong;
	GtkAdjustment *a;
	ExifEntry *e;
	guint i;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_NUMBER (entry));

	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		a = entry->priv->a->pdata[i];
		gtk_signal_handler_block_by_data (GTK_OBJECT (a), entry);
		switch (e->format) {
		case EXIF_FORMAT_BYTE:
			v_byte = e->data[i];
			gtk_adjustment_set_value (a, v_byte);
			break;
		case EXIF_FORMAT_SHORT:
			v_short = exif_get_short (e->data + 2 * i, e->order);
			gtk_adjustment_set_value (a, v_short);
			break;
		case EXIF_FORMAT_LONG:
			v_long = exif_get_long (e->data + 4 * i, e->order);
			gtk_adjustment_set_value (a, v_long);
			break;
		case EXIF_FORMAT_SLONG:
			v_slong = exif_get_slong (e->data + 4 * i, e->order);
			gtk_adjustment_set_value (a, v_slong);
			break;
		default:
			g_warning ("Invalid format!");
			break;
		}
		gtk_signal_handler_unblock_by_data (GTK_OBJECT (a), entry);
	}
}

static void
gtk_exif_entry_number_save (GtkExifEntryNumber *entry)
{
	ExifEntry *e;
	GtkAdjustment *a;
	guint i;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_NUMBER (entry));

	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		a = entry->priv->a->pdata[i];
		switch (e->format) {
		case EXIF_FORMAT_BYTE:
			e->data[i] = a->value;
			break;
		case EXIF_FORMAT_SHORT:
			exif_set_short (e->data + 2 * i, e->order, a->value);
			break;
		case EXIF_FORMAT_LONG:
			exif_set_long (e->data + 4 * i, e->order, a->value);
			break;
		case EXIF_FORMAT_SLONG:
			exif_set_slong (e->data + 4 * i, e->order, a->value);
			break;
		default:
			g_warning ("Invalid format!");
			return;
		}
	}
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_adjustment_value_changed (GtkAdjustment *adj, GtkExifEntryNumber *entry)
{
	gtk_exif_entry_number_save (entry);
}

GtkWidget *
gtk_exif_entry_number_new (ExifEntry *e)
{
	GtkExifEntryNumber *entry;
	GtkWidget *table, *label, *spin;
	GtkObject *a;
	gchar *txt;
	guint i;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->format == EXIF_FORMAT_BYTE) ||
			      (e->format == EXIF_FORMAT_SHORT) ||
			      (e->format == EXIF_FORMAT_LONG) ||
			      (e->format == EXIF_FORMAT_SLONG), NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_NUMBER);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	table = gtk_table_new (2, 1, FALSE);
	gtk_widget_show (table);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, TRUE, 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);

	g_ptr_array_set_size (entry->priv->a, e->components);
	for (i = 0; i < e->components; i++) {
		if (e->components > 1)
			txt = g_strdup_printf (_("Value %i:"), i + 1);
		else
			txt = g_strdup (_("Value:"));
		label = gtk_label_new (txt);
		g_free (txt);
		gtk_widget_show (label);
		gtk_table_attach (GTK_TABLE (table), label, 0, 1, i, i + 1,
				  GTK_FILL, 0, 0, 0);
		gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
		gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);

		a = gtk_adjustment_new (0, 0, 0xffff, 1, 0xff, 0);
		spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
		gtk_widget_show (spin);
		gtk_table_attach (GTK_TABLE (table), spin, 1, 2, i, i + 1,
				  GTK_FILL | GTK_EXPAND, 0, 0, 0);
		entry->priv->a->pdata[i] = a;
		gtk_signal_connect (a, "value_changed",
			GTK_SIGNAL_FUNC (on_adjustment_value_changed), entry);
	}

	gtk_exif_entry_number_load (entry);

	return (GTK_WIDGET (entry));
}
