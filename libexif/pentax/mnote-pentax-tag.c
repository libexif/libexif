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
	{MNOTE_PENTAX_TAG_MODE, "Mode", N_("Capture Mode"), NULL},
	{MNOTE_PENTAX_TAG_QUALITY, "Quality", N_("Quality Level"), NULL},
	{MNOTE_PENTAX_TAG_FOCUS, "Focus", N_("Focus Mode"), NULL},
	{MNOTE_PENTAX_TAG_FLASH, "Flash", N_("Flash Mode"), NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_05, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_06, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_WHITE_BALANCE, "WhiteBalance", N_("White Balance"), NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_08, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_09, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_ZOOM, "Zoom", N_("Zoom"), NULL},
	{MNOTE_PENTAX_TAG_SHARPNESS, "Sharpness", N_("Sharpness"), NULL},
	{MNOTE_PENTAX_TAG_CONTRAST, "Contrast", N_("Contrast"), NULL},
	{MNOTE_PENTAX_TAG_SATURATION, "Saturation", N_("Saturation"), NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_14, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_15, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_16, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_17, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_18, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_19, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_ISO_SPEED, "ISOSpeed", N_("ISOSpeed"), NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_21, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_COLOR, "Color", N_("Color"), NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_24, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_UNKNOWN_25, NULL, NULL, NULL},
	{MNOTE_PENTAX_TAG_PRINTIM, "PrintIM", N_("PrintIM Settings"), NULL},
	{MNOTE_PENTAX_TAG_TZ_CITY, "TimeZone", N_("TimeZone"), NULL},
	{MNOTE_PENTAX_TAG_TZ_DST, "DaylightSavings", N_("DaylightSavings"), NULL},
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
		if (table[i].tag == t) return (_(table[i].description));
	return NULL;
}
