/* gtk-exif-entry-version.c
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
#include "gtk-exif-entry-version.h"

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

#include "gtk-extensions/gtk-options.h"

struct _GtkExifEntryVersionPrivate {
	ExifEntry *entry;

	GtkOptions *options;
};

#define PARENT_TYPE GTK_EXIF_TYPE_ENTRY
static GtkExifEntryClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_entry_version_destroy (GtkObject *object)
{
	GtkExifEntryVersion *entry = GTK_EXIF_ENTRY_VERSION (object);

	if (entry->priv->entry) {
		exif_entry_unref (entry->priv->entry);
		entry->priv->entry = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_entry_version_finalize (GtkObject *object)
{
	GtkExifEntryVersion *entry = GTK_EXIF_ENTRY_VERSION (object);

	g_free (entry->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_entry_version_class_init (GtkExifEntryVersionClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_entry_version_destroy;
	object_class->finalize = gtk_exif_entry_version_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_entry_version_init (GtkExifEntryVersion *entry)
{
	entry->priv = g_new0 (GtkExifEntryVersionPrivate, 1);
}

GtkType
gtk_exif_entry_version_get_type (void)
{
	static GtkType entry_type = 0;

	if (!entry_type) {
		static const GtkTypeInfo entry_info = {
			"GtkExifEntryVersion",
			sizeof (GtkExifEntryVersion),
			sizeof (GtkExifEntryVersionClass),
			(GtkClassInitFunc)  gtk_exif_entry_version_class_init,
			(GtkObjectInitFunc) gtk_exif_entry_version_init,
			NULL, NULL, NULL};
		entry_type = gtk_type_unique (PARENT_TYPE, &entry_info);
	}

	return (entry_type);
}

typedef enum _ExifVersion ExifVersion;
enum _ExifVersion {
	EXIF_VERSION_2_0,
	EXIF_VERSION_2_1
};

typedef enum _FlashPixVersion FlashPixVersion;
enum _FlashPixVersion {
	FLASH_PIX_VERSION_1
};

static GtkOptionsList exif_list[] = {
        {EXIF_VERSION_2_0, N_("Exif Format Version 2.0")},
        {EXIF_VERSION_2_1, N_("Exif Format Version 2.1")},
        {0, NULL}
};

static GtkOptionsList flash_pix_list[] = {
        {FLASH_PIX_VERSION_1, N_("FlashPix Format Version 1.0")},
        {0, NULL}
};

static struct {
	ExifVersion version;
	const guchar *data;
} exif_versions[] = {
	{EXIF_VERSION_2_0, "0200"},
	{EXIF_VERSION_2_1, "0210"},
	{0, NULL}
};

static struct {
	FlashPixVersion version;
	const guchar *data;
} flash_pix_versions[] = {
	{FLASH_PIX_VERSION_1, "0100"},
	{0, NULL}
};

static void
gtk_exif_entry_version_load (GtkExifEntryVersion *entry)
{
	guint i;

	g_return_if_fail (GTK_EXIF_IS_ENTRY_VERSION (entry));

	gtk_signal_handler_block_by_data (GTK_OBJECT (entry->priv->options),
					  entry);
	switch (entry->priv->entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
		for (i = 0; exif_versions[i].data; i++)
			if (!memcmp (exif_versions[i].data,
				     entry->priv->entry->data, 4)) {
				gtk_options_set (entry->priv->options,
						 exif_versions[i].version);
				break;
			}
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		for (i = 0; flash_pix_versions[i].data; i++)
			if (!memcmp (flash_pix_versions[i].data, 
				     entry->priv->entry->data, 4)) {
				gtk_options_set (entry->priv->options, 
						 flash_pix_versions[i].version);
				break;
			}
		break;
	default:
		break;
	}
	gtk_signal_handler_unblock_by_data (GTK_OBJECT (entry->priv->options),
					    entry);
}

static void
gtk_exif_entry_version_save (GtkExifEntryVersion *entry)
{
	guint option, i;

	option = gtk_options_get (entry->priv->options);
	switch (entry->priv->entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
		for (i = 0; exif_versions[i].data; i++)
			if (option == exif_versions[i].version) {
				memcpy (entry->priv->entry->data,
					exif_versions[i].data, 4);
				break;
		}
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		for (i = 0; flash_pix_versions[i].data; i++)
			if (option == flash_pix_versions[i].version) {
				memcpy (entry->priv->entry->data,
					flash_pix_versions[i].data, 4);
				break;
			}
		break;
	default:
		break;
	}
	gtk_signal_emit_by_name (GTK_OBJECT (entry), "entry_changed",
				 entry->priv->entry);
}

static void
on_option_selected (GtkOptions *options, guint option, GtkExifEntryVersion *entry)
{
	gtk_exif_entry_version_save (entry);
}

GtkWidget *
gtk_exif_entry_version_new (ExifEntry *e)
{
	GtkExifEntryVersion *entry;
	GtkWidget *hbox, *label, *options;

	g_return_val_if_fail (e != NULL, NULL);
	g_return_val_if_fail ((e->tag == EXIF_TAG_EXIF_VERSION) ||
			      (e->tag == EXIF_TAG_FLASH_PIX_VERSION), NULL);
	g_return_val_if_fail (e->format == EXIF_FORMAT_UNDEFINED, NULL);
	g_return_val_if_fail (e->components == 4, NULL);
	g_return_val_if_fail (e->data != NULL, NULL);

	entry = gtk_type_new (GTK_EXIF_TYPE_ENTRY_VERSION);
	entry->priv->entry = e;
	exif_entry_ref (e);
	gtk_exif_entry_construct (GTK_EXIF_ENTRY (entry),
			exif_tag_get_title (e->tag),
			exif_tag_get_description (e->tag));

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (entry), hbox, TRUE, FALSE, 0);
	label = gtk_label_new (_("Version:"));
	gtk_widget_show (label);
	gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
	switch (e->tag) {
	case EXIF_TAG_EXIF_VERSION:
		options = gtk_options_new (exif_list);
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
	default:
		options = gtk_options_new (flash_pix_list);
		break;
	}
	gtk_widget_show (options);
	gtk_box_pack_start (GTK_BOX (hbox), options, FALSE, FALSE, 0);
	entry->priv->options = GTK_OPTIONS (options);
	gtk_signal_connect (GTK_OBJECT (options), "option_selected",
			    GTK_SIGNAL_FUNC (on_option_selected), entry);

	gtk_exif_entry_version_load (entry);

	return (GTK_WIDGET (entry));
}
