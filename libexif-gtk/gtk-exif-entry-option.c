/* gtk-exif-entry-option.c
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
#include "gtk-exif-entry-option.h"

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

#include "gtk-extensions/gtk-option-menu-option.h"
#include "gtk-extensions/gtk-options.h"

struct _GtkExifEntryOptionPrivate {
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
gtk_exif_entry_option_destroy (GtkObject *object)
{
	GtkExifEntryOption *entry = GTK_EXIF_ENTRY_OPTION (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_option_finalize (GtkObject *object)
{
	GtkExifEntryOption *entry = GTK_EXIF_ENTRY_OPTION (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_option_class_init (GtkExifEntryOptionClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_option_destroy;
	object_class->finalize = gtk_exif_entry_option_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_option_init (GtkExifEntryOption *entry)
{
	entry->priv = g_new0 (GtkExifEntryOptionPrivate, 1);
}

GtkType
gtk_exif_entry_option_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryOption",
			sizeof (GtkExifEntryOption),
			sizeof (GtkExifEntryOptionClass),
			(GtkClassInitFunc)  gtk_exif_entry_option_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_option_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

static void
gtk_exif_entry_option_load (GtkExifEntryOption *entry)
{
	ExifShort value;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_OPTION (entry));

	value = exif_get_short (entry->priv->entry->data,
				entry->priv->entry->order);
	gtk_option_menu_option_set (entry->priv->menu, value);
}

static void
gtk_exif_entry_option_save (GtkExifEntryOption *entry)
{
	ExifShort value;

	value = gtk_option_menu_option_get (entry->priv->menu);
	exif_set_short (entry->priv->entry->data, entry->priv->entry->order,
			value);
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

static void
on_option_selected (GtkOptions *options, guint option, GtkExifEntryOption *entry)
{
	gtk_exif_entry_option_save (entry);
}

static GtkOptions options_sensing_method[] = {
	{  1, N_("Not defined")},
	{  2, N_("One-chip color area sensor")},
	{  3, N_("Two-chip color area sensor")},
	{  4, N_("Three-chip color area sensor")},
	{  5, N_("Color sequential area sensor")},
	{  7, N_("Trilinear sensor")},
	{  8, N_("Color sequential linear sensor")},
	{  0, NULL}
};

static GtkOptions options_compression[] = {
	{1, N_("Uncompressed")},
	{6, N_("JPEG compression")},
	{0, NULL}
};

static GtkOptions options_metering_mode[] = {
        {  0, N_("Unknown")},
        {  1, N_("Average")},
        {  2, N_("Center-Weighted Average")},
        {  3, N_("Spot")},
        {  4, N_("Multi Spot")},
        {  5, N_("Pattern")},
        {  6, N_("Partial")},
        {255, N_("Other")},
        {  0, NULL}
};

static GtkOptions options_light_source[] = {
        {  0, N_("Unknown")},
        {  1, N_("Daylight")},
        {  2, N_("Fluorescent")},
        {  3, N_("Tungsten")},
        { 17, N_("Standard light A")},
        { 18, N_("Standard light B")},
        { 19, N_("Standard light C")},
        { 20, N_("D55")},
        { 21, N_("D65")},
        { 22, N_("D75")},
        {255, N_("Other")},
        {  0, NULL}
};

static GtkOptions options_orientation[] = {
        {1, N_("top - left")},
        {2, N_("top - right")},
        {3, N_("bottom - right")},
        {4, N_("bottom - left")},
        {5, N_("left - top")},
        {6, N_("right - top")},
        {7, N_("right - bottom")},
        {8, N_("left - bottom")},
        {0, NULL}
};

static GtkOptions options_ycbcr_positioning[] = {
        {  1, N_("centered")},
        {  2, N_("co-sited")},
        {  0, NULL}
};

GtkWidget *
gtk_exif_entry_option_new (ExifEntry *e)
{
	GtkExifEntryOption *entry;
	GtkWidget *hbox, *label, *menu;
	GtkOptions *options;
	const gchar *title;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_SHORT, NULL);
	g_return_val_if_fail ((e->tag == EXIF_TAG_SENSING_METHOD) ||
			      (e->tag == EXIF_TAG_METERING_MODE) ||
			      (e->tag == EXIF_TAG_LIGHT_SOURCE) ||
			      (e->tag == EXIF_TAG_ORIENTATION) ||
			      (e->tag == EXIF_TAG_YCBCR_POSITIONING) ||
			      (e->tag == EXIF_TAG_COMPRESSION), NULL);

	switch (e->tag) {
	case EXIF_TAG_SENSING_METHOD:
		title = N_("Sensing method:");
		options = options_sensing_method;
		break;
	case EXIF_TAG_ORIENTATION:
		title = N_("0-th row - 0-th column:");
		options = options_orientation;
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		title = N_("Light source:");
		options = options_light_source;
		break;
	case EXIF_TAG_METERING_MODE:
		title = N_("Metering mode:");
		options = options_metering_mode;
		break;
	case EXIF_TAG_COMPRESSION:
		title = N_("Compression scheme:");
		options = options_compression;
		break;
	case EXIF_TAG_YCBCR_POSITIONING:
		title = N_("YCbCr Positioning:");
		options = options_ycbcr_positioning;
		break;
	default:
		return (NULL);
	}

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_OPTION);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			exif_tag_get_title (e->tag),
			exif_tag_get_description (e->tag));

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	label = gtk_label_new (_(title));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	menu = gtk_option_menu_option_new (options);
	gtk_widget_show (menu);
	gtk_box_pack_start (GTK_BOX (hbox), menu, FALSE, FALSE, 0);
	entry->priv->menu = GTK_OPTION_MENU_OPTION (menu);
	gtk_signal_connect (GTK_OBJECT (menu), "option_selected",
			    GTK_SIGNAL_FUNC (on_option_selected), entry);

	gtk_exif_entry_option_load (entry);

	return (GTK_WIDGET (entry));
}
