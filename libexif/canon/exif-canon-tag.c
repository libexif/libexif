/* exif-canon-tag.c
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

#include <config.h>
#include "exif-canon-tag.h"

#include <stdlib.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

static struct {
	ExifCanonTag tag;
	const char *name;
} table[] = {
	{EXIF_CANON_TAG_SETTINGS_1, N_("Settings (first part)")},
	{EXIF_CANON_TAG_SETTINGS_2, N_("Settings (second part)")},
	{EXIF_CANON_TAG_IMAGE_TYPE, N_("Image type")},
	{EXIF_CANON_TAG_FIRMWARE, N_("Firmware version")},
	{EXIF_CANON_TAG_IMAGE_NUMBER, N_("Image number")},
	{EXIF_CANON_TAG_OWNER, N_("Owner name")},
	{EXIF_CANON_TAG_SERIAL_NUMBER, N_("Serial number")},
	{EXIF_CANON_TAG_CUSTOM_FUNCS, N_("Custom functions")},
	{0, NULL}
};

const char *
exif_canon_tag_get_name (ExifCanonTag tag)
{
	unsigned int i;

	for (i = 0; table[i].name; i++)
		if (table[i].tag == tag)
			break;

	return (_(table[i].name));
}
