/* mnote-pentax-entry.c
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
#include "mnote-pentax-entry.h"

#include <libexif/i18n.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-format.h>
#include <libexif/exif-utils.h>

#if 0
void
mnote_pentax_entry_dump (MnoteEntry *e, unsigned int indent)
{
	MnotePentaxEntry *entry = (MnotePentaxEntry *)e;

	char buf[1024];
	unsigned int i;

	for (i = 0; i < 2 * indent; i++)
		buf[i] = ' ';
	buf[i] = '\0';

	if (!e)
		return;

	printf ("%sTag: 0x%x ('%s')\n", buf, entry->tag,
		mnote_pentax_tag_get_name (entry->tag));
	printf ("%s  Format: %i ('%s')\n", buf, entry->format,
		exif_format_get_name (entry->format));
	printf ("%s  Components: %i\n", buf, (int) entry->components);
	printf ("%s  Size: %i\n", buf, entry->size);
	printf ("%s  Value: %s\n", buf, mnote_pentax_entry_get_value (entry));
}

#endif

#define CF(format,target,v)                                     \
{                                                               \
        if (format != target) {                                 \
                snprintf (v, sizeof (v),                        \
                        _("Invalid format '%s', "               \
                        "expected '%s'."),                      \
                        exif_format_get_name (format),          \
                        exif_format_get_name (target));         \
                break;                                          \
        }                                                       \
}

#define CC(number,target,v)                                             \
{                                                                       \
        if (number != target) {                                         \
                snprintf (v, sizeof (v),                                \
                        _("Invalid number of components (%i, "          \
                        "expected %i)."), (int) number, (int) target);  \
                break;                                                  \
        }                                                               \
}

char *
mnote_pentax_entry_get_value (MnotePentaxEntry *entry)
{
	static char v[1024];
	ExifLong vl;
	ExifShort vs;

	if (!entry)
		return (NULL);

	memset (v, 0, sizeof (v));
	switch (entry->tag) {
	case MNOTE_PENTAX_TAG_MODE:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Auto"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Night-scene"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Manual"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_QUALITY:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Good"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Better"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Best"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_FOCUS:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 3:
			strncpy (v, _("Auto"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Custom"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_FLASH:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (v, _("Auto"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Flash On"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("Flash Off"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("Red-eye Reduction"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_WHITE_BALANCE:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Auto"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Daylight"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Shade"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Tungsten"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("Fluorescent"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("Manual"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_ZOOM:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 1, v);
		vl = exif_get_long (entry->data, entry->order);
		switch (vl) {
		default:
			snprintf (v, sizeof (v), "%li", vl);
		}
		break;
	case MNOTE_PENTAX_TAG_SHARPNESS:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Soft"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Hard"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_CONTRAST:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Low"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("High"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_SATURATION:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Low"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("High"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_ISO_SPEED:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 10:
			strncpy (v, _("100"), sizeof (v));
			break;
		case 16:
			strncpy (v, _("200"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_PRINTIM:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, v);
		CC (entry->components, 124, v);
		snprintf (v,  sizeof(v), "%li bytes unknown data", entry->components);
		break;
	case MNOTE_PENTAX_TAG_TZ_CITY:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, v);
		CC (entry->components, 4, v);
		snprintf (v,  entry->components, "%s", entry->data);
		break;
	case MNOTE_PENTAX_TAG_TZ_DST:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, v);
		CC (entry->components, 4, v);
		snprintf (v, entry->components, "%s", entry->data);
		break;
	case MNOTE_PENTAX_TAG_COLOR:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (v, _("Full"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Black & White"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Sepia"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", vs);
		}
		break;
	default:
		switch (entry->format) {
		case EXIF_FORMAT_ASCII:
		  snprintf (v, entry->components, "%s", entry->data);
		  break;
		case EXIF_FORMAT_SHORT:
		  vs = exif_get_short (entry->data, entry->order);
		  snprintf (v, sizeof (v), "%i", vs);
		  break;
		case EXIF_FORMAT_LONG:
		  vl = exif_get_long (entry->data, entry->order);
		  snprintf (v, sizeof (v), "%li", vl);
		  break;
		case EXIF_FORMAT_UNDEFINED:
		default:
		  snprintf (v,  sizeof(v), "%li bytes unknown data",
			    entry->components);
		  break;
		}
		break;
	}

	return (v);
}
