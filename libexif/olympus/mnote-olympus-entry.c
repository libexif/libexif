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
mnote_olympus_entry_get_value (MnoteOlympusEntry *entry, char *v, unsigned int maxlen)
{
	char buf[32];
	ExifLong vl;
	ExifShort vs;
	ExifRational vr;

	if (!entry)
		return (NULL);

	memset (v, 0, maxlen);
	maxlen--;

	if ((!entry->data) && (entry->components > 0)) return (v);

	switch (entry->tag) {
	
	/* Nikon */
        case MNOTE_NIKON_TAG_FIRMWARE:
                CF (entry->format,  EXIF_FORMAT_UNDEFINED, v, maxlen);
                CC (entry->components, 4, v, maxlen);
                vl =  exif_get_long (entry->data, entry->order);
                snprintf (v, sizeof (v), "0x%04lx", vl);
                break;
        case MNOTE_NIKON_TAG_ISO:
                CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
                CC (entry->components, 2, v, maxlen);
                //vs = exif_get_short (entry->data, entry->order);
                vs = exif_get_short (entry->data + 2, entry->order);
                snprintf (v, sizeof (v), "ISO %hd", vs);
                break;
        case MNOTE_NIKON_TAG_ISO2:
                CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
                CC (entry->components, 2, v, maxlen);
                //vs = exif_get_short (entry->data, entry->order);
                vs = exif_get_short (entry->data + 2, entry->order);
                snprintf (v, sizeof (v), "ISO2 %hd", vs);
                break;
        case MNOTE_NIKON_TAG_QUALITY:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                //CC (entry->components, 8, v, maxlen);
                //vl =  exif_get_long (entry->data  , entry->order);
                //printf("-> 0x%04x\n",entry->data);
                //printf("-> 0x%s<\n",entry->data - 0);
                memcpy(v, entry->data ,entry->components);
                //snprintf (v, sizeof (v), "%s<",  ( entry->data - 9  );
                break;
        case MNOTE_NIKON_TAG_COLORMODE:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_COLORMODE1:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_TOTALPICTURES:
                CF (entry->format, EXIF_FORMAT_LONG, v, maxlen);
                CC (entry->components, 1, v, maxlen);
                vl =  exif_get_long (entry->data  , entry->order);
                snprintf (v, sizeof (v), "%lu",  vl  );
                break;
        case MNOTE_NIKON_TAG_WHITEBALANCE:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_SHARPENING:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_FOCUSMODE:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_FLASHSETTING:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_ISOSELECTION:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_FLASHMODE:
                CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
                memcpy(v, entry->data ,entry->components);
                break;
        case MNOTE_NIKON_TAG_WHITEBALANCEFINE:
                CF (entry->format, EXIF_FORMAT_SSHORT, v, maxlen);
                CC (entry->components, 1, v, maxlen);
                vs =  exif_get_short (entry->data, entry->order);
                snprintf (v, sizeof (v), "%hd", vs);
                break;
        case MNOTE_NIKON_TAG_WHITEBALANCERB:
                CF (entry->format, EXIF_FORMAT_RATIONAL, v, maxlen);
                CC (entry->components, 4, v, maxlen);
                //vr = exif_get_rational (entry->data, entry->order);
                //if (vr.numerator == 0) {
                //      strncpy (v, _("Unknown"), sizeof (v));
                //}
                //else {
                {
                        float r,b;
                        vr = exif_get_rational (entry->data, entry->order);
                        r = (1.0*vr.numerator) / vr.denominator;
                        vr = exif_get_rational (entry->data+8, entry->order);
                        b = (1.0*vr.numerator) / vr.denominator;
                        //printf("numerator %li, denominator %li\n", vr.numerator, vr.denominator);
                        snprintf (v, sizeof (v), "Red Correction %f, Blue Correction %f", r,b);
                }
                break;
        case MNOTE_NIKON_TAG_LENSTYPE:
                CF (entry->format, EXIF_FORMAT_BYTE, v, maxlen);
                CC (entry->components, 1, v, maxlen);
                switch (  *( entry->data)  ) {
                   case  0: snprintf (v, sizeof (v), "AF non D Lens"); break;
                   case  1: snprintf (v, sizeof (v), "manual"); break;
                   case  2: snprintf (v, sizeof (v), "AF-D or AF-S Lens"); break;
                   case  6: snprintf (v, sizeof (v), "AF-D G Lens"); break;
                   case 10: snprintf (v, sizeof (v), "AF-D VR Lens"); break;
                   default: snprintf (v, sizeof (v), "unknown Lens");
                }
		break;
        case MNOTE_NIKON_TAG_LENS:
                CF (entry->format, EXIF_FORMAT_RATIONAL, v, maxlen);
                CC (entry->components, 4, v, maxlen);
                {
                        float c,d;
                        unsigned long a,b;
                        vr = exif_get_rational (entry->data, entry->order);
                        a = vr.numerator / vr.denominator;
                        vr = exif_get_rational (entry->data+8, entry->order);
                        b = vr.numerator / vr.denominator;
                        vr = exif_get_rational (entry->data+16, entry->order);
                        c = (1.0*vr.numerator) / vr.denominator;
                        vr = exif_get_rational (entry->data+24, entry->order);
                        d = (1.0*vr.numerator) / vr.denominator;
                                                                                                         //printf("numerator %li, denominator %li\n", vr.numerator, vr.denominator);                         snprintf (v, sizeof (v), "%ld-%ldmm 1:%3.1f - %3.1f",a,b,c,d);
                }
                break;
        case MNOTE_NIKON_TAG_FLASHUSED:
                CF (entry->format, EXIF_FORMAT_BYTE, v, maxlen);
                CC (entry->components, 1, v, maxlen);
                switch (  *( entry->data)  ) {
                   case  0: snprintf (v, sizeof (v), "Flash did not fire"); break;
                   case  4: snprintf (v, sizeof (v), "Flash unit unknown"); break;
                   case  7: snprintf (v, sizeof (v), "Flash is external"); break;
                   case  9: snprintf (v, sizeof (v), "Flash is on Camera"); break;
                   default: snprintf (v, sizeof (v), "unknown Flash status");
                }
                break;
        case MNOTE_NIKON_TAG_AFFOCUSPOSITION:
                CF (entry->format, EXIF_FORMAT_UNDEFINED, v, maxlen);
                CC (entry->components, 4, v, maxlen);
                switch (  *( entry->data+1)  ) {
                   case  0: snprintf (v, sizeof (v), "AF Position: Center"); break;
                   case  1: snprintf (v, sizeof (v), "AF Position: Top"); break;                   case  2: snprintf (v, sizeof (v), "AF Position: Bottom"); break;
                   case  3: snprintf (v, sizeof (v), "AF Position: Left"); break;
                   case  4: snprintf (v, sizeof (v), "AF Position: Right"); break;
                   default: snprintf (v, sizeof (v), "unknown AF Position");
                }
                break;

	/* Olympus */
	case MNOTE_OLYMPUS_TAG_MODE:
		CF (entry->format, EXIF_FORMAT_LONG, v, maxlen);
		CC (entry->components, 3, v, maxlen);
		vl = exif_get_long (entry->data, entry->order);
		switch (vl) {
		case 0:
			strncpy (v, _("normal"), maxlen);
			break;
		case 1:
			strncpy (v, _("unknown"), maxlen);
			break;
		case 2:
			strncpy (v, _("fast"), maxlen);
			break;
		case 3:
			strncpy (v, _("panorama"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%li"), vl);
		}
		vl = exif_get_long (entry->data + 4, entry->order);
		snprintf (buf, sizeof (buf), "/%li/", vl);
		strncat (v, buf, maxlen - strlen (v));
		vl = exif_get_long (entry->data + 4, entry->order);
		switch (vl) {
		case 1:
			strncat (v, _("left to right"), maxlen - strlen (v));
			break;
		case 2:
			strncat (v, _("right to left"), maxlen - strlen (v));
			break;
		case 3:
			strncat (v, _("bottom to top"), maxlen - strlen (v));
			break;
		case 4:
			strncat (v, _("top to bottom"), maxlen - strlen (v));
			break;
		default:
			snprintf (buf, sizeof (buf), _("%li"), vl);
			strncat (v, buf, maxlen - strlen (v));
		}
		break;
	case MNOTE_OLYMPUS_TAG_QUALITY:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (v, _("SQ"), maxlen);
			break;
		case 2:
			strncpy (v, _("HQ"), maxlen);
			break;
		case 3:
			strncpy (v, _("SHQ"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_MACRO:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("no"), maxlen);
			break;
		case 1:
			strncpy (v, _("yes"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_1:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		strncpy (v, _("Unknown tag."), maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_DIGIZOOM:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("1x"), maxlen);
			break;
		case 2:
			strncpy (v, _("2x"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_2:
		CF (entry->format, EXIF_FORMAT_RATIONAL, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_3:
		CF (entry->format, EXIF_FORMAT_SSHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_VERSION:
		CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
		CC (entry->components, 5, v, maxlen);
		strncpy (v, entry->data, MIN (maxlen, entry->size));
		break;
	case MNOTE_OLYMPUS_TAG_INFO:
		CF (entry->format, EXIF_FORMAT_ASCII, v, maxlen);
		CC (entry->components, 52, v, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_ID:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, v, maxlen);
		CC (entry->components, 32, v, maxlen);
		strncpy (v, entry->data, MIN (maxlen, entry->size));
		break;
	case MNOTE_OLYMPUS_TAG_UNKNOWN_4:
		CF (entry->format, EXIF_FORMAT_LONG, v, maxlen);
		CC (entry->components, 30, v, maxlen);
		break;
	case MNOTE_OLYMPUS_TAG_FLASHMODE:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Auto"), maxlen);
			break;
		case 1:
			strncpy (v, _("Red-eye reduction"), maxlen);
			break;
		case 2:
			strncpy (v, _("Fill"), maxlen);
			break;
		case 3:
			strncpy (v, _("Off"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_FOCUSDIST:
		CF (entry->format, EXIF_FORMAT_RATIONAL, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vr = exif_get_rational (entry->data, entry->order);
		if (vr.numerator == 0) {
			strncpy (v, _("Unknown"), maxlen);
		}
		else {
			unsigned long tmp = vr.numerator / vr.denominator;
			/* printf("numerator %li, denominator %li\n", vr.numerator, vr.denominator); */
			snprintf (v, maxlen, "%li mm", tmp);
		}
		break;
	case MNOTE_OLYMPUS_TAG_SHARPNESS:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (v, _("Hard"), maxlen);
			break;
		case 2:
			strncpy (v, _("Soft"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%i"), vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_WBALANCE:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 2, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 1:
			strncpy (v, _("Automatic"), maxlen);
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
					snprintf (v, maxlen, "Manual: %liK", colorTemp);
				}
				else {
					strncpy (v, _("Manual: Unknown"), maxlen);
				}

			}
			break;
		case 3:
			strncpy (v, _("One-touch"), maxlen);
			break;
		default:
			strncpy (v, _("Unknown"), maxlen);
			break;
		}
		break;
	case MNOTE_OLYMPUS_TAG_CONTRAST:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("Hard"), maxlen);
			break;
		case 1:
			strncpy (v, _("Normal"), maxlen);
			break;
		case 2:
			strncpy (v, _("Soft"), maxlen);
			break;
		default:
			snprintf (v, maxlen, "%i", vs);
		}
		break;
	case MNOTE_OLYMPUS_TAG_MANFOCUS:
		CF (entry->format, EXIF_FORMAT_SHORT, v, maxlen);
		CC (entry->components, 1, v, maxlen);
		vs = exif_get_short (entry->data, entry->order);
		switch (vs) {
		case 0:
			strncpy (v, _("No"), maxlen);
			break;
		case 1:
			strncpy (v, _("Yes"), maxlen);
			break;
		default:
			snprintf (v, maxlen, _("%i"), vs);
		}
		break;
	default:
		break;
	}

	return (v);
}
