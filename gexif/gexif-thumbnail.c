/* gexif-thumbnail.c
 *
 * Copyright (C) 2001 Lutz Müller <urc8@rz.uni-karlsruhe.de>
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
#include "gexif-thumbnail.h"

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#include <string.h>

#include <gtk/gtkbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkpixmap.h>

#ifdef HAVE_GDK_PIXBUF
#  include <gdk-pixbuf/gdk-pixbuf.h>
#  include <gdk-pixbuf/gdk-pixbuf-loader.h>
#endif

struct _GExifThumbnailPrivate
{
};

#define PARENT_TYPE GTK_TYPE_DIALOG
static GtkDialogClass *parent_class;

static void
gexif_thumbnail_destroy (GtkObject *object)
{
	GExifThumbnail *thumbnail = GEXIF_THUMBNAIL (object);

	thumbnail = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gexif_thumbnail_finalize (GtkObject *object)
{
	GExifThumbnail *thumbnail = GEXIF_THUMBNAIL (object);

	g_free (thumbnail->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gexif_thumbnail_class_init (GExifThumbnailClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gexif_thumbnail_destroy;
	object_class->finalize = gexif_thumbnail_finalize;

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gexif_thumbnail_init (GExifThumbnail *thumbnail)
{
	thumbnail->priv = g_new0 (GExifThumbnailPrivate, 1);
}

GtkType
gexif_thumbnail_get_type (void)
{
	static GtkType thumbnail_type = 0;

	if (!thumbnail_type) {
		static const GtkTypeInfo thumbnail_info = {
			"GExifThumbnail",
			sizeof (GExifThumbnail),
			sizeof (GExifThumbnailClass),
			(GtkClassInitFunc)  gexif_thumbnail_class_init,
			(GtkObjectInitFunc) gexif_thumbnail_init,
			NULL, NULL, NULL};
		thumbnail_type = gtk_type_unique (PARENT_TYPE, &thumbnail_info);
	}

	return (thumbnail_type);
}

static void
on_close_clicked (GtkButton *button, GExifThumbnail *thumbnail)
{
	gtk_object_destroy (GTK_OBJECT (thumbnail));
}

GtkWidget *
gexif_thumbnail_new (const guchar *data, guint size)
{
	GExifThumbnail *thumbnail;
	GtkWidget *button, *image;
#ifdef HAVE_GDK_PIXBUF
	GdkPixbufLoader *loader;
	GdkPixmap *pixmap = NULL;
	GdkBitmap *bitmap = NULL;
	GdkPixbuf *pixbuf;
#endif

	thumbnail = gtk_type_new (GEXIF_TYPE_THUMBNAIL);
	gtk_signal_connect (GTK_OBJECT (thumbnail), "delete_event",
			    GTK_SIGNAL_FUNC (gtk_object_destroy), NULL);

#ifdef HAVE_GDK_PIXBUF
	loader = gdk_pixbuf_loader_new ();
	gdk_pixbuf_loader_write (loader, data, size);
	gdk_pixbuf_loader_close (loader);
	pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
	gdk_pixbuf_render_pixmap_and_mask (pixbuf, &pixmap, &bitmap, 127);
	gtk_object_unref (GTK_OBJECT (loader));
	image = gtk_pixmap_new (pixmap, bitmap);
	if (pixmap)
		gdk_pixmap_unref (pixmap);
	if (bitmap)
		gdk_bitmap_unref (bitmap);
#else
	image = gtk_label_new ("Compiled without gdk-pixbuf support!");
#endif
	gtk_widget_show (image);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (thumbnail)->vbox),
			    image, FALSE, FALSE, 0);

	button = gtk_button_new_with_label (_("Close"));
	gtk_widget_show (button);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			    GTK_SIGNAL_FUNC (on_close_clicked), thumbnail);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (thumbnail)->action_area),
			   button);
	gtk_widget_grab_focus (button);

	return (GTK_WIDGET (thumbnail));
}
