/* mnote-olympus-data.h
 *
 * Copyright © 2002 Lutz Müller <lutz@users.sourceforge.net>
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

#ifndef __MNOTE_OLYMPUS_CONTENT_H__
#define __MNOTE_OLYMPUS_CONTENT_H__

#include <libexif/exif-mnote-data-priv.h>
#include <libexif/olympus/mnote-olympus-entry.h>
#include <libexif/exif-byte-order.h>

typedef struct _ExifMnoteDataOlympus ExifMnoteDataOlympus;

struct _ExifMnoteDataOlympus {
	ExifMnoteData parent;

	MnoteOlympusEntry *entries;
	unsigned int count;

	ExifByteOrder order;
};

ExifMnoteData *exif_mnote_data_olympus_new (ExifByteOrder);

#endif /* __MNOTE_OLYMPUS_CONTENT_H__ */
