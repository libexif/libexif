/* gtk-exif-browser.c
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
#include "gtk-exif-browser.h"

#include <gtk/gtksignal.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkscrolledwindow.h>

#include "gtk-exif-tree.h"
#include "gtk-exif-content-list.h"
#include "gtk-exif-entry-ascii.h"
#include "gtk-exif-entry-date.h"
#include "gtk-exif-entry-exposure.h"
#include "gtk-exif-entry-flash.h"
#include "gtk-exif-entry-generic.h"
#include "gtk-exif-entry-light.h"
#include "gtk-exif-entry-meter.h"
#include "gtk-exif-entry-orientation.h"
#include "gtk-exif-entry-resolution.h"
#include "gtk-exif-entry-sensing.h"
#include "gtk-exif-entry-version.h"
#include "gtk-exif-entry-ycbcrpos.h"

struct _GtkExifBrowserPrivate {
	ExifData *data;

	GtkExifContentList *list;
	GtkWidget *empty, *current, *info;
};

#define PARENT_TYPE gtk_hpaned_get_type()
static GtkHPanedClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_browser_destroy (GtkObject *object)
{
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (object);

	if (browser->priv->data) {
		exif_data_unref (browser->priv->data);
		browser->priv->data = NULL;
	}

	if (browser->priv->empty) {
		gtk_widget_unref (browser->priv->empty);
		browser->priv->empty = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_browser_finalize (GtkObject *object)
{
	GtkExifBrowser *browser = GTK_EXIF_BROWSER (object);

	g_free (browser->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_browser_class_init (GtkExifBrowserClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_browser_destroy;
	object_class->finalize = gtk_exif_browser_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_browser_init (GtkExifBrowser *browser)
{
	browser->priv = g_new0 (GtkExifBrowserPrivate, 1);
}

GtkType
gtk_exif_browser_get_type (void)
{
	static GtkType browser_type = 0;

	if (!browser_type) {
		static const GtkTypeInfo browser_info = {
			"GtkExifBrowser",
			sizeof (GtkExifBrowser),
			sizeof (GtkExifBrowserClass),
			(GtkClassInitFunc)  gtk_exif_browser_class_init,
			(GtkObjectInitFunc) gtk_exif_browser_init,
			NULL, NULL, NULL};
		browser_type = gtk_type_unique (PARENT_TYPE, &browser_info);
	}

	return (browser_type);
}

static void
gtk_exif_browser_set_widget (GtkExifBrowser *browser, GtkWidget *w)
{
	if (browser->priv->current)
		gtk_container_remove (GTK_CONTAINER (browser->priv->info),
				      browser->priv->current);
	gtk_box_pack_start (GTK_BOX (browser->priv->info), w, TRUE, FALSE, 0);
	browser->priv->current = w;
}

static void
on_entry_selected (GtkExifContentList *list, ExifEntry *entry,
		   GtkExifBrowser *browser)
{
	GtkWidget *w;

	switch (entry->tag) {
	case EXIF_TAG_EXIF_OFFSET:
	case EXIF_TAG_INTEROPERABILITY_OFFSET:
		w = gtk_exif_content_list_new ();
		gtk_exif_content_list_set_content (GTK_EXIF_CONTENT_LIST (w),
						   entry->content);
		break;
	case EXIF_TAG_EXIF_VERSION:
	case EXIF_TAG_FLASH_PIX_VERSION:
		w = gtk_exif_entry_version_new (entry);
		break;
	case EXIF_TAG_FLASH:
		w = gtk_exif_entry_flash_new (entry);
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		w = gtk_exif_entry_exposure_new (entry);
		break;
	case EXIF_TAG_SENSING_METHOD:
		w = gtk_exif_entry_sensing_new (entry);
		break;
	case EXIF_TAG_ORIENTATION:
		w = gtk_exif_entry_orientation_new (entry);
		break;
	case EXIF_TAG_METERING_MODE:
		w = gtk_exif_entry_meter_new (entry);
		break;
	case EXIF_TAG_YCBCR_POSITIONING:
		w = gtk_exif_entry_ycbcrpos_new (entry);
		break;
	case EXIF_TAG_RESOLUTION_UNIT:
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_Y_RESOLUTION:
		w = gtk_exif_entry_resolution_new (entry->parent, FALSE);
		break;
	case EXIF_TAG_FOCAL_PLANE_X_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
		w = gtk_exif_entry_resolution_new (entry->parent, TRUE);
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		w = gtk_exif_entry_light_new (entry);
		break;
	case EXIF_TAG_MAKE:
	case EXIF_TAG_MODEL:
	case EXIF_TAG_IMAGE_DESCRIPTION:
	case EXIF_TAG_SOFTWARE:
	case EXIF_TAG_ARTIST:
		w = gtk_exif_entry_ascii_new (entry);
		break;
	case EXIF_TAG_DATE_TIME:
	case EXIF_TAG_DATE_TIME_ORIGINAL:
	case EXIF_TAG_DATE_TIME_DIGITIZED:
		w = gtk_exif_entry_date_new (entry);
		break;
	default:
		w = gtk_exif_entry_generic_new (entry);
		break;
	}
	gtk_widget_show (w);
	gtk_exif_browser_set_widget (browser, w);
}

GtkWidget *
gtk_exif_browser_new (void)
{
	GtkWidget *et, *swin;
	GtkExifBrowser *browser;

	browser = gtk_type_new (GTK_EXIF_TYPE_BROWSER);

	/* Tree */
	swin = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (swin);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
				GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1 (GTK_PANED (browser), swin, TRUE, TRUE);
	et = gtk_exif_content_list_new ();
	gtk_widget_show (et);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (swin), et);
	browser->priv->list = GTK_EXIF_CONTENT_LIST (et);
	gtk_signal_connect (GTK_OBJECT (et), "entry_selected",
			    GTK_SIGNAL_FUNC (on_entry_selected), browser);

	/* Info */
	browser->priv->info = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (browser->priv->info);
	gtk_paned_pack2 (GTK_PANED (browser), browser->priv->info, TRUE, FALSE);

	/* Placeholder */
	browser->priv->empty = gtk_label_new ("Nothing selected.");
	gtk_widget_show (browser->priv->empty);
	gtk_object_ref (GTK_OBJECT (browser->priv->empty));
	gtk_exif_browser_set_widget (browser, browser->priv->empty);

	return (GTK_WIDGET (browser));
}

void
gtk_exif_browser_set_data (GtkExifBrowser *browser, ExifData *data)
{
	g_return_if_fail (GTK_EXIF_IS_BROWSER (browser));
	g_return_if_fail (data != NULL);

	if (browser->priv->data)
		exif_data_unref (browser->priv->data);
	browser->priv->data = data;
	exif_data_ref (data);

	gtk_exif_content_list_set_content (browser->priv->list, data->content);
	gtk_exif_browser_set_widget (browser, browser->priv->empty);
}
