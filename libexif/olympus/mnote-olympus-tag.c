/* mnote-olympus-tag.c:
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
#include "mnote-olympus-tag.h"

#include <libexif/i18n.h>

#include <stdlib.h>

static struct {
	MnoteOlympusTag tag;
	const char *name;
	const char *title;
	const char *description;
} table[] = {
	{MNOTE_OLYMPUS_TAG_MODE, "Mode", N_("Speed/Sequence/Panorama direction"), ""},
	{MNOTE_OLYMPUS_TAG_QUALITY, "Quality", N_("Quality"), ""},
	{MNOTE_OLYMPUS_TAG_MACRO, "Macro", N_("Macro"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_1, "Unknown1", N_("Unknown 1"), ""},
	{MNOTE_OLYMPUS_TAG_DIGIZOOM, "DigiZoom", N_("Digital Zoom"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_2, "Unknown2", N_("Unknown 2"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_3, "Unknown3", N_("Unknown 3"), ""},
	{MNOTE_OLYMPUS_TAG_VERSION, "FirmwareVersion", N_("Firmware version"), ""},
	{MNOTE_OLYMPUS_TAG_INFO, "Info", N_("Info"), ""},
	{MNOTE_OLYMPUS_TAG_ID, "CameraID", N_("Camera ID"), ""},
	{MNOTE_OLYMPUS_TAG_UNKNOWN_4, "Unknown4", N_("Unknown 4"), ""},
	{MNOTE_OLYMPUS_TAG_FLASHMODE, "FlashMode", N_("Flash Mode"), ""},
	{MNOTE_OLYMPUS_TAG_FOCUSDIST, "ManualFocusDistance", N_("Manual Focus Distance"), ""},
	{MNOTE_OLYMPUS_TAG_SHARPNESS, "Sharpness", N_("Sharpness Setting"), ""},
	{MNOTE_OLYMPUS_TAG_WBALANCE, "WhiteBalance", N_("White Balance Setting"), ""},
	{MNOTE_OLYMPUS_TAG_CONTRAST, "Contrast", N_("Contrast Setting"), ""},
	{MNOTE_OLYMPUS_TAG_MANFOCUS, "ManualFocus", N_("Manual Focus"), ""},
	{0, NULL, NULL, NULL}
};

const char *
mnote_olympus_tag_get_name (MnoteOlympusTag t)
{
	unsigned int i;

	for (i = 0; table[i].name; i++) if (table[i].tag == t) break;
	return (table[i].name);
}

const char *
mnote_olympus_tag_get_title (MnoteOlympusTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LIBMNOTE_LOCALEDIR);
	for (i = 0; table[i].title; i++) if (table[i].tag == t) break;
	return (_(table[i].title));
}

const char *
mnote_olympus_tag_get_description (MnoteOlympusTag t)
{
	unsigned int i;

	bindtextdomain (GETTEXT_PACKAGE, LIBMNOTE_LOCALEDIR);
	for (i = 0; table[i].description; i++) if (table[i].tag == t) break;
	return (_(table[i].description));
}
