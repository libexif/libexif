/* gtk-options.c
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
#include "gtk-options.h"

#include <gtk/gtksignal.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>

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

struct _GtkOptionsPrivate
{
	guint current;

	GArray *array;
};

#define PARENT_TYPE GTK_TYPE_OPTION_MENU
static GtkOptionMenuClass *parent_class;

enum {
	OPTION_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_options_destroy (GtkObject *object)
{
	GtkOptions *options = GTK_OPTIONS (object);

	if (options->priv->array) {
		g_array_free (options->priv->array, TRUE);
		options->priv->array = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_options_finalize (GtkObject *object)
{
	GtkOptions *options = GTK_OPTIONS (object);

	g_free (options->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_options_class_init (GtkOptionsClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_options_destroy;
	object_class->finalize = gtk_options_finalize;

	signals[OPTION_SELECTED] = gtk_signal_new ("option_selected",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkOptionsClass, option_selected),
		gtk_marshal_NONE__UINT, GTK_TYPE_NONE, 1, GTK_TYPE_UINT);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_options_init (GtkOptions *options)
{
	options->priv = g_new0 (GtkOptionsPrivate, 1);
	options->priv->array = g_array_new (FALSE, TRUE, sizeof (guint));
}

GtkType
gtk_options_get_type (void)
{
	static GtkType options_type = 0;

	if (!options_type) {
		static const GtkTypeInfo options_info = {
			"GtkOptions",
			sizeof (GtkOptions),
			sizeof (GtkOptionsClass),
			(GtkClassInitFunc)  gtk_options_class_init,
			(GtkObjectInitFunc) gtk_options_init,
			NULL, NULL, NULL};
		options_type = gtk_type_unique (PARENT_TYPE, &options_info);
	}

	return (options_type);
}

static void
on_item_activate (GtkMenuItem *item, GtkOptions *options)
{
	options->priv->current = GPOINTER_TO_INT (
		gtk_object_get_data (GTK_OBJECT (item), "option"));
	gtk_signal_emit (GTK_OBJECT (options), signals[OPTION_SELECTED],
			 options->priv->current);
}

GtkWidget *
gtk_options_new (GtkOptionsList *list)
{
	GtkOptions *options;

	g_return_val_if_fail (list != NULL, NULL);

	options = gtk_type_new (GTK_TYPE_OPTIONS);
	gtk_options_construct (options, list);

	return (GTK_WIDGET (options));
}

void
gtk_options_construct (GtkOptions *options, GtkOptionsList *list)
{
	GtkWidget *menu, *item;
	guint i;

	g_return_if_fail (GTK_IS_OPTIONS (options));
	g_return_if_fail (list != NULL);

	menu = gtk_menu_new ();
	gtk_widget_show (menu);
	for (i = 0; list[i].name; i++) {
		item = gtk_menu_item_new_with_label (list[i].name);
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU (menu), item);
		gtk_object_set_data (GTK_OBJECT (item), "option",
				GINT_TO_POINTER ((gint) list[i].option));
		gtk_signal_connect (GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (on_item_activate), options);
		g_array_append_val (options->priv->array, list[i].option);
	}
	gtk_option_menu_set_menu (GTK_OPTION_MENU (options), menu);
}

void
gtk_options_set (GtkOptions *options, guint option)
{
	guint i;

	g_return_if_fail (GTK_IS_OPTIONS (options));

	for (i = 0; i < options->priv->array->len; i++)
		if (g_array_index (options->priv->array, guint, i) == option)
			break;
	if (i == options->priv->array->len)
		return;

	gtk_option_menu_set_history (GTK_OPTION_MENU (options), i);
}

guint
gtk_options_get (GtkOptions *options)
{
	g_return_val_if_fail (GTK_IS_OPTIONS (options), 0);

	return (options->priv->current);
}
