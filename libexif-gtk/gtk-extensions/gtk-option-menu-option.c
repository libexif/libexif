/* gtk-option-menu-option.c
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
#include "gtk-option-menu-option.h"

#include <gtk/gtksignal.h>

#include "gtk-menu-option.h"

struct _GtkOptionMenuOptionPrivate {
};

#define PARENT_TYPE GTK_TYPE_OPTION_MENU
static GtkOptionMenuClass *parent_class;

enum {
	OPTION_SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};

static void
gtk_option_menu_option_destroy (GtkObject *object)
{
	GtkOptionMenuOption *menu = GTK_OPTION_MENU_OPTION (object);

	menu = NULL;

	GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
gtk_option_menu_option_finalize (GtkObject *object)
{
	GtkOptionMenuOption *menu = GTK_OPTION_MENU_OPTION (object);

	g_free (menu->priv);

	GTK_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_option_menu_option_class_init (GtkOptionMenuOptionClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy  = gtk_option_menu_option_destroy;
	object_class->finalize = gtk_option_menu_option_finalize;

	signals[OPTION_SELECTED] = gtk_signal_new ("option_selected",
		GTK_RUN_FIRST, object_class->type,
		GTK_SIGNAL_OFFSET (GtkOptionMenuOptionClass, option_selected),
		gtk_marshal_NONE__UINT, GTK_TYPE_NONE, 1, GTK_TYPE_UINT);
	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);

	parent_class = gtk_type_class (PARENT_TYPE);
}

static void
gtk_option_menu_option_init (GtkOptionMenuOption *menu)
{
	menu->priv = g_new0 (GtkOptionMenuOptionPrivate, 1);
}

GtkType
gtk_option_menu_option_get_type (void)
{
	static GtkType option_menu_type = 0;

	if (!option_menu_type) {
		static const GtkTypeInfo option_menu_info = {
			"GtkOptionMenuOption",
			sizeof (GtkOptionMenuOption),
			sizeof (GtkOptionMenuOptionClass),
			(GtkClassInitFunc)  gtk_option_menu_option_class_init,
			(GtkObjectInitFunc) gtk_option_menu_option_init,
			NULL, NULL, NULL};
			option_menu_type = gtk_type_unique (PARENT_TYPE,
							&option_menu_info);
	}

	return (option_menu_type);
}

static void
on_option_set (GtkMenuOption *m, guint option, GtkOptionMenuOption *menu)
{
	guint index;

	index = gtk_menu_option_get_index (m, option);
	gtk_option_menu_set_history (GTK_OPTION_MENU (menu), index);
}

static void
on_option_selected (GtkMenuOption *m, guint option, GtkOptionMenuOption *menu)
{
	gtk_signal_emit (GTK_OBJECT (menu), signals[OPTION_SELECTED],
			 option);
}

GtkWidget *
gtk_option_menu_option_new (GtkOptions *options)
{
	GtkOptionMenuOption *menu;
	GtkWidget *m;

	g_return_val_if_fail (options != NULL, NULL);

	menu = gtk_type_new (GTK_TYPE_OPTION_MENU_OPTION);

	m = gtk_menu_option_new (options);
	gtk_widget_show (m);
	gtk_option_menu_set_menu (GTK_OPTION_MENU (menu), m);
	gtk_signal_connect (GTK_OBJECT (m), "option_set",
			    GTK_SIGNAL_FUNC (on_option_set), menu);
	gtk_signal_connect (GTK_OBJECT (m), "option_selected",
			    GTK_SIGNAL_FUNC (on_option_selected), menu);

	return (GTK_WIDGET (menu));
}

guint
gtk_option_menu_option_get (GtkOptionMenuOption *menu)
{
	GtkWidget *m;

	g_return_val_if_fail (GTK_IS_OPTION_MENU_OPTION (menu), 0);

	m = gtk_option_menu_get_menu (GTK_OPTION_MENU (menu));
	return (gtk_menu_option_get (GTK_MENU_OPTION (m)));
}

void
gtk_option_menu_option_set (GtkOptionMenuOption *menu, guint option)
{
	GtkWidget *m;

	g_return_if_fail (GTK_IS_OPTION_MENU_OPTION (menu));

	m = gtk_option_menu_get_menu (GTK_OPTION_MENU (menu));
	gtk_menu_option_set (GTK_MENU_OPTION (m), option);
}
