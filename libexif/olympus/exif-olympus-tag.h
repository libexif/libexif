/* exif-olympus-tag.h
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

#ifndef __EXIF_OLYMPUS_TAG_H__
#define __EXIF_OLYMPUS_TAG_H__

typedef enum _ExifOlympusTag ExifOlympusTag;
enum _ExifOlympusTag {
	EXIF_OLYMPUS_TAG_MODE		= 0x0200,
	EXIF_OLYMPUS_TAG_QUALITY	= 0x0201,
	EXIF_OLYMPUS_TAG_MACRO		= 0x0202,
	EXIF_OLYMPUS_TAG_UNKNOWN_1	= 0x0203,
	EXIF_OLYMPUS_TAG_ZOOM		= 0x0204,
	EXIF_OLYMPUS_TAG_UNKNOWN_2	= 0x0205,
	EXIF_OLYMPUS_TAG_UNKNOWN_3	= 0x0206,
	EXIF_OLYMPUS_TAG_VERSION	= 0x0207,
	EXIF_OLYMPUS_TAG_INFO		= 0x0208,
	EXIF_OLYMPUS_TAG_ID		= 0x0209,
	EXIF_OLYMPUS_TAG_UNKNOWN_4	= 0x0f04
};

const char *exif_olympus_tag_get_name (ExifOlympusTag tag);

#endif /* __EXIF_OLYMPUS_TAG_H__ */
