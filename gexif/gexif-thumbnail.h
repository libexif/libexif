/* gexif-thumbnail.h
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

#ifndef __GEXIF_THUMBNAIL_H__
#define __GEXIF_THUMBNAIL_H__

#include <gtk/gtkdialog.h>

#define GEXIF_TYPE_THUMBNAIL  (gexif_thumbnail_get_type ())
#define GEXIF_THUMBNAIL(o)    (GTK_CHECK_CAST((o),GEXIF_TYPE_THUMBNAIL,GExifThumbnail))
#define GEXIF_IS_THUMBNAIL(o) (GTK_CHECK_TYPE((o),GEXIF_TYPE_THUMBNAIL))

typedef struct _GExifThumbnail        GExifThumbnail;
typedef struct _GExifThumbnailPrivate GExifThumbnailPrivate;
typedef struct _GExifThumbnailClass   GExifThumbnailClass;

struct _GExifThumbnail
{
	GtkDialog parent;

	GExifThumbnailPrivate *priv;
};

struct _GExifThumbnailClass
{
	GtkDialogClass parent_class;
};

GtkType    gexif_thumbnail_get_type (void);
GtkWidget *gexif_thumbnail_new      (const guchar *data, guint size);

#endif /* __GEXIF_THUMBNAIL_H__ */
