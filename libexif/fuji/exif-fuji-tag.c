/* exif-fuji-tag.c
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

#include "config.h"
#include "exif-fuji-tag.h"
#include "i18n.h"

#include <stdlib.h>

static struct {
	ExifFujiTag tag;
	const char *name;
} table[] = {
	{EXIF_FUJI_TAG_VERSION, N_("Version")},
	{EXIF_FUJI_TAG_QUALITY, N_("Quality")},
	{EXIF_FUJI_TAG_SHARPNESS, N_("Sharpness")},
	{EXIF_FUJI_TAG_WHITE_BALANCE, N_("White balance")},
	{EXIF_FUJI_TAG_COLOR, N_("Chromaticity saturation")},
	{EXIF_FUJI_TAG_TONE, N_("Contrast")},
	{EXIF_FUJI_TAG_FLASH_MODE, N_("Flash mode")},
	{EXIF_FUJI_TAG_FLASH_STRENGTH,
	 N_("Flash firing strength compensation")},
	{EXIF_FUJI_TAG_MACRO, N_("Macro mode")},
	{EXIF_FUJI_TAG_FOCUS_MODE, N_("Focusing mode")},
	{EXIF_FUJI_TAG_SLOW_SYNC, N_("Slow synchro mode")},
	{EXIF_FUJI_TAG_PICTURE_MODE, N_("Picture mode")},
	{EXIF_FUJI_TAG_UNKNOWN_1, N_("Unknown 1")},
	{EXIF_FUJI_TAG_CONT_TAKING, N_("Continuous taking")},
	{EXIF_FUJI_TAG_UNKNOWN_2, N_("Unknown 2")},
	{EXIF_FUJI_TAG_BLUR_WARNING, N_("Blur warning")},
	{EXIF_FUJI_TAG_FOCUS_WARNING, N_("Auto focus warning")},
	{EXIF_FUJI_TAG_AUTO_EXPOSURE_WARNING, N_("Auto exposure warning")},
	{0, NULL}
};

const char *
exif_fuji_tag_get_name (ExifFujiTag tag)
{
	unsigned int i;

	for (i = 0; table[i].name; i++)
		if (table[i].tag == tag)
			break;

	return (_(table[i].name));
}
