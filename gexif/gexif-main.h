/* gexif-main.h
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

#ifndef __GEXIF_MAIN_H__
#define __GEXIF_MAIN_H__

#include <gtk/gtkwindow.h>

#define GEXIF_TYPE_MAIN     (gexif_main_get_type())
#define GEXIF_MAIN(o)       (GTK_CHECK_CAST((o),GEXIF_TYPE_MAIN,GExifMain))
#define GEXIF_MAIN_CLASS(k) (GTK_CHECK_CLASS_CAST((k),GEXIF_TYPE_MAIN_CLASS,GExifMainClass))
#define GEXIF_IS_MAIN(o)    (GTK_CHECK_TYPE((o),GEXIF_TYPE_MAIN))

typedef struct _GExifMain        GExifMain;
typedef struct _GExifMainPrivate GExifMainPrivate;
typedef struct _GExifMainClass   GExifMainClass;

struct _GExifMain
{
	GtkWindow parent;

	GExifMainPrivate *priv;
};

struct _GExifMainClass
{
	GtkWindowClass parent_class;
};

GtkType    gexif_main_get_type (void);
GtkWidget *gexif_main_new (void);

#endif /* __GEXIF_MAIN_H__ */
