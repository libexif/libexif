/* exif-fuji-tag.h
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

#ifndef __EXIF_FUJI_TAG_H__
#define __EXIF_FUJI_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum _ExifFujiTag ExifFujiTag;
enum _ExifFujiTag {
	EXIF_FUJI_TAG_VERSION			= 0x0000,
	EXIF_FUJI_TAG_QUALITY			= 0x1000,
	EXIF_FUJI_TAG_SHARPNESS			= 0x1001,
	EXIF_FUJI_TAG_WHITE_BALANCE		= 0x1002,
	EXIF_FUJI_TAG_COLOR			= 0x1003,
	EXIF_FUJI_TAG_TONE			= 0x1004,
	EXIF_FUJI_TAG_FLASH_MODE		= 0x1010,
	EXIF_FUJI_TAG_FLASH_STRENGTH		= 0x1011,
	EXIF_FUJI_TAG_MACRO			= 0x1020,
	EXIF_FUJI_TAG_FOCUS_MODE		= 0x1021,
	EXIF_FUJI_TAG_SLOW_SYNC			= 0x1030,
	EXIF_FUJI_TAG_PICTURE_MODE		= 0x1031,
	EXIF_FUJI_TAG_UNKNOWN_1			= 0x1032,
	EXIF_FUJI_TAG_CONT_TAKING		= 0x1100,
	EXIF_FUJI_TAG_UNKNOWN_2			= 0x1200,
	EXIF_FUJI_TAG_BLUR_WARNING		= 0x1300,
	EXIF_FUJI_TAG_FOCUS_WARNING		= 0x1301,
	EXIF_FUJI_TAG_AUTO_EXPOSURE_WARNING	= 0x1302
};

const char *exif_fuji_tag_get_name (ExifFujiTag tag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_FUJI_TAG_H__ */
