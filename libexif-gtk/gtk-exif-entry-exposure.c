/* gtk-exif-entry-exposure.c
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
#include "gtk-exif-entry-exposure.h"

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkradiobutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkframe.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkoptionmenu.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkhbox.h>

#include <libexif/exif-i18n.h>

#include <gtk-extensions/gtk-option-menu-option.h>

struct _GtkExifEntryExposurePrivate {
	ExifEntry *entry;

	GtkOptionMenuOption *menu;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_exposure_destroy (GtkObject *object)
{
	GtkExifEntryExposure *entry = GTK_EXIF_ENTRY_EXPOSURE (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_exposure_finalize (GtkObject *object)
{
	GtkExifEntryExposure *entry = GTK_EXIF_ENTRY_EXPOSURE (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_exposure_class_init (GtkExifEntryExposureClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_exposure_destroy;
	object_class->finalize = gtk_exif_entry_exposure_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_exposure_init (GtkExifEntryExposure *entry)
{
	entry->priv = g_new0 (GtkExifEntryExposurePrivate, 1);
}

GtkType
gtk_exif_entry_exposure_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryExposure",
			sizeof (GtkExifEntryExposure),
			sizeof (GtkExifEntryExposureClass),
			(GtkClassInitFunc)  gtk_exif_entry_exposure_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_exposure_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
gtk_exif_entry_exposure_load (GtkExifEntryExposure *entry)
{
	ExifShort value;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_EXPOSURE (entry));

	value = exif_get_short (entry->priv->entry->data,
				entry->priv->entry->order);
	gtk_option_menu_option_set (entry->priv->menu, value);
}

static void
gtk_exif_entry_exposure_save (GtkExifEntryExposure *entry)
{
	ExifShort value;

	value = gtk_option_menu_option_get (entry->priv->menu);
	exif_set_short (entry->priv->entry->data, entry->priv->entry->order,
			value);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

static void
on_option_selected (GtkOptionMenuOption *menu, guint option,
		    GtkExifEntryExposure *entry)
{
	gtk_exif_entry_exposure_save (entry);
}

static GtkOptions programs[] = {
	{0, N_("Not defined")},
	{1, N_("Manual")},
	{2, N_("Normal program")},
	{3, N_("Aperture priority")},
	{4, N_("Shutter priority")},
	{5, N_("Creative program (biased toward depth of field)")},
	{6, N_("Action program (biased toward fast shutter speed)")},
	{7, N_("Portrait mode (for closeup photos with the "
	       "background out of focus")},
	{8, N_("Landscape mode (for landscape photos with the "
	       "background in focus")},
	{0, NULL}
};

GtkWidget *
gtk_exif_entry_exposure_new (ExifEntry *e)
{
	GtkExifEntryExposure *entry;
	GtkWidget *hbox, *label, *options;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->tag == EXIF_TAG_EXPOSURE_PROGRAM, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_SHORT, NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_EXPOSURE);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			exif_tag_get_title (e->tag),
			exif_tag_get_description (e->tag));

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	label = gtk_label_new (_("Exposure Program:"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	options = gtk_option_menu_option_new (programs);
	gtk_widget_show (options);
	gtk_box_pack_start (GTK_BOX (hbox), options, FALSE, FALSE, 0);
	entry->priv->menu = GTK_OPTION_MENU_OPTION (options);
	gtk_signal_connect (GTK_OBJECT (options), "option_selected",
			    GTK_SIGNAL_FUNC (on_option_selected), entry);

	gtk_exif_entry_exposure_load (entry);

	return (GTK_WIDGET (entry));
}
