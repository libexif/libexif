/* mnote-olympus-entry.c
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
#include "mnote-olympus-entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-format.h>
#include <libexif/exif-utils.h>
#include <libexif/i18n.h>

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

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

char *
mnote_olympus_entry_get_value (MnoteOlympusEntry *entry)
{
	char v[1024], buf[1024];
	ExifLong vl;
	ExifShort vs;
	ExifRational vr;

	if (!entry)
		return (NULL);

	memset (v, 0, sizeof (v));
	switch (entry->tag) {
	case MNOTE_OLYMPUS_TAG_MODE:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 3, v);
		vl = exif_get_long (entry->data, entry->order);
		switch (vl) {
		case 0:
			strncpy (v, _("normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("unknown"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("fast"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("panorama"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%li", vl);
			strncpy (v, buf, sizeof (v));
		}
		strncat (v, "/", sizeof (v) - 1 - strlen(v));
		vl = exif_get_long (entry->data + 4, entry->order);
		snprintf (buf, sizeof (buf), "%li", vl);
		strncat (v, buf, sizeof (v) - 1 - strlen(v));
		strncat (v, "/", sizeof (v) - 1 - strlen(v));
		vl = exif_get_long (entry->data + 4, entry->order);
		switch (vl) {
		case 1:
			strncat (v, _("left to right"),
					sizeof (v) - 1 - strlen(v));
			break;
		case 2:
			strncat (v, _("right to left"),
					sizeof (v) - 1 - strlen(v));
			break;
		case 3:
			strncat (v, _("bottom to top"),
					sizeof (v) - 1 - strlen(v));
			break;
		case 4:
			strncat (v, _("top to bottom"),
					sizeof (v) - 1 - strlen(v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%li", vl);
			strncat (v, buf, sizeof (v) - 1 - strlen(v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_QUALITY:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (v, _("SQ"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("HQ"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("SHQ"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_MACRO:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("no"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("yes"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_1:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		strncpy (v, _("Unknown tag."), sizeof (v));
		break;
	case MNOTE_OLYMPUS_TAG_DIGIZOOM:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("1x"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("2x"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_2:
		CF (entry->format, EXIF_FORMAT_RATIONAL, v);
		CC (entry->components, 1, v);
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_3:
		CF (entry->format, EXIF_FORMAT_SSHORT, v);
		CC (entry->components, 1, v);
		break;
	case MNOTE_OLYMPUS_TAG_VERSION:
		CF (entry->format, EXIF_FORMAT_ASCII, v);
		CC (entry->components, 5, v);
		strncpy (v, entry->data, MIN (sizeof (v), entry->size));
		break;
	case MNOTE_OLYMPUS_TAG_INFO:
		CF (entry->format, EXIF_FORMAT_ASCII, v);
		CC (entry->components, 52, v);
		break;
	case MNOTE_OLYMPUS_TAG_ID:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, v);
		CC (entry->components, 32, v);
		strncpy (v, entry->data, MIN (sizeof (v), entry->size));
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_4:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 30, v);
		break;
	case MNOTE_OLYMPUS_TAG_FLASHMODE:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Auto"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Red-eye reduction"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Fill"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Off"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_FOCUSDIST:
		CF (entry->format, EXIF_FORMAT_RATIONAL, v);
		CC (entry->components, 1, v);
		vr = exif_get_rational (entry->data, entry->order);
		if (vr.numerator == 0) {
			strncpy (v, _("Unknown"), sizeof (v));
		}
		else {
			unsigned long tmp = vr.numerator / vr.denominator;
			/* printf("numerator %li, denominator %li\n", vr.numerator, vr.denominator); */
			snprintf (v, sizeof (v), "%li mm", tmp);
		}
		break;
	case MNOTE_OLYMPUS_TAG_SHARPNESS:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Hard"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Soft"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_WBALANCE:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 2, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (v, _("Automatic"), sizeof (v));
			break;
		case 2:
			{
				ExifShort v2 = exif_get_short (entry->data + 2, entry->order);
				unsigned long colorTemp = 0;
				switch (v2) {
				case 2:
					colorTemp = 3000;
					break;
				case 3:
					colorTemp = 3700;
					break;
				case 4:
					colorTemp = 4000;
					break;
				case 5:
					colorTemp = 4500;
					break;
				case 6:
					colorTemp = 5500;
					break;
				case 7:
					colorTemp = 6500;
					break;
				case 9:
					colorTemp = 7500;
					break;
				}
				if (colorTemp) {
					snprintf (v, sizeof (v), "Manual: %liK", colorTemp);
				}
				else {
					strncpy (v, _("Manual: Unknown"), sizeof (v));
				}

			}
			break;
		case 3:
			strncpy (v, _("One-touch"), sizeof (v));
			break;
		default:
			strncpy (v, _("Unknown"), sizeof (v));
			break;
		}
		break;
	case MNOTE_OLYMPUS_TAG_CONTRAST:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Hard"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Soft"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_MANFOCUS:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("No"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Yes"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncpy (v, buf, sizeof (v));
		}
		break;
	default:
		break;
	}

	return (strdup (v));
}
