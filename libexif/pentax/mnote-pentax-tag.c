/* mnote-pentax-tag.c:
 *
 * Copyright (c) 2002 Lutz Mueller <lutz@users.sourceforge.net>
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

#include <config.h>
#include "mnote-pentax-tag.h"

#include <stdlib.h>

#include <libexif/i18n.h>

static const struct {
	MnotePentaxTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {
#ifndef NO_VERBOSE_TAG_STRINGS
	{MNOTE_PENTAX_TAG_MODE, "Mode", N_("Capture Mode"), ""},
	{MNOTE_PENTAX_TAG_QUALITY, "Quality", N_("Quality Level"), ""},
	{MNOTE_PENTAX_TAG_FOCUS, "Focus", N_("Focus Mode"), ""},
	{MNOTE_PENTAX_TAG_FLASH, "Flash", N_("Flash Mode"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_05, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_06, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_WHITE_BALANCE, "WhiteBalance", N_("White Balance"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_08, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_09, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_ZOOM, "Zoom", N_("Zoom"), NULL},
	{MNOTE_PENTAX_TAG_SHARPNESS, "Sharpness", N_("Sharpness"), ""},
	{MNOTE_PENTAX_TAG_CONTRAST, "Contrast", N_("Contrast"), ""},
	{MNOTE_PENTAX_TAG_SATURATION, "Saturation", N_("Saturation"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_14, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_15, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_16, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_17, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_18, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_19, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_ISO_SPEED, "ISOSpeed", N_("ISOSpeed"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_21, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_COLOR, "Color", N_("Colors"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_24, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_25, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_PRINTIM, "PrintIM", N_("PrintIM Settings"), ""},
	{MNOTE_PENTAX_TAG_TZ_CITY, "TimeZone", N_("Time Zone"), ""},
	{MNOTE_PENTAX_TAG_TZ_DST, "DaylightSavings", N_("Daylight Savings"), ""},
	{MNOTE_PENTAX2_TAG_MODE, "Mode", N_("Capture Mode"), ""},
	{MNOTE_PENTAX2_TAG_QUALITY, "Quality", N_("Quality Level"), ""},
	{MNOTE_PENTAX2_TAG_IMAGE_SIZE, "ImageSize", N_("Image Size"), ""},
	{MNOTE_PENTAX2_TAG_PICTURE_MODE, "PictureMode", N_("PictureMode"), ""},
	{MNOTE_PENTAX2_TAG_FLASH_MODE, "FlashMode", N_("Flash Mode"), ""},
	{MNOTE_PENTAX2_TAG_FOCUS_MODE, "FocusMode", N_("Focus Mode"), ""},
	{MNOTE_PENTAX2_TAG_AFPOINT_SELECTED, "AFPointSelected", N_("AF Point Selected"), ""},
	{MNOTE_PENTAX2_TAG_AUTO_AFPOINT, "AutoAFPoint", N_("Auto AF Point"), ""},
	{MNOTE_PENTAX2_TAG_WHITE_BALANCE, "WhiteBalance", N_("White Balance"), ""},
	{MNOTE_CASIO2_TAG_OBJECT_DISTANCE, "ObjectDistance", N_("Object Distance"), N_("Distance of photographed object in millimeters.")},
	{MNOTE_CASIO2_TAG_TIME_ZONE, "TimeZone", N_("Time Zone"), ""},
	{MNOTE_CASIO2_TAG_BESTSHOT_MODE, "BestshotMode", N_("Bestshot mode"), ""},
#endif
	{0, NULL, NULL, NULL}
};

const char *
mnote_pentax_tag_get_name (MnotePentaxTag t)
{
	unsigned int i;

	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (table[i].name);
	return NULL;
}

const char *
mnote_pentax_tag_get_title (MnotePentaxTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (_(table[i].title));
	return NULL;
}

const char *
mnote_pentax_tag_get_description (MnotePentaxTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) {
			if (!table[i].description || !*table[i].description)
				return "";
			return (_(table[i].description));
		}
	return NULL;
}
