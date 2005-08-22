/* mnote-canon-tag.c
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
#include "mnote-canon-tag.h"

#include <stdlib.h>

#include <libexif/i18n.h>

static struct {
	MnoteCanonTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {
	{MNOTE_CANON_TAG_SETTINGS_1, "Settings1", N_("Settings (first part)"), ""},
	{MNOTE_CANON_TAG_SETTINGS_2, "Settings2", N_("Settings (second part)"), ""},
	{MNOTE_CANON_TAG_IMAGE_TYPE, "ImageType", N_("Image type"), ""},
	{MNOTE_CANON_TAG_FIRMWARE, "FirmwareVersion", N_("Firmware version"), ""},
	{MNOTE_CANON_TAG_IMAGE_NUMBER, "ImageNumber", N_("Image number"), ""},
	{MNOTE_CANON_TAG_OWNER, "OwnerName", N_("Owner name"), ""},
	{MNOTE_CANON_TAG_SERIAL_NUMBER, "SerialNumber", N_("Serial number"), ""},
	{MNOTE_CANON_TAG_CUSTOM_FUNCS, "CustomFunctions", N_("Custom functions"), ""},
	{0, NULL, NULL, NULL}
};

static struct {
	MnoteCanonTag tag;
	unsigned int subtag;
	const char *name;
} table_sub[] = {
	{MNOTE_CANON_TAG_SETTINGS_2,  7, N_("White balance")},
	{MNOTE_CANON_TAG_SETTINGS_2,  9, N_("Sequence number")},
	{MNOTE_CANON_TAG_SETTINGS_2, 14, N_("AF point used")},
	{MNOTE_CANON_TAG_SETTINGS_2, 15, N_("Flash bias")},
	{MNOTE_CANON_TAG_SETTINGS_2, 19, N_("Subject distance")},
	{0, 0, NULL}
};

const char *
mnote_canon_tag_get_name (MnoteCanonTag t)
{
	unsigned int i;

	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (_(table[i].name));
	return NULL;
}

const char *
mnote_canon_tag_get_name_sub (MnoteCanonTag t, unsigned int s)
{
	unsigned int i;
	for (i = 0; i < sizeof (table_sub) / sizeof (table_sub[0]); i++)
		if ((table_sub[i].tag == t) && (table_sub[i].subtag == s))
			return _(table_sub[i].name);
	return mnote_canon_tag_get_name (t);
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
mnote_canon_tag_get_title_sub (MnoteCanonTag t, unsigned int s)
{
	unsigned int i;
	for (i = 0; i < sizeof (table_sub) / sizeof (table_sub[0]); i++)
		if ((table_sub[i].tag == t) && (table_sub[i].subtag == s))
			return _(table_sub[i].name);
	return mnote_canon_tag_get_title (t);
}

const char *
mnote_canon_tag_get_description (MnoteCanonTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	for (i = 0; i < sizeof (table) / sizeof (table[0]); i++)
		if (table[i].tag == t) return (_(table[i].description));
	return NULL;
}
