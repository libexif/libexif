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

	/* Nikon */
	MNOTE_NIKON_TAG_FIRMWARE                = 0x0001,
        MNOTE_NIKON_TAG_ISO                     = 0x0002,
        MNOTE_NIKON_TAG_COLORMODE1              = 0x0003,
        MNOTE_NIKON_TAG_QUALITY                 = 0x0004,
        MNOTE_NIKON_TAG_WHITEBALANCE            = 0x0005,
        MNOTE_NIKON_TAG_SHARPENING              = 0x0006,
        MNOTE_NIKON_TAG_FOCUSMODE               = 0x0007,
        MNOTE_NIKON_TAG_FLASHSETTING            = 0x0008,
        MNOTE_NIKON_TAG_FLASHMODE               = 0x0009,
        MNOTE_NIKON_TAG_WHITEBALANCEFINE        = 0x000b,
        MNOTE_NIKON_TAG_WHITEBALANCERB          = 0x000c,
        MNOTE_NIKON_TAG_UNKNOWN_0X000D          = 0x000d,
        MNOTE_NIKON_TAG_EXPOSUREDIFF            = 0x000e,
        MNOTE_NIKON_TAG_ISOSELECTION            = 0x000f,
        MNOTE_NIKON_TAG_UNKNOWN_0X0011          = 0x0011,
        MNOTE_NIKON_TAG_FLASHCOMPENSATION       = 0x0012,
        MNOTE_NIKON_TAG_ISO2                    = 0x0013,
        MNOTE_NIKON_TAG_UNKNOWN_0X0016          = 0x0016,
        MNOTE_NIKON_TAG_UNKNOWN_0X0017          = 0x0017,
        MNOTE_NIKON_TAG_UNKNOWN_0X0018          = 0x0018,
        MNOTE_NIKON_TAG_UNKNOWN_0X0019          = 0x0019,
        MNOTE_NIKON_TAG_TONECOMPENSATION        = 0x0081,
        MNOTE_NIKON_TAG_LENSTYPE                = 0x0083,
        MNOTE_NIKON_TAG_LENS                    = 0x0084,
        MNOTE_NIKON_TAG_MANUALFOCUSDISTANCE     = 0x0085,
        MNOTE_NIKON_TAG_FLASHUSED               = 0x0087,
        MNOTE_NIKON_TAG_AFFOCUSPOSITION         = 0x0088,
        MNOTE_NIKON_TAG_BRACKETING              = 0x0089,
        MNOTE_NIKON_TAG_UNKNOWN_0X008A          = 0x008a,
        MNOTE_NIKON_TAG_UNKNOWN_0X008B          = 0x008b,
        MNOTE_NIKON_TAG_CURVE                   = 0x008c,
        MNOTE_NIKON_TAG_COLORMODE               = 0x008d,
        MNOTE_NIKON_TAG_LIGHTYPE                = 0x0090,
        MNOTE_NIKON_TAG_UNKNOWN_0X0091          = 0x0091,
        MNOTE_NIKON_TAG_HUE                     = 0x0092,
        MNOTE_NIKON_TAG_NOISEREDUCTION          = 0x0095,
        MNOTE_NIKON_TAG_UNKNOWN_0X0097          = 0x0097,
        MNOTE_NIKON_TAG_UNKNOWN_0X0098          = 0x0098,
        MNOTE_NIKON_TAG_UNKNOWN_0X009A          = 0x009a,
        MNOTE_NIKON_TAG_UNKNOWN_0X00A0          = 0x00a0,
        MNOTE_NIKON_TAG_UNKNOWN_0X00A2          = 0x00a2,
        MNOTE_NIKON_TAG_UNKNOWN_0X00A3          = 0x00a3,
        MNOTE_NIKON_TAG_TOTALPICTURES           = 0x00a7,
        MNOTE_NIKON_TAG_UNKNOWN_0X00A8          = 0x00a8,
        MNOTE_NIKON_TAG_OPTIMIZATION            = 0x00a9,
        MNOTE_NIKON_TAG_UNKNOWN_0X00AA          = 0x00aa,
        MNOTE_NIKON_TAG_UNKNOWN_0X00AB          = 0x00ab,
        MNOTE_NIKON_TAG_CAPTUREEDITORDATA       = 0x0e01,

	/* Olympus */
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
