/* exif-note.h
 *
 * Copyright (C) 2002 Lutz Müller <lutz@users.sourceforge.net>
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

#ifndef __EXIF_NOTE_H__
#define __EXIF_NOTE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libexif/exif-byte-order.h>

typedef struct _ExifNote        ExifNote;
typedef struct _ExifNotePrivate ExifNotePrivate;

typedef struct _ExifNoteMethods ExifNoteMethods;
struct _ExifNoteMethods {
	void    (* free)      (ExifNote *);

	void    (* load_data) (ExifNote *, const unsigned char *, unsigned int);
	void    (* save_data) (ExifNote *, unsigned char **, unsigned int *);

	void          (* set_order) (ExifNote *, ExifByteOrder);
	ExifByteOrder (* get_order) (ExifNote *);

	char ** (*get_value)  (ExifNote *);
};

struct _ExifNote {
	ExifNoteMethods methods;
	ExifNotePrivate *priv;
};

void exif_note_construct (ExifNote *note);

void exif_note_ref   (ExifNote *note);
void exif_note_unref (ExifNote *note);
void exif_note_free  (ExifNote *note);

ExifNote *exif_note_new_from_data (const unsigned char *data,
				   unsigned int size);

char **exif_note_get_value (ExifNote *note);

void          exif_note_set_byte_order (ExifNote *note, ExifByteOrder order);
ExifByteOrder exif_note_get_byte_order (ExifNote *note);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_NOTE_H__ */
