/* gtk-option-menu-option.h
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

#ifndef __GTK_OPTION_MENU_OPTION_H__
#define __GTK_OPTION_MENU_OPTION_H__

#include <gtk/gtkoptionmenu.h>

#include <gtk-options.h>

#define GTK_TYPE_OPTION_MENU_OPTION  (gtk_option_menu_option_get_type())
#define GTK_OPTION_MENU_OPTION(o)    (GTK_CHECK_CAST((o),GTK_TYPE_OPTION_MENU_OPTION,GtkOptionMenuOption))
#define GTK_IS_OPTION_MENU_OPTION(o) (GTK_CHECK_TYPE((o),GTK_TYPE_OPTION_MENU_OPTION))

typedef struct _GtkOptionMenuOption        GtkOptionMenuOption;
typedef struct _GtkOptionMenuOptionPrivate GtkOptionMenuOptionPrivate;
typedef struct _GtkOptionMenuOptionClass   GtkOptionMenuOptionClass;

struct _GtkOptionMenuOption {
	GtkOptionMenu parent;

	GtkOptionMenuOptionPrivate *priv;
};

struct _GtkOptionMenuOptionClass {
	GtkOptionMenuClass parent_class;

	/* Signals */
	void (* option_selected)     (GtkOptionMenuOption *menu, guint option);
};

GtkType    gtk_option_menu_option_get_type (void);
GtkWidget *gtk_option_menu_option_new      (GtkOptions *options);

guint      gtk_option_menu_option_get (GtkOptionMenuOption *menu);
void       gtk_option_menu_option_set (GtkOptionMenuOption *menu, guint option);

#endif /* __GTK_OPTION_MENU_OPTION_H__ */
