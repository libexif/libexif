/* mnote-pentax-tag.c:
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

#include <config.h>
#include "mnote-pentax-tag.h"

#include <stdlib.h>

#include <libexif/i18n.h>

static struct {
	MnotePentaxTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {
	{MNOTE_PENTAX_TAG_MODE, "Mode", N_("Capture Mode"), ""},
	{MNOTE_PENTAX_TAG_QUALITY, "Quality", N_("Quality Level"), ""},
	{MNOTE_PENTAX_TAG_FOCUS, "Focus", N_("Focus Mode"), ""},
	{MNOTE_PENTAX_TAG_FLASH, "Flash", N_("Flash Mode"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_05, "Unknown5", N_("Unknown 5"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_06, "Unknown6", N_("Unknown 6"), ""},
	{MNOTE_PENTAX_TAG_WHITE_BALANCE, "WhiteBalance", N_("White Balance"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_08, "Unknown8", N_("Unknown 8"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_09, "Unknown9", N_("Unknown 9"), ""},
	{MNOTE_PENTAX_TAG_ZOOM, "Zoom", N_("Zoom"), ""},
	{MNOTE_PENTAX_TAG_SHARPNESS, "Sharpness", N_("Sharpness"), ""},
	{MNOTE_PENTAX_TAG_CONTRAST, "Contrast", N_("Contrast"), ""},
	{MNOTE_PENTAX_TAG_SATURATION, "Saturation", N_("Saturation"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_14, "Unknown14", N_("Unknown 14"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_15, "Unknown15", N_("Unknown 15"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_16, "Unknown16", N_("Unknown 16"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_17, "Unknown17", N_("Unknown 17"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_18, "Unknown18", N_("Unknown 18"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_19, "Unknown19", N_("Unknown 19"), ""},
	{MNOTE_PENTAX_TAG_ISO_SPEED, "ISOSpeed", N_("ISOSpeed"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_21, "Unknown21", N_("Unknown 21"), ""},
	{MNOTE_PENTAX_TAG_COLOR, "Color", N_("Color"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_24, "Unknown24", N_("Unknown 24"), ""},
	{MNOTE_PENTAX_TAG_UNKNOWN_25, "Unknown25", N_("Unknown 25"), ""},
	{MNOTE_PENTAX_TAG_PRINTIM, "PrintIM", N_("PrintIM Settings"), ""},
	{MNOTE_PENTAX_TAG_TZ_CITY, "TimeZone", N_("TimeZone"), ""},
	{MNOTE_PENTAX_TAG_TZ_DST, "DaylightSavings", N_("DaylightSavings"), ""},
	{0, NULL, NULL, NULL}
};

const char *
mnote_pentax_tag_get_name (MnotePentaxTag t)
{
	unsigned int i;

	for (i = 0; table[i].name; i++) if (table[i].tag == t) break;
	return (table[i].name);
}

const char *
mnote_pentax_tag_get_title (MnotePentaxTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LIBMNOTE_LOCALEDIR);
	for (i = 0; table[i].title; i++) if (table[i].tag == t) break; 
	return (_(table[i].title));
}

const char *
mnote_pentax_tag_get_description (MnotePentaxTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LIBMNOTE_LOCALEDIR);
	for (i = 0; table[i].description; i++) if (table[i].tag == t) break;
	return (_(table[i].description));
}
