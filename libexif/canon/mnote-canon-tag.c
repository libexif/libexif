/* mnote-canon-tag.c
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
#include "mnote-canon-tag.h"

#include <stdlib.h>

#include <libexif/i18n.h>

static const struct {
	MnoteCanonTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {
#ifndef NO_VERBOSE_TAG_STRINGS
	{MNOTE_CANON_TAG_SETTINGS_1, "Settings1", N_("Settings (first part)"), ""},
	{MNOTE_CANON_TAG_FOCAL_LENGTH, "FocalLength", N_("Focal length"), ""},
	{MNOTE_CANON_TAG_SETTINGS_2, "Settings2", N_("Settings (second part)"), ""},
	{MNOTE_CANON_TAG_PANORAMA, "Panorama", N_("Panorama"), ""},
	{MNOTE_CANON_TAG_IMAGE_TYPE, "ImageType", N_("Image type"), ""},
	{MNOTE_CANON_TAG_FIRMWARE, "FirmwareVersion", N_("Firmware version"), ""},
	{MNOTE_CANON_TAG_IMAGE_NUMBER, "ImageNumber", N_("Image number"), ""},
	{MNOTE_CANON_TAG_OWNER, "OwnerName", N_("Owner name"), ""},
	{MNOTE_CANON_TAG_COLOR_INFORMATION, "ColorInformation", N_("Color information"), ""},
	{MNOTE_CANON_TAG_SERIAL_NUMBER, "SerialNumber", N_("Serial number"), ""},
	{MNOTE_CANON_TAG_CUSTOM_FUNCS, "CustomFunctions", N_("Custom functions"), ""},
#endif
	{0, NULL, NULL, NULL}
};

static const struct {
	MnoteCanonTag tag;
	unsigned int subtag;
	const char *name;
} table_sub[] = {
#ifndef NO_VERBOSE_TAG_STRINGS
	{MNOTE_CANON_TAG_SETTINGS_1,  0, N_("Macro mode")},
	{MNOTE_CANON_TAG_SETTINGS_1,  1, N_("Self-timer")},
	{MNOTE_CANON_TAG_SETTINGS_1,  2, N_("Quality")},
	{MNOTE_CANON_TAG_SETTINGS_1,  3, N_("Flash mode")},
	{MNOTE_CANON_TAG_SETTINGS_1,  4, N_("Drive mode")},
	{MNOTE_CANON_TAG_SETTINGS_1,  6, N_("Focus mode")},
	{MNOTE_CANON_TAG_SETTINGS_1,  8, N_("Record mode")},
	{MNOTE_CANON_TAG_SETTINGS_1,  9, N_("Image size")},
	{MNOTE_CANON_TAG_SETTINGS_1, 10, N_("Easy shooting mode")},
	{MNOTE_CANON_TAG_SETTINGS_1, 11, N_("Digital zoom")},
	{MNOTE_CANON_TAG_SETTINGS_1, 12, N_("Contrast")},
	{MNOTE_CANON_TAG_SETTINGS_1, 13, N_("Saturation")},
	{MNOTE_CANON_TAG_SETTINGS_1, 14, N_("Sharpness")},
	{MNOTE_CANON_TAG_SETTINGS_1, 15, N_("ISO")},
	{MNOTE_CANON_TAG_SETTINGS_1, 16, N_("Metering mode")},
	{MNOTE_CANON_TAG_SETTINGS_1, 17, N_("Focus range")},
	{MNOTE_CANON_TAG_SETTINGS_1, 18, N_("AF point")},
	{MNOTE_CANON_TAG_SETTINGS_1, 19, N_("Exposure mode")},
	{MNOTE_CANON_TAG_SETTINGS_1, 21, N_("Lens type")},
	{MNOTE_CANON_TAG_SETTINGS_1, 22, N_("Long focal length of lens")},
	{MNOTE_CANON_TAG_SETTINGS_1, 23, N_("Short focal length of lens")},
	{MNOTE_CANON_TAG_SETTINGS_1, 24, N_("Focal units per mm")},
	{MNOTE_CANON_TAG_SETTINGS_1, 25, N_("Maximal aperture")},
	{MNOTE_CANON_TAG_SETTINGS_1, 26, N_("Minimal aperture")},
	{MNOTE_CANON_TAG_SETTINGS_1, 27, N_("Flash activity")},
	{MNOTE_CANON_TAG_SETTINGS_1, 28, N_("Flash details")},
	{MNOTE_CANON_TAG_SETTINGS_1, 31, N_("Focus mode")},
	{MNOTE_CANON_TAG_SETTINGS_1, 32, N_("AE setting")},
	{MNOTE_CANON_TAG_SETTINGS_1, 33, N_("Image stabilization")},
	{MNOTE_CANON_TAG_SETTINGS_1, 34, N_("Display aperture")},
	{MNOTE_CANON_TAG_SETTINGS_1, 35, N_("Zoom source width")},
	{MNOTE_CANON_TAG_SETTINGS_1, 36, N_("Zoom target width")},
	{MNOTE_CANON_TAG_SETTINGS_1, 39, N_("Photo effect")},
	{MNOTE_CANON_TAG_SETTINGS_1, 40, N_("Manual flash output")},
	{MNOTE_CANON_TAG_SETTINGS_1, 41, N_("Color tone")},
	{MNOTE_CANON_TAG_FOCAL_LENGTH, 0, N_("Focal type")},
	{MNOTE_CANON_TAG_FOCAL_LENGTH, 1, N_("Focal length")},
	{MNOTE_CANON_TAG_FOCAL_LENGTH, 2, N_("Focal plane x size")},
	{MNOTE_CANON_TAG_FOCAL_LENGTH, 3, N_("Focal plane y size")},
	{MNOTE_CANON_TAG_SETTINGS_2, 0, N_("Auto ISO")},
	{MNOTE_CANON_TAG_SETTINGS_2, 1, N_("Shot ISO")},
	{MNOTE_CANON_TAG_SETTINGS_2, 2, N_("Measured EV")},
	{MNOTE_CANON_TAG_SETTINGS_2, 3, N_("Target aperture")},
	{MNOTE_CANON_TAG_SETTINGS_2, 4, N_("Target exposure time")},
	{MNOTE_CANON_TAG_SETTINGS_2, 5, N_("Exposure compensation")},
	{MNOTE_CANON_TAG_SETTINGS_2, 6, N_("White balance")},
	{MNOTE_CANON_TAG_SETTINGS_2, 7, N_("Slow shutter")},
	{MNOTE_CANON_TAG_SETTINGS_2, 8, N_("Sequence number")},
	{MNOTE_CANON_TAG_SETTINGS_2, 12, N_("Flash guide number")},
	{MNOTE_CANON_TAG_SETTINGS_2, 13, N_("AF point")},
	{MNOTE_CANON_TAG_SETTINGS_2, 14, N_("Flash exposure compensation")},
	{MNOTE_CANON_TAG_SETTINGS_2, 15, N_("AE bracketing")},
	{MNOTE_CANON_TAG_SETTINGS_2, 16, N_("AE bracket value")},
	{MNOTE_CANON_TAG_SETTINGS_2, 18, N_("Focus distance upper")},
	{MNOTE_CANON_TAG_SETTINGS_2, 19, N_("Focus distance lower")},
	{MNOTE_CANON_TAG_SETTINGS_2, 20, N_("FNumber")},
	{MNOTE_CANON_TAG_SETTINGS_2, 21, N_("Exposure time")},
	{MNOTE_CANON_TAG_SETTINGS_2, 23, N_("Bulb duration")},
	{MNOTE_CANON_TAG_SETTINGS_2, 25, N_("Camera type")},
	{MNOTE_CANON_TAG_SETTINGS_2, 26, N_("Auto rotate")},
	{MNOTE_CANON_TAG_SETTINGS_2, 27, N_("ND filter")},
	{MNOTE_CANON_TAG_SETTINGS_2, 28, N_("Self-timer")},
	{MNOTE_CANON_TAG_SETTINGS_2, 32, N_("Manual flash output")},
	{MNOTE_CANON_TAG_PANORAMA, 2, N_("Panorama frame")},
	{MNOTE_CANON_TAG_PANORAMA, 5, N_("Panorama direction")},
	{MNOTE_CANON_TAG_COLOR_INFORMATION, 0, N_("Tone curve")},
	{MNOTE_CANON_TAG_COLOR_INFORMATION, 2, N_("Sharpness frequency")},
	{MNOTE_CANON_TAG_COLOR_INFORMATION, 7, N_("White balance")},
	{MNOTE_CANON_TAG_COLOR_INFORMATION, 9, N_("Picture style")},
#endif
	{0, 0, NULL}
};

const char *
mnote_canon_tag_get_name (MnoteCanonTag t)
{
	unsigned int i;

	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return table[i].name; /* do not translate */
	return NULL;
}

const char *
mnote_canon_tag_get_name_sub (MnoteCanonTag t, unsigned int s, ExifDataOption o)
{
	unsigned int i;
	int tag_found = 0;

	for (i = 0; i < sizeof (table_sub) / sizeof (table_sub[0]); i++) {
		if (table_sub[i].tag == t) {
			if (table_sub[i].subtag == s)
				return table_sub[i].name;
			tag_found = 1;
		}
	}
	if (!tag_found || !(o & EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS))
		return mnote_canon_tag_get_name (t);
	else
		return NULL;
}

const char *
mnote_canon_tag_get_title (MnoteCanonTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR); 
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (_(table[i].title));
	return NULL;
}

const char *
mnote_canon_tag_get_title_sub (MnoteCanonTag t, unsigned int s, ExifDataOption o)
{
	unsigned int i;
	int tag_found = 0;

	for (i = 0; i < sizeof (table_sub) / sizeof (table_sub[0]); i++) {
		if (table_sub[i].tag == t) {
			if (table_sub[i].subtag == s)
				return _(table_sub[i].name);
			tag_found = 1;
		}
	}
	if (!tag_found || !(o & EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS))
		return mnote_canon_tag_get_title (t);
	else
		return NULL;
}

const char *
mnote_canon_tag_get_description (MnoteCanonTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) {
			if (!*table[i].description)
				return "";
			return (_(table[i].description));
		}
	return NULL;
}
