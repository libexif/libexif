/* gtk-position.h
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

#ifndef __GTK_POSITION_H__
#define __GTK_POSITION_H__

#include <gtk-options.h>

#define GTK_TYPE_POSITION     (gtk_position_get_type())
#define GTK_POSITION(o)       (GTK_CHECK_CAST((o),GTK_TYPE_POSITION,GtkPosition))
#define GTK_POSITION_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GTK_TYPE_POSITION,GtkPositionClass))
#define GTK_IS_POSITION(o)    (GTK_CHECK_TYPE((o),GTK_TYPE_POSITION))

typedef struct _GtkPosition        GtkPosition;
typedef struct _GtkPositionPrivate GtkPositionPrivate;
typedef struct _GtkPositionClass   GtkPositionClass;

struct _GtkPosition
{
	GtkOptions parent;

	GtkPositionPrivate *priv;
};

struct _GtkPositionClass
{
	GtkOptionsClass parent_class;

	/* Signals */
	void (* type_selected) (GtkPosition *position, GtkPositionType type);
};

GtkType    gtk_position_get_type (void);
GtkWidget *gtk_position_new      (void);

#endif /* __GTK_POSITION_H__ */
