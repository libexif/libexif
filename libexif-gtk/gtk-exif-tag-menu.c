/* gtk-exif_tag_menu.c
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
#include "gtk-exif-tag-menu.h"

#include <gtk/gtksignal.h>

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

struct _GtkExifTagMenuPrivate {
};

#define PARENT_TYPE GTK_TYPE_OPTIONS
static GtkOptionsClass *parent_class;

enum {
	TAG_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_exif_tag_menu_destroy (GtkObject *object)
{
	GtkExifTagMenu *menu = GTK_EXIF_TAG_MENU (object);

	menu = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_exif_tag_menu_finalize (GtkObject *object)
{
	GtkExifTagMenu *menu = GTK_EXIF_TAG_MENU (object);

	g_free (menu->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_exif_tag_menu_class_init (GtkExifTagMenuClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_exif_tag_menu_destroy;
	object_class->finalize = gtk_exif_tag_menu_finalize;

	signals[TAG_SELECTED] = gtk_signal_new ("tag_selected",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkExifTagMenuClass, tag_selected),
		gtk_marshal_NONE__UINT, GTK_TYPE_NONE, 1, GTK_TYPE_UINT);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_exif_tag_menu_init (GtkExifTagMenu *menu)
{
	menu->priv = g_new0 (GtkExifTagMenuPrivate, 1);
}

GtkType
gtk_exif_tag_menu_get_type (void)
{
	static GtkType menu_type = 0;

	if (!menu_type) {
		static const GtkTypeInfo menu_info = {
			"GtkExifTagMenu",
			sizeof (GtkExifTagMenu),
			sizeof (GtkExifTagMenuClass),
			(GtkClassInitFunc)  gtk_exif_tag_menu_class_init,
			(GtkObjectInitFunc) gtk_exif_tag_menu_init,
			NULL, NULL, NULL};
		menu_type = gtk_type_unique (PARENT_TYPE, &menu_info);
	}

	return (menu_type);
}

static void
on_option_selected (GtkOptions *options, guint option, GtkExifTagMenu *menu)
{
	gtk_signal_emit (GTK_OBJECT (menu), signals[TAG_SELECTED],
			 option);
}

#define LIST_SIZE 1024

GtkWidget *
gtk_exif_tag_menu_new (void)
{
	GtkExifTagMenu *menu;
	GtkOptionsList tags[LIST_SIZE];
	guint i, t;
	const gchar *name;

	menu = gtk_type_new (GTK_EXIF_TYPE_TAG_MENU);

	t = i = 0;
	memset (tags, 0, sizeof (GtkOptionsList) * LIST_SIZE);
	while ((t < 0xffff) && (i < LIST_SIZE - 1)) {
		name = exif_tag_get_name (t);
		if (name) {
			tags[i].option = t;
			tags[i].name = name;
			i++;
		}
		t++;
	}
	gtk_options_construct (GTK_OPTIONS (menu), tags);
	gtk_signal_connect (GTK_OBJECT (menu), "option_selected",
			    GTK_SIGNAL_FUNC (on_option_selected), menu);

	return (GTK_WIDGET (menu));
}
