/* gtk-menu-int.h
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

#ifndef __GTK_MENU_OPTION_H__
#define __GTK_MENU_OPTION_H__

#include <gtk/gtkmenu.h>

#include <gtk-options.h>

#define GTK_TYPE_MENU_OPTION     (gtk_menu_option_get_type())
#define GTK_MENU_OPTION(o)       (GTK_CHECK_CAST((o),GTK_TYPE_MENU_OPTION,GtkMenuOption))
#define GTK_MENU_OPTION_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_TYPE_MENU_OPTION,GtkMenuOptionClass))
#define GTK_IS_MENU_OPTION(o)    (GTK_CHECK_TYPE((o),GTK_TYPE_MENU_OPTION))

typedef struct _GtkMenuOption        GtkMenuOption;
typedef struct _GtkMenuOptionPrivate GtkMenuOptionPrivate;
typedef struct _GtkMenuOptionClass   GtkMenuOptionClass;

struct _GtkMenuOption
{
	GtkMenu parent;

	GtkMenuOptionPrivate *priv;
};

struct _GtkMenuOptionClass
{
	GtkMenuClass parent_class;

	/* Signals */
	void (* option_selected) (GtkMenuOption *menu, guint option);
	void (* option_set)      (GtkMenuOption *menu, guint option);
};

GtkType    gtk_menu_option_get_type  (void);
GtkWidget *gtk_menu_option_new       (GtkOptions *list);
void       gtk_menu_option_construct (GtkMenuOption *menu, GtkOptions *list);

void       gtk_menu_option_set (GtkMenuOption *menu, guint option);
guint      gtk_menu_option_get (GtkMenuOption *menu);

void       gtk_menu_option_set_sensitive_all (GtkMenuOption *menu,
					   gboolean sensitive);
void       gtk_menu_option_set_sensitive     (GtkMenuOption *menu, guint option,
				           gboolean sensitive);

guint      gtk_menu_option_get_index         (GtkMenuOption *menu, guint option);

#endif /* __GTK_MENU_OPTION_H__ */
