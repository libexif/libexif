/* gtk-options.h
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

#ifndef __GTK_OPTIONS_H__
#define __GTK_OPTIONS_H__

#include <gtk/gtkoptionmenu.h>

#define GTK_TYPE_OPTIONS     (gtk_options_get_type())
#define GTK_OPTIONS(o)       (GTK_CHECK_CAST((o),GTK_TYPE_OPTIONS,GtkOptions))
#define GTK_OPTIONS_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_TYPE_OPTIONS,GtkOptionsClass))
#define GTK_IS_OPTIONS(o)    (GTK_CHECK_TYPE((o),GTK_TYPE_OPTIONS))

typedef struct _GtkOptions        GtkOptions;
typedef struct _GtkOptionsPrivate GtkOptionsPrivate;
typedef struct _GtkOptionsClass   GtkOptionsClass;

struct _GtkOptions
{
	GtkOptionMenu parent;

	GtkOptionsPrivate *priv;
};

struct _GtkOptionsClass
{
	GtkOptionMenuClass parent_class;

	/* Signals */
	void (* option_selected) (GtkOptions *options, guint option);
};

typedef struct _GtkOptionsList GtkOptionsList;
struct _GtkOptionsList {
	guint option;
	const gchar *name;
};

GtkType    gtk_options_get_type  (void);
GtkWidget *gtk_options_new       (GtkOptionsList *list);
void       gtk_options_construct (GtkOptions *options, GtkOptionsList *list);

void       gtk_options_set       (GtkOptions *options, guint option);
guint      gtk_options_get       (GtkOptions *options);

void       gtk_options_set_sensitive_all (GtkOptions *options,
					  gboolean sensitive);
void       gtk_options_set_sensitive     (GtkOptions *options, guint option,
				          gboolean sensitive);

#endif /* __GTK_OPTIONS_H__ */
