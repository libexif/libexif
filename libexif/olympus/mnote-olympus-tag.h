/* mnote-olympus-tag.h
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

#ifndef __MNOTE_OLYMPUS_TAG_H__
#define __MNOTE_OLYMPUS_TAG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum _MnoteOlympusTag {
	MNOTE_OLYMPUS_TAG_MODE		= 0x0200,
	MNOTE_OLYMPUS_TAG_QUALITY	= 0x0201,
	MNOTE_OLYMPUS_TAG_MACRO		= 0x0202,
	MNOTE_OLYMPUS_TAG_UNKNOWN_1	= 0x0203,
	MNOTE_OLYMPUS_TAG_DIGIZOOM	= 0x0204,
	MNOTE_OLYMPUS_TAG_UNKNOWN_2	= 0x0205,
	MNOTE_OLYMPUS_TAG_UNKNOWN_3	= 0x0206,
	MNOTE_OLYMPUS_TAG_VERSION	= 0x0207,
	MNOTE_OLYMPUS_TAG_INFO		= 0x0208,
	MNOTE_OLYMPUS_TAG_ID		= 0x0209,
	MNOTE_OLYMPUS_TAG_UNKNOWN_4	= 0x0f04,
	MNOTE_OLYMPUS_TAG_FLASHMODE	= 0x1004,
	MNOTE_OLYMPUS_TAG_MANFOCUS	= 0x100b,
	MNOTE_OLYMPUS_TAG_FOCUSDIST	= 0x100c,
	MNOTE_OLYMPUS_TAG_SHARPNESS	= 0x100f,
	MNOTE_OLYMPUS_TAG_WBALANCE	= 0x1015,
	MNOTE_OLYMPUS_TAG_CONTRAST	= 0x1029
};
typedef enum _MnoteOlympusTag MnoteOlympusTag;

const char *mnote_olympus_tag_get_name        (MnoteOlympusTag tag);
const char *mnote_olympus_tag_get_title       (MnoteOlympusTag tag);
const char *mnote_olympus_tag_get_description (MnoteOlympusTag tag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MNOTE_OLYMPUS_TAG_H__ */
