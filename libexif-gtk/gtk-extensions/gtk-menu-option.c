/* gtk-menu-option.c
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
#include "gtk-menu-option.h"

#include <gtk/gtksignal.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkmenu.h>

#ifdef ENABLE_NLS
#  include <liboptionl.h>
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

struct _GtkMenuOptionPrivate
{
	guint current;

	GArray *array;

	GPtrArray *items;
};

#define PARENT_TYPE GTK_TYPE_MENU
static GtkMenuClass *parent_class;

enum {
	OPTION_SELECTED,
	OPTION_SET,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_menu_option_destroy (GtkObject *object)
{
	GtkMenuOption *menu = GTK_MENU_OPTION (object);

	if (menu->priv->array) {
		g_array_free (menu->priv->array, TRUE);
		menu->priv->array = NULL;
	}

	if (menu->priv->items) {
		g_ptr_array_free (menu->priv->items, TRUE);
		menu->priv->items = NULL;
	}

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_menu_option_finalize (GtkObject *object)
{
	GtkMenuOption *menu = GTK_MENU_OPTION (object);

	g_free (menu->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_menu_option_class_init (GtkMenuOptionClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_menu_option_destroy;
	object_class->finalize = gtk_menu_option_finalize;

	signals[OPTION_SELECTED] = gtk_signal_new ("option_selected",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkMenuOptionClass, option_selected),
		gtk_marshal_NONE__UINT, GTK_TYPE_NONE, 1, GTK_TYPE_UINT);
	signals[OPTION_SET] = gtk_signal_new ("option_set",
		GTK_RUN_LAST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkMenuOptionClass, option_set),
		gtk_marshal_NONE__UINT, GTK_TYPE_NONE, 1, GTK_TYPE_UINT);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_menu_option_init (GtkMenuOption *menu)
{
	menu->priv = g_new0 (GtkMenuOptionPrivate, 1);
	menu->priv->array = g_array_new (FALSE, TRUE, sizeof (guint));
	menu->priv->items = g_ptr_array_new ();
}

GtkType
gtk_menu_option_get_type (void)
{
	static GtkType menu_option_type = 0;

	if (!menu_option_type) {
		static const GtkTypeInfo menu_option_info = {
			"GtkMenuOption",
			sizeof (GtkMenuOption),
			sizeof (GtkMenuOptionClass),
			(GtkClassInitFunc)  gtk_menu_option_class_init,
			(GtkObjectInitFunc) gtk_menu_option_init,
			NULL, NULL, NULL};
		menu_option_type = gtk_type_unique (PARENT_TYPE, &menu_option_info);
	}

	return (menu_option_type);
}

static void
on_item_activate (GtkMenuItem *item, GtkMenuOption *menu)
{
	menu->priv->current = GPOINTER_TO_INT (
		gtk_object_get_data (GTK_OBJECT (item), "option"));
	gtk_signal_emit (GTK_OBJECT (menu), signals[OPTION_SELECTED],
			 menu->priv->current);
}

GtkWidget *
gtk_menu_option_new (GtkOptions *list)
{
	GtkMenuOption *menu;

	g_return_val_if_fail (list != NULL, NULL);

	menu = gtk_type_new (GTK_TYPE_MENU_OPTION);
	gtk_menu_option_construct (menu, list);

	return (GTK_WIDGET (menu));
}

void
gtk_menu_option_construct (GtkMenuOption *menu, GtkOptions *list)
{
	GtkWidget *item;
	guint i;

	g_return_if_fail (GTK_IS_MENU_OPTION (menu));
	g_return_if_fail (list != NULL);

	/* Sort the list */
	gtk_options_sort (list);

	for (i = 0; list[i].name; i++) {
		item = gtk_menu_item_new_with_label (list[i].name);
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU (menu), item);
		gtk_object_set_data (GTK_OBJECT (item), "option",
				GINT_TO_POINTER ((gint) list[i].option));
		gtk_signal_connect (GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (on_item_activate), menu);
		g_array_append_val (menu->priv->array, list[i].option);
		g_ptr_array_add (menu->priv->items, item);
	}
}

void
gtk_menu_option_set_sensitive (GtkMenuOption *menu, guint option,
			   gboolean sensitive)
{
	guint i;

	g_return_if_fail (GTK_IS_MENU_OPTION (menu));

	i = gtk_menu_option_get_index (menu, option);
	gtk_widget_set_sensitive (GTK_WIDGET (menu->priv->items->pdata[i]),
				  sensitive);
}

void
gtk_menu_option_set_sensitive_all (GtkMenuOption *menu, gboolean sensitive)
{
	guint i;

	g_return_if_fail (GTK_IS_MENU_OPTION (menu));

	for (i = 0; i < menu->priv->items->len; i++)
		gtk_widget_set_sensitive (
			GTK_WIDGET (menu->priv->items->pdata[i]), 
			sensitive);
}

guint
gtk_menu_option_get (GtkMenuOption *menu)
{
	g_return_val_if_fail (GTK_IS_MENU_OPTION (menu), 0);

	return (menu->priv->current);
}

void
gtk_menu_option_set (GtkMenuOption *menu, guint option)
{
	g_return_if_fail (GTK_IS_MENU_OPTION (menu));

	menu->priv->current = option;

	gtk_signal_emit (GTK_OBJECT (menu), signals[OPTION_SET], option);
}

guint
gtk_menu_option_get_index (GtkMenuOption *menu, guint option)
{
	guint i;

	g_return_val_if_fail (GTK_IS_MENU_OPTION (menu), 0);

	for (i = 0; i < menu->priv->array->len; i++)
		if (g_array_index (menu->priv->array, guint, i) == option)
			break;
	if (i == menu->priv->array->len) {
		g_warning ("Option %i not found!", option);
		return (0);
	}

	return (i);
}
