/* gexif-main.c
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
#include "gexif-main.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtktooltips.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkfilesel.h>
#include <gtk/gtkbutton.h>

#include <libexif/exif-i18n.h>
#include <libjpeg/jpeg-data.h>

#include <libexif-gtk/gtk-exif-browser.h>

#include "gexif-thumbnail.h"

struct _GExifMainPrivate
{
	gchar *path;

	JPEGData *data;

	GtkExifBrowser *browser;
};

#define PARENT_TYPE GTK_TYPE_WINDOW
static GtkWindowClass *parent_class;

enum {
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gexif_main_destroy (GtkObject *object)
{
	GExifMain *m = GEXIF_MAIN (object);

	if (m->priv->data) {
		jpeg_data_unref (m->priv->data);
		m->priv->data = NULL;
	}

	if (m->priv->path) {
		g_free (m->priv->path);
		m->priv->path = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gexif_main_finalize (GtkObject *object)
{
	GExifMain *m = GEXIF_MAIN (object);

	g_free (m->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gexif_main_class_init (GExifMainClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gexif_main_destroy;
	object_class->finalize = gexif_main_finalize;

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gexif_main_init (GExifMain *m)
{
	m->priv = g_new0 (GExifMainPrivate, 1);
}

GtkType
gexif_main_get_type (void)
{
	static GtkType main_type = 0;

	if (!main_type) {
		static const GtkTypeInfo main_info = {
			"GExifMain",
			sizeof (GExifMain),
			sizeof (GExifMainClass),
			(GtkClassInitFunc)  gexif_main_class_init,
			(GtkObjectInitFunc) gexif_main_init,
			NULL, NULL, NULL};
		main_type = gtk_type_unique (PARENT_TYPE, &main_info);
	}

	return (main_type);
}

static void
gexif_main_load_data (GExifMain *m, JPEGData *jdata)
{
	ExifData *edata;

	g_return_if_fail (GEXIF_IS_MAIN (m));
	g_return_if_fail (jdata != NULL);

	if (m->priv->data)
		jpeg_data_unref (m->priv->data);
	m->priv->data = jdata;
	jpeg_data_ref (jdata);

	edata = jpeg_data_get_exif_data (jdata);
	if (!edata) {
		g_warning ("No EXIF data found!");
		return;
	}

	gtk_exif_browser_set_data (m->priv->browser, edata);
	exif_data_unref (edata);
}

static void
gexif_main_load_file (GExifMain *m, const gchar *path)
{
	JPEGData *data;

	g_return_if_fail (GEXIF_IS_MAIN (m));
	g_return_if_fail (path != NULL);

	if (m->priv->path)
		g_free (m->priv->path);
	m->priv->path = g_strdup (path);

	data = jpeg_data_new_from_file (path);
	gexif_main_load_data (m, data);
	jpeg_data_unref (data);
}

static void
gexif_main_save_file (GExifMain *m, const gchar *path)
{
	jpeg_data_save_file (m->priv->data, path);
}

static void
on_exit_activate (GtkMenuItem *item, GExifMain *m)
{
	gtk_object_destroy (GTK_OBJECT (m));
}

static void
on_save_activate (GtkMenuItem *item, GExifMain *m)
{
	gexif_main_save_file (m, m->priv->path);
}

static void
on_cancel_clicked (GtkButton *button, GExifMain *m)
{
	GtkWidget *fsel;

	fsel = gtk_widget_get_ancestor (GTK_WIDGET (button), GTK_TYPE_WINDOW);
	gtk_object_destroy (GTK_OBJECT (fsel));
}

static void
on_save_as_ok_clicked (GtkButton *button, GExifMain *m)
{
	GtkWidget *fsel;

	fsel = gtk_widget_get_ancestor (GTK_WIDGET (button),
					GTK_TYPE_FILE_SELECTION);
	gexif_main_save_file (m,
		gtk_file_selection_get_filename (GTK_FILE_SELECTION (fsel)));
	gtk_object_destroy (GTK_OBJECT (fsel));
}

static void
on_save_as_activate (GtkMenuItem *item, GExifMain *m)
{
	GtkWidget *fsel;

	fsel = gtk_file_selection_new (_("Save As..."));
	gtk_widget_show (fsel);
	gtk_window_set_transient_for (GTK_WINDOW (fsel), GTK_WINDOW (m));
	gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (fsel)->ok_button),
			"clicked", GTK_SIGNAL_FUNC (on_save_as_ok_clicked), m);
	gtk_signal_connect (
			GTK_OBJECT (GTK_FILE_SELECTION (fsel)->cancel_button),
			"clicked", GTK_SIGNAL_FUNC (on_cancel_clicked), m);
	gtk_file_selection_set_filename (GTK_FILE_SELECTION (fsel),
					 m->priv->path);
}

static void
on_open_ok_clicked (GtkButton *button, GExifMain *m)
{
	GtkWidget *fsel;

	fsel = gtk_widget_get_ancestor (GTK_WIDGET (button),
					GTK_TYPE_FILE_SELECTION);
	gexif_main_load_file (m, 
		gtk_file_selection_get_filename (GTK_FILE_SELECTION (fsel)));
	gtk_object_destroy (GTK_OBJECT (fsel));
}

static void
on_open_activate (GtkMenuItem *item, GExifMain *m)
{
	GtkWidget *fsel;

	fsel = gtk_file_selection_new (_("Open..."));
	gtk_widget_show (fsel);
	gtk_window_set_transient_for (GTK_WINDOW (fsel), GTK_WINDOW (m));
	gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (fsel)->ok_button),
		"clicked", GTK_SIGNAL_FUNC (on_open_ok_clicked), m);
	gtk_signal_connect (
		GTK_OBJECT (GTK_FILE_SELECTION (fsel)->cancel_button),
		"clicked", GTK_SIGNAL_FUNC (on_cancel_clicked), m);
}

static void
on_about_activate (GtkMenuItem *item, GExifMain *m)
{
	g_warning ("Implement!");
}

static void
on_thumbnail_activate (GtkMenuItem *item, GExifMain *m)
{
	GtkWidget *dialog;
	ExifData *edata;

	edata = jpeg_data_get_exif_data (m->priv->data);
	if (edata && edata->size) {
		dialog = gexif_thumbnail_new (edata->data, edata->size);
		gtk_window_set_transient_for (GTK_WINDOW (dialog),
					      GTK_WINDOW (m));
		gtk_widget_show (dialog);
	}
}

GtkWidget *
gexif_main_new (void)
{
	GExifMain *m;
	GtkWidget *browser, *vbox, *menubar, *item, *menu;
	GtkAccelGroup *accel_group, *accels;
	GtkTooltips *tooltips;
	guint key;

	m = gtk_type_new (GEXIF_TYPE_MAIN);
	gtk_window_set_title (GTK_WINDOW (m), PACKAGE);
	gtk_window_set_default_size (GTK_WINDOW (m), 640, 480);
	gtk_window_set_policy (GTK_WINDOW (m), TRUE, TRUE, TRUE);
	gtk_signal_connect (GTK_OBJECT (m), "delete_event",
			    GTK_SIGNAL_FUNC (gtk_object_destroy), NULL);

	vbox = gtk_vbox_new (FALSE, 1);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (m), vbox);

	menubar = gtk_menu_bar_new ();
	gtk_widget_show (menubar);
	gtk_box_pack_start (GTK_BOX (vbox), menubar, FALSE, FALSE, 0);

	accel_group = gtk_accel_group_new ();
	tooltips = gtk_tooltips_new ();

	/*
	 * File menu
	 */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("_File"));
	gtk_widget_add_accelerator (item, "activate_item", accel_group, key,
				    GDK_MOD1_MASK, 0);
	gtk_container_add (GTK_CONTAINER (menubar), item);

	menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), menu);
	accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (menu));

	/* Open */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("_Open..."));
	gtk_widget_add_accelerator (item, "activate_item", accels, key, 0, 0);
	gtk_container_add (GTK_CONTAINER (menu), item);
	gtk_widget_add_accelerator (item, "activate", accels, GDK_o,
				    GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_open_activate), m);

	/* Save */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("_Save"));
	gtk_widget_add_accelerator (item, "activate_item", accels, key, 0, 0);
	gtk_widget_add_accelerator (item, "activate", accels, GDK_s,
				    GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_save_activate), m);
	gtk_container_add (GTK_CONTAINER (menu), item);

	/* Save as */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("Save _As..."));
	gtk_widget_add_accelerator (item, "activate_item", accels, key, 0, 0);
	gtk_container_add (GTK_CONTAINER (menu), item);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_save_as_activate), m);

	item = gtk_menu_item_new ();
	gtk_widget_show (item);
	gtk_container_add (GTK_CONTAINER (menu), item);
	gtk_widget_set_sensitive (item, FALSE);

	/* Exit */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("E_xit"));
	gtk_widget_add_accelerator (item, "activate_item", accels, key, 0, 0);
	gtk_widget_add_accelerator (item, "activate", accels, GDK_q,
				    GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_exit_activate), m);
	gtk_container_add (GTK_CONTAINER (menu), item);

	/*
	 * View menu
	 */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("_View"));
	gtk_widget_add_accelerator (item, "activate_item", accel_group, key,
				    GDK_MOD1_MASK, 0);
	gtk_container_add (GTK_CONTAINER (menubar), item);

	menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), menu);
	accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (menu));

	/* Thumbnail */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("Thumbnail"));
	gtk_widget_add_accelerator (item, "activate_item", accels, key, 0, 0);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_thumbnail_activate), m);
	gtk_container_add (GTK_CONTAINER (menu), item);

	/*
	 * Help menu
	 */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("_Help"));
	gtk_widget_add_accelerator (item, "activate_item", accel_group, key,
				    GDK_MOD1_MASK, 0);
	gtk_container_add (GTK_CONTAINER (menubar), item);

	menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), menu);
	accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (menu));

	/* About */
	item = gtk_menu_item_new_with_label ("");
	gtk_widget_show (item);
	key = gtk_label_parse_uline (GTK_LABEL (GTK_BIN (item)->child),
				     _("_About"));
	gtk_widget_add_accelerator (item, "activate_item", accels, key, 0, 0);
	gtk_signal_connect (GTK_OBJECT (item), "activate",
			    GTK_SIGNAL_FUNC (on_about_activate), m);
	gtk_container_add (GTK_CONTAINER (menu), item); 

	browser = gtk_exif_browser_new ();
	gtk_widget_show (browser);
	gtk_box_pack_start (GTK_BOX (vbox), browser, TRUE, TRUE, 0);
	m->priv->browser = GTK_EXIF_BROWSER (browser);

	return (GTK_WIDGET (m));
}
