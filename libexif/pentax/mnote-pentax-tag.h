/* mnote-pentax-tag.h
 *
 * Copyright © 2002, 2003 Lutz Mueller <lutz@users.sourceforge.net>
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

#ifndef __MNOTE_PENTAX_TAG_H__
#define __MNOTE_PENTAX_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Missing features which are probably in the unknowns somewhere ...
 * 1/ AF Area (Wide, Spot, Free)
 * 2/ AE Metering (Multi segment, Centre-weighted, Spot)
 * 3/ 
 */

enum _MnotePentaxTag {
	MNOTE_PENTAX_TAG_MODE		= 0x0001,
	MNOTE_PENTAX_TAG_QUALITY	= 0x0002,
	MNOTE_PENTAX_TAG_FOCUS		= 0x0003,
	MNOTE_PENTAX_TAG_FLASH		= 0x0004,
	MNOTE_PENTAX_TAG_UNKNOWN_05	= 0x0005,
	MNOTE_PENTAX_TAG_UNKNOWN_06	= 0x0006,
	MNOTE_PENTAX_TAG_WHITE_BALANCE	= 0x0007,
	MNOTE_PENTAX_TAG_UNKNOWN_08	= 0x0008,
	MNOTE_PENTAX_TAG_UNKNOWN_09	= 0x0009,
	MNOTE_PENTAX_TAG_ZOOM		= 0x000a,
	MNOTE_PENTAX_TAG_SHARPNESS	= 0x000b,
	MNOTE_PENTAX_TAG_CONTRAST	= 0x000c,
	MNOTE_PENTAX_TAG_SATURATION	= 0x000d,
	MNOTE_PENTAX_TAG_UNKNOWN_14	= 0x000e,
	MNOTE_PENTAX_TAG_UNKNOWN_15	= 0x000f,
	MNOTE_PENTAX_TAG_UNKNOWN_16	= 0x0010,
	MNOTE_PENTAX_TAG_UNKNOWN_17	= 0x0011,
	MNOTE_PENTAX_TAG_UNKNOWN_18	= 0x0012,
	MNOTE_PENTAX_TAG_UNKNOWN_19	= 0x0013,
	MNOTE_PENTAX_TAG_ISO_SPEED	= 0x0014,
	MNOTE_PENTAX_TAG_UNKNOWN_21	= 0x0015,
	MNOTE_PENTAX_TAG_COLOR		= 0x0017,
	MNOTE_PENTAX_TAG_UNKNOWN_24	= 0x0018,
	MNOTE_PENTAX_TAG_UNKNOWN_25	= 0x0019,
	MNOTE_PENTAX_TAG_PRINTIM	= 0x0e00,
	MNOTE_PENTAX_TAG_TZ_CITY	= 0x1000,
	MNOTE_PENTAX_TAG_TZ_DST		= 0x1001,
};
typedef enum _MnotePentaxTag MnotePentaxTag;

const char *mnote_pentax_tag_get_name        (MnotePentaxTag tag);
const char *mnote_pentax_tag_get_title       (MnotePentaxTag tag);
const char *mnote_pentax_tag_get_description (MnotePentaxTag tag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MNOTE_PENTAX_TAG_H__ */
