/* exif-format.c
 *
 * Copyright © 2001 Lutz Müller <lutz@users.sourceforge.net>
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
#include "exif-format.h"
#include "i18n.h"

#include <stdlib.h>

static struct {
        ExifFormat format;
	const char *name;
        unsigned char size;
} ExifFormatTable[] = {
        {EXIF_FORMAT_BYTE,      "Byte",      1},
        {EXIF_FORMAT_ASCII,     "Ascii",     1},
        {EXIF_FORMAT_SHORT,     "Short",     2},
        {EXIF_FORMAT_LONG,      "Long",      4},
        {EXIF_FORMAT_RATIONAL,  "Rational",  8},
	{EXIF_FORMAT_SBYTE,     "SByte",     1},
	{EXIF_FORMAT_SSHORT,    "SShort",    2},
        {EXIF_FORMAT_SLONG,     "SLong",     4},
        {EXIF_FORMAT_SRATIONAL, "SRational", 8},
	{EXIF_FORMAT_FLOAT,     "Float",     4},
	{EXIF_FORMAT_DOUBLE,    "Double",    8},
        {EXIF_FORMAT_UNDEFINED, N_("Undefined"), 1},
        {0, NULL, 0}
};

const char *
exif_format_get_name (ExifFormat format)
{
	unsigned int i;

	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	bindtextdomain (GETTEXT_PACKAGE, LIBEXIF_LOCALEDIR);

	for (i = 0; ExifFormatTable[i].name; i++)
		if (ExifFormatTable[i].format == format)
			return (_(ExifFormatTable[i].name));
	return (NULL);
}

unsigned char
exif_format_get_size (ExifFormat format)
{
	unsigned int i;

	for (i = 0; ExifFormatTable[i].size; i++)
		if (ExifFormatTable[i].format == format)
			return (ExifFormatTable[i].size);
	return (0);
}
