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

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

char *
mnote_olympus_entry_get_value (MnoteOlympusEntry *entry, char *val, unsigned int maxlen)
{
	char buf[32];
	ExifLong vl;
	ExifShort vs;
	ExifRational vr;

	if (!entry)
		return (NULL);

	memset (val, 0, maxlen);
	maxlen--;

	if ((!entry->data) && (entry->components > 0)) return (val);

	switch (entry->tag) {
	case MNOTE_OLYMPUS_TAG_MODE:
		CF (entry->format, EXIF_FORMAT_LONG, val, maxlen);
		CC (entry->components, 3, val, maxlen);
		vl = exif_get_long (entry->data, entry->order);
		switch (vl) {
		case 0:
			strncpy (val, _("normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("unknown"), maxlen);
			break;
		case 2:
			strncpy (val, _("fast"), maxlen);
			break;
		case 3:
			strncpy (val, _("panorama"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%li"), vl);
		}
		vl = exif_get_long (entry->data + 4, entry->order);
		snprintf (buf, sizeof (buf), "/%li/", vl);
		strncat (val, buf, maxlen - strlen(val));
		vl = exif_get_long (entry->data + 4, entry->order);
		switch (vl) {
		case 1:
			strncat (val, _("left to right"), maxlen - strlen(val));
			break;
		case 2:
			strncat (val, _("right to left"), maxlen - strlen(val));
			break;
		case 3:
			strncat (val, _("bottom to top"), maxlen - strlen(val));
			break;
		case 4:
			strncat (val, _("top to bottom"), maxlen - strlen(val));
			break;
		default:
			snprintf (buf, sizeof (buf), _("%li"), vl);
			strncat (val, buf, maxlen - strlen(val));
		}
		break;
	case MNOTE_OLYMPUS_TAG_QUALITY:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (val, _("SQ"), maxlen);
			break;
		case 2:
			strncpy (val, _("HQ"), maxlen);
			break;
		case 3:
			strncpy (val, _("SHQ"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_MACRO:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("no"), maxlen);
			break;
		case 1:
			strncpy (val, _("yes"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_1:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		strncpy (val, _("Unknown tag."), maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_DIGIZOOM:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("1x"), maxlen);
			break;
		case 2:
			strncpy (val, _("2x"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_2:
		CF (entry->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_3:
		CF (entry->format, EXIF_FORMAT_SSHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_VERSION:
		CF (entry->format, EXIF_FORMAT_ASCII, val, maxlen);
		CC (entry->components, 5, val, maxlen);
		strncpy (val, entry->data, MIN (maxlen, entry->size));
		break;
	case MNOTE_OLYMPUS_TAG_INFO:
		CF (entry->format, EXIF_FORMAT_ASCII, val, maxlen);
		CC (entry->components, 52, val, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_ID:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (entry->components, 32, val, maxlen);
		strncpy (val, entry->data, MIN (maxlen, entry->size));
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_4:
		CF (entry->format, EXIF_FORMAT_LONG, val, maxlen);
		CC (entry->components, 30, val, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_FLASHMODE:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Auto"), maxlen);
			break;
		case 1:
			strncpy (val, _("Red-eye reduction"), maxlen);
			break;
		case 2:
			strncpy (val, _("Fill"), maxlen);
			break;
		case 3:
			strncpy (val, _("Off"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_FOCUSDIST:
		CF (entry->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vr = exif_get_rational (entry->data, entry->order);
		if (vr.numerator == 0) {
			strncpy (val, _("Unknown"), maxlen);
		}
		else {
			unsigned long tmp = vr.numerator / vr.denominator;
			/* printf("numerator %li, denominator %li\n", vr.numerator, vr.denominator); */
			snprintf (val, maxlen, "%li mm", tmp);
		}
		break;
	case MNOTE_OLYMPUS_TAG_SHARPNESS:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("Hard"), maxlen);
			break;
		case 2:
			strncpy (val, _("Soft"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_WBALANCE:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 2, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (val, _("Automatic"), maxlen);
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
					snprintf (val, maxlen, "Manual: %liK", colorTemp);
				}
				else {
					strncpy (val, _("Manual: Unknown"), maxlen);
				}

			}
			break;
		case 3:
			strncpy (val, _("One-touch"), maxlen);
			break;
		default:
			strncpy (val, _("Unknown"), maxlen);
			break;
		}
		break;
	case MNOTE_OLYMPUS_TAG_CONTRAST:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("Hard"), maxlen);
			break;
		case 1:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 2:
			strncpy (val, _("Soft"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_MANFOCUS:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (val, _("No"), maxlen);
			break;
		case 1:
			strncpy (val, _("Yes"), maxlen);
			break;
		default:
			snprintf (val, maxlen, _("%i"), vs);
		}
		break;
	default:
		break;
	}

	return (val);
}
