/* gtk-exif-entry-date.c
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
#include "gtk-exif-entry-date.h"

#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkcalendar.h>
#include <gtk/gtkspinbutton.h>
#include <gtk/gtkcalendar.h>

#include <libexif/exif-i18n.h>

struct _GtkExifEntryDatePrivate {
	ExifEntry *entry;

	GtkCalendar *cal;
	GtkAdjustment *a_hour, *a_min, *a_sec;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_date_destroy (GtkObject *object)
{
	GtkExifEntryDate *entry = GTK_EXIF_ENTRY_DATE (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_date_finalize (GtkObject *object)
{
	GtkExifEntryDate *entry = GTK_EXIF_ENTRY_DATE (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_date_class_init (GtkExifEntryDateClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_date_destroy;
	object_class->finalize = gtk_exif_entry_date_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_date_init (GtkExifEntryDate *entry)
{
	entry->priv = g_new0 (GtkExifEntryDatePrivate, 1);
}

GtkType
gtk_exif_entry_date_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryDate",
			sizeof (GtkExifEntryDate),
			sizeof (GtkExifEntryDateClass),
			(GtkClassInitFunc)  gtk_exif_entry_date_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_date_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
gtk_exif_entry_date_load (GtkExifEntryDate *entry)
{
	GtkExifEntryDatePrivate *p;
	gchar *data;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_DATE (entry));
	p = entry->priv;

	gtk_signal_handler_block_by_data (GTK_OBJECT (p->cal), entry);
	gtk_signal_handler_block_by_data (GTK_OBJECT (p->a_hour), entry);
	gtk_signal_handler_block_by_data (GTK_OBJECT (p->a_min), entry);
	gtk_signal_handler_block_by_data (GTK_OBJECT (p->a_sec), entry);

	data = g_strdup (entry->priv->entry->data);
	data[4] = data[7] = data[10] = data[13] = data[16] = '\0';
	gtk_calendar_select_month (p->cal, atoi (data + 5) - 1, atoi (data));
	gtk_calendar_select_day (entry->priv->cal, atoi (data + 8));
	gtk_adjustment_set_value (entry->priv->a_hour, atoi (data + 11));
	gtk_adjustment_set_value (entry->priv->a_min,  atoi (data + 14));
	gtk_adjustment_set_value (entry->priv->a_sec,  atoi (data + 17));
	g_free (data);

	gtk_signal_handler_unblock_by_data (GTK_OBJECT (p->cal), entry);
	gtk_signal_handler_unblock_by_data (GTK_OBJECT (p->a_hour), entry);
	gtk_signal_handler_unblock_by_data (GTK_OBJECT (p->a_min), entry);
	gtk_signal_handler_unblock_by_data (GTK_OBJECT (p->a_sec), entry);
}

static void
gtk_exif_entry_date_save (GtkExifEntryDate *entry)
{
	g_return_if_fail (GTK_EXIF_IS_ENTRY_DATE (entry));

	sprintf (entry->priv->entry->data, "%04i:%02i:%02i %02i:%02i:%02i",
		 entry->priv->cal->year, entry->priv->cal->month + 1,
		 entry->priv->cal->selected_day,
		 (gint) entry->priv->a_hour->value,
		 (gint) entry->priv->a_min->value,
		 (gint) entry->priv->a_sec->value);
	exif_entry_notify (entry->priv->entry, EXIF_ENTRY_EVENT_CHANGED);
}

static void
on_day_selected (GtkCalendar *calendar, GtkExifEntryDate *entry)
{
	gtk_exif_entry_date_save (entry);
}

static void
on_time_changed (GtkAdjustment *adjustment, GtkExifEntryDate *entry)
{
	gtk_exif_entry_date_save (entry);
}

GtkWidget *
gtk_exif_entry_date_new (ExifEntry *e)
{
	GtkExifEntryDate *entry;
	GtkWidget *c, *hbox, *label, *spin;
	GtkObject *a;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->tag == EXIF_TAG_DATE_TIME) ||
			      (e->tag == EXIF_TAG_DATE_TIME_ORIGINAL) ||
			      (e->tag == EXIF_TAG_DATE_TIME_DIGITIZED), NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_DATE);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
		exif_tag_get_title (e->tag),
		exif_tag_get_description (e->tag));

	c = gtk_calendar_new ();
	gtk_widget_show (c);
	gtk_box_pack_start (GTK_BOX (entry), c, TRUE, FALSE, 0);
	entry->priv->cal = GTK_CALENDAR (c);
	gtk_signal_connect (GTK_OBJECT (c), "day_selected",
			    GTK_SIGNAL_FUNC (on_day_selected), entry);

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	label = gtk_label_new (_("Time:"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	a = gtk_adjustment_new (0, 0, 23, 1, 1, 0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
	gtk_widget_show (spin);
	gtk_box_pack_start (GTK_BOX (hbox), spin, FALSE, FALSE, 0);
	entry->priv->a_hour = GTK_ADJUSTMENT (a);
	gtk_signal_connect (a, "value_changed",
			    GTK_SIGNAL_FUNC (on_time_changed), entry);
	label = gtk_label_new (":");
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	a = gtk_adjustment_new (0, 0, 59, 1, 1, 0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
	gtk_widget_show (spin);
	gtk_box_pack_start (GTK_BOX (hbox), spin, FALSE, FALSE, 0);
	entry->priv->a_min = GTK_ADJUSTMENT (a);
	gtk_signal_connect (a, "value_changed",
			    GTK_SIGNAL_FUNC (on_time_changed), entry);
	label = gtk_label_new (":");
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	a = gtk_adjustment_new (0, 0, 59, 1, 1, 0);
	spin = gtk_spin_button_new (GTK_ADJUSTMENT (a), 0, 0);
	gtk_widget_show (spin);
	gtk_box_pack_start (GTK_BOX (hbox), spin, FALSE, FALSE, 0);
	entry->priv->a_sec = GTK_ADJUSTMENT (a);
	gtk_signal_connect (a, "value_changed",
			    GTK_SIGNAL_FUNC (on_time_changed), entry);

	gtk_exif_entry_date_load (entry);

	return (GTK_WIDGET (entry));
}
