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


#define CF(format,target,v,maxlen)                              \
{                                                               \
        if (format != target) {                                 \
                snprintf (v, maxlen,	                        \
                        _("Invalid format '%s', "               \
                        "expected '%s'."),                      \
                        exif_format_get_name (format),          \
                        exif_format_get_name (target));         \
                break;                                          \
        }                                                       \
}

#define CC(number,target,v,maxlen)                                      \
{                                                                       \
        if (number != target) {                                         \
                snprintf (v, maxlen,                                    \
                        _("Invalid number of components (%i, "          \
                        "expected %i)."), (int) number, (int) target);  \
                break;                                                  \
        }                                                               \
}

char *
mnote_pentax_entry_get_value (MnotePentaxEntry *entry, char *val, unsigned int maxlen)
{
	ExifLong vl;
	ExifShort vs;

	if (!entry)
		return (NULL);

	memset (val, 0, maxlen);
	maxlen--;

	switch (entry->tag) {
	case MNOTE_PENTAX_TAG_MODE:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Auto"), maxlen);
			break;
		case 1:
			strncpy (val, _("Night-scene"), maxlen);
			break;
		case 2:
			strncpy (val, _("Manual"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_QUALITY:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Good"), maxlen);
			break;
		case 1:
			strncpy (val, _("Better"), maxlen);
			break;
		case 2:
			strncpy (val, _("Best"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_FOCUS:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 3:
			strncpy (val, _("Auto"), maxlen);
			break;
		case 2:
			strncpy (val, _("Custom"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_FLASH:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (val, _("Auto"), maxlen);
			break;
		case 2:
			strncpy (val, _("Flash On"), maxlen);
			break;
		case 4:
			strncpy (val, _("Flash Off"), maxlen);
			break;
		case 6:
			strncpy (val, _("Red-eye Reduction"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_WHITE_BALANCE:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Auto"), maxlen);
			break;
		case 1:
			strncpy (val, _("Daylight"), maxlen);
			break;
		case 2:
			strncpy (val, _("Shade"), maxlen);
			break;
		case 3:
			strncpy (val, _("Tungsten"), maxlen);
			break;
		case 4:
			strncpy (val, _("Fluorescent"), maxlen);
			break;
		case 5:
			strncpy (val, _("Manual"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_ZOOM:
		CF (entry->format, EXIF_FORMAT_LONG, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vl = exif_get_long (entry->data, entry->order);
		snprintf (val, maxlen, "%li", vl);
		break;
	case MNOTE_PENTAX_TAG_SHARPNESS:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("Soft"), maxlen);
			break;
		case 2:
			strncpy (val, _("Hard"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_CONTRAST:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("Low"), maxlen);
			break;
		case 2:
			strncpy (val, _("High"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_SATURATION:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("Low"), maxlen);
			break;
		case 2:
			strncpy (val, _("High"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_ISO_SPEED:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 10:
			strncpy (val, _("100"), maxlen);
			break;
		case 16:
			strncpy (val, _("200"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_PENTAX_TAG_PRINTIM:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (entry->components, 124, val, maxlen);
		snprintf (val, maxlen, "%li bytes unknown data", entry->components);
		break;
	case MNOTE_PENTAX_TAG_TZ_CITY:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (entry->components, 4, val, maxlen);
		snprintf (val, entry->components, "%s", entry->data);
		break;
	case MNOTE_PENTAX_TAG_TZ_DST:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (entry->components, 4, val, maxlen);
		snprintf (val, entry->components, "%s", entry->data);
		break;
	case MNOTE_PENTAX_TAG_COLOR:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (val, _("Full"), maxlen);
			break;
		case 2:
			strncpy (val, _("Black & White"), maxlen);
			break;
		case 3:
			strncpy (val, _("Sepia"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	default:
		switch (entry->format) {
		case EXIF_FORMAT_ASCII:
		  snprintf (val, entry->components, "%s", entry->data);
		  break;
		case EXIF_FORMAT_SHORT:
		  vs = exif_get_short (entry->data, entry->order);
		  snprintf (val, maxlen, "%i", vs);
		  break;
		case EXIF_FORMAT_LONG:
		  vl = exif_get_long (entry->data, entry->order);
		  snprintf (val, maxlen, "%li", vl);
		  break;
		case EXIF_FORMAT_UNDEFINED:
		default:
		  snprintf (val, maxlen, "%li bytes unknown data",
			    entry->components);
		  break;
		}
		break;
	}

	return (val);
}
