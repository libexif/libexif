/* exif-content.h
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

#ifndef __EXIF_CONTENT_H__
#define __EXIF_CONTENT_H__

typedef struct _ExifContent        ExifContent;
typedef struct _ExifContentPrivate ExifContentPrivate;

#include <libexif/exif-tag.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-data.h>

struct _ExifContent
{
        ExifEntry **entries;
        unsigned int count;

	/* Data containing this content */
	ExifData *parent;

	ExifContentPrivate *priv;
};

/* Lifecycle */
ExifContent *exif_content_new   (void);
void         exif_content_ref   (ExifContent *content);
void         exif_content_unref (ExifContent *content);
void         exif_content_free  (ExifContent *content);

void         exif_content_add_entry     (ExifContent *content, ExifEntry *e);
void         exif_content_remove_entry  (ExifContent *content, ExifEntry *e);
ExifEntry   *exif_content_get_entry     (ExifContent *content, ExifTag tag);

typedef void (* ExifContentForeachEntryFunc) (ExifEntry *, void *user_data);
void         exif_content_foreach_entry (ExifContent *content,
					 ExifContentForeachEntryFunc func,
					 void *user_data);

/* For your convenience */
#define exif_content_get_value(c,t)					\
	(exif_content_get_entry (c,t) ?					\
	 exif_entry_get_value (exif_content_get_entry (c,t)) : NULL)
#define exif_content_get_value_brief(c,t)				\
	(exif_content_get_entry (c,t) ?					\
	 exif_entry_get_value_brief (exif_content_get_entry (c,t)) : NULL)

void         exif_content_dump  (ExifContent *content, unsigned int indent);

#endif /* __EXIF_CONTENT_H__ */
