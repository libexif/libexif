/* exif-canon-tag.h
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

#ifndef __EXIF_CANON_TAG_H__
#define __EXIF_CANON_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum _ExifCanonTag ExifCanonTag;
enum _ExifCanonTag {
	EXIF_CANON_TAG_UNKNOWN_0	= 0x0,
	EXIF_CANON_TAG_SETTINGS_1	= 0x1,
	EXIF_CANON_TAG_UNKNOWN_3	= 0x3,
	EXIF_CANON_TAG_SETTINGS_2	= 0x4,
	EXIF_CANON_TAG_IMAGE_TYPE	= 0x6,
	EXIF_CANON_TAG_FIRMWARE		= 0x7,
	EXIF_CANON_TAG_IMAGE_NUMBER	= 0x8,
	EXIF_CANON_TAG_OWNER		= 0x9,
	EXIF_CANON_TAG_UNKNOWN_10	= 0xa,
	EXIF_CANON_TAG_SERIAL_NUMBER	= 0xc,
	EXIF_CANON_TAG_UNKNOWN_13	= 0xd,
	EXIF_CANON_TAG_CUSTOM_FUNCS	= 0xf
};

const char *exif_canon_tag_get_name (ExifCanonTag tag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_CANON_TAG_H__ */
