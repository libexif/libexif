/* gtk-exif-entry-rational.c
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
#include "gtk-exif-entry-rational.h"

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

struct _GtkExifEntryRationalPrivate {
	ExifEntry *entry;

	GPtrArray *ap, *aq;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_rational_destroy (GtkObject *object)
{
	GtkExifEntryRational *entry = GTK_EXIF_ENTRY_RATIONAL (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	if (entry->priv->ap) {
		g_ptr_array_free (entry->priv->ap, TRUE);
		entry->priv->ap = NULL;
	}

	if (entry->priv->aq) {
		g_ptr_array_free (entry->priv->aq, TRUE);
		entry->priv->aq = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_rational_finalize (GtkObject *object)
{
	GtkExifEntryRational *entry = GTK_EXIF_ENTRY_RATIONAL (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_rational_class_init (GtkExifEntryRationalClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_rational_destroy;
	object_class->finalize = gtk_exif_entry_rational_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_rational_init (GtkExifEntryRational *entry)
{
	entry->priv = g_new0 (GtkExifEntryRationalPrivate, 1);
	entry->priv->ap = g_ptr_array_new ();
	entry->priv->aq = g_ptr_array_new ();
}

GtkType
gtk_exif_entry_rational_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryRational",
			sizeof (GtkExifEntryRational),
			sizeof (GtkExifEntryRationalClass),
			(GtkClassInitFunc)  gtk_exif_entry_rational_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_rational_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
gtk_exif_entry_rational_load (GtkExifEntryRational *entry)
{
	ExifRational r;
	ExifSRational sr;
	GtkAdjustment *ap, *aq;
	ExifEntry *e;
	guint i;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_RATIONAL (entry));

	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		ap = entry->priv->ap->pdata[i];
		aq = entry->priv->aq->pdata[i];
		gtk_signal_handler_block_by_data (GTK_OBJECT (ap), entry);
		gtk_signal_handler_block_by_data (GTK_OBJECT (aq), entry);
		switch (e->format) {
		case EXIF_FORMAT_RATIONAL:
			r = exif_get_rational (e->data + 8 * i, e->order);
			gtk_adjustment_set_value (ap, r.numerator);
			gtk_adjustment_set_value (aq, r.denominator);
			break;
		case EXIF_FORMAT_SRATIONAL:
			sr = exif_get_srational (e->data + 8 * i, e->order);
			gtk_adjustment_set_value (ap, sr.numerator);
			gtk_adjustment_set_value (aq, sr.denominator);
			break;
		default:
			g_warning ("Invalid format!");
			break;
		}
		gtk_signal_handler_unblock_by_data (GTK_OBJECT (ap), entry);
		gtk_signal_handler_unblock_by_data (GTK_OBJECT (aq), entry);
	}
}

static void
gtk_exif_entry_rational_save (GtkExifEntryRational *entry)
{
	ExifRational r;
	ExifSRational sr;
	ExifEntry *e;
	GtkAdjustment *ap, *aq;
	guint i;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_RATIONAL (entry));

	e = entry->priv->entry;
	for (i = 0; i < e->components; i++) {
		ap = entry->priv->ap->pdata[i];
		aq = entry->priv->aq->pdata[i];
		switch (e->format) {
		case EXIF_FORMAT_RATIONAL:
			r.numerator = ap->value;
			r.denominator = aq->value;
			exif_set_rational (e->data + 8 * i, e->order, r);
			break;
		case EXIF_FORMAT_SRATIONAL:
			sr.numerator = ap->value;
			sr.denominator = aq->value;
			exif_set_srational (e->data + 8 * i, e->order, sr);
			break;
		default:
			g_warning ("Invalid format!");
			return;
		}
	}
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed", e);
}

static void
on_adjustment_value_changed (GtkAdjustment *adj, GtkExifEntryRational *entry)
{
	gtk_exif_entry_rational_save (entry);
}

GtkWidget *
gtk_exif_entry_rational_new (ExifEntry *e)
{
	GtkExifEntryRational *entry;
	GtkWidget *table, *label, *spin;
	GtkObject *a;
	gchar *txt;
	guint i;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->format == EXIF_FORMAT_RATIONAL) ||
			      (e->format == EXIF_FORMAT_SRATIONAL), NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_RATIONAL);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	table = gtk_table_new (4, 1, FALSE);
	gtk_widget_show (table);
	gtk_box_pack_start (GTK_BOX (entry), table, TRUE, TRUE, 0);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);
	gtk_table_set_row_spacings (GTK_TABLE (table), 5);

	g_ptr_array_set_size (entry->priv->ap, e->components);
	g_ptr_array_set_size (entry->priv->aq, e->components);
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
		entry->priv->ap->pdata[i] = a;
		gtk_signal_connect (a, "value_changed",
			GTK_SIGNAL_FUNC (on_adjustment_value_changed), entry);

		label = gtk_label_new ("/");
		gtk_widget_show (label);
		gtk_table_attach (GTK_TABLE (table), label, 2, 3, i, i + 1,
				  0, 0, 0, 0);

		a = gtk_adjustment_new (0, 0, 0xffff, 1, 0xff, 0);
		spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
		gtk_widget_show (spin);
		gtk_table_attach (GTK_TABLE (table), spin, 3, 4, i, i + 1,
				  GTK_FILL | GTK_EXPAND, 0, 0, 0);
		entry->priv->aq->pdata[i] = a;
		gtk_signal_connect (a, "value_changed",
			GTK_SIGNAL_FUNC (on_adjustment_value_changed), entry);
	}

	gtk_exif_entry_rational_load (entry);

	return (GTK_WIDGET (entry));
}
