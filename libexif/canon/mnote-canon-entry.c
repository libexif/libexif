/* mnote-canon-entry.c
 *
 * Copyright © 2002 Lutz Müller <lutz@users.sourceforge.net>
 * Copyright © 2003 Matthieu Castet <mat-c@users.sourceforge.net>
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
#include "mnote-canon-entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-format.h>
#include <libexif/exif-utils.h>
#include <libexif/i18n.h>

/* #define DEBUG */

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define CF(format,target,v,maxlen)                              \
{                                                               \
        if (format != target) {                                 \
                snprintf (v, maxlen,                            \
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
#define CC2(number,t1,t2,v,maxlen)                                      \
{                                                                       \
	if ((number != t1) && (number != t2)) {                         \
		snprintf (v, maxlen,                                    \
			_("Invalid number of components (%i, "          \
			"expected %i or %i)."), (int) number,		\
			(int) t1, (int) t2);  				\
		break;                                                  \
	}                                                               \
}

char *
mnote_canon_entry_get_value (const MnoteCanonEntry *entry, char *val, unsigned int maxlen)
{
    char buf[128];
    ExifLong vl;
    ExifShort vs, n;
    int i;
    unsigned char *data = entry->data;

    if (!entry) return NULL;

    memset (val, 0, maxlen);
    maxlen--;

	switch (entry->tag) {
	case MNOTE_CANON_TAG_SETTINGS_1:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		n = exif_get_short (data, entry->order) / 2;
		data += 2;
		CC (entry->components, n, val, maxlen);
		for (i = 1; i < n; i++) {
		    vs = exif_get_short (data, entry->order);
		    data += 2;
		    switch (i) {
		    case 1:
			strncpy (val, _("Macro mode : "), maxlen);
			switch (vs) {
			case 1:
			    strncat (val, _("Macro"), maxlen - strlen(val));
			    break;
			case 2:
			    strncat (val, _("Normal"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 2:
			if (vs) {
				snprintf (buf, sizeof (buf),
					_(" / Self Timer : %i (ms)"), vs*100);
				strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 4:
			strncat (val, _(" / Flash mode : "), maxlen - strlen(val));
			switch (vs) {
			case 0:
			    strncat (val, _("Flash not fired"), maxlen - strlen(val));
			    break;
			case 1:
			    strncat (val, _("auto"), maxlen - strlen(val));
			    break;
			case 2:
			    strncat (val, _("on"), maxlen - strlen(val));
			    break;
			case 3:
			    strncat (val, _("red eyes reduction"), maxlen - strlen(val));
			    break;
			case 4:
			    strncat (val, _("slow synchro"), maxlen - strlen(val));
			    break;
			case 5:
			    strncat (val, _("auto + red eyes reduction"), maxlen - strlen(val));
			    break;
			case 6:
			    strncat (val, _("on + red eyes reduction"), maxlen - strlen(val));
			    break;
			case 16:
			    strncat (val, _("external"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 5:
			strncat (val, _(" / Continuous drive mode : "), maxlen - strlen(val));
			switch (vs) {
			case 0:
			    strncat (val, _("single or timer"), maxlen - strlen(val));
			    break;
			case 1:
			    strncat (val, _("continuous"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 7:
			strncat (val, _(" / Focus mode : "), maxlen - strlen(val));
			switch (vs) {
			case 0:
			    strncat (val, _("One-Shot"), maxlen - strlen(val));
			    break;
			case 1:
			    strncat (val, _("AI Servo"), maxlen - strlen(val));
			    break;
			case 2:
			    strncat (val, _("AI Focus"), maxlen - strlen(val));
			    break;
			case 3:
			    strncat (val, _("MF"), maxlen - strlen(val));
			    break;
			case 4:
			    strncat (val, _("Single"), maxlen - strlen(val));
			    break;
			case 5:
			    strncat (val, _("Continuous"), maxlen - strlen(val));
			    break;
			case 6:
			    strncat (val, _("MF"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 10:
			strncat (val, _(" / Image size : "), maxlen - strlen(val));
			switch (vs) {
			case 0:
			    strncat (val, _("Large"), maxlen - strlen(val));
			    break;
			case 1:
			    strncat (val, _("Medium"), maxlen - strlen(val));
			    break;
			case 2:
			    strncat (val, _("Small"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 11:
			strncat (val, _(" / Easy shooting mode : "), maxlen - strlen(val));
			switch (vs) {
			case 0:
			    strncat (val, _("Full Auto"), maxlen - strlen(val));
			    break;
			case 1:
			    strncat (val, _("Manual"), maxlen - strlen(val));
			    break;
			case 2:
			    strncat (val, _("Landscape"), maxlen - strlen(val));
			    break;
			case 3:
			    strncat (val, _("Fast Shutter"), maxlen - strlen(val));
			    break;
			case 4:
			    strncat (val, _("Slow Shutter"), maxlen - strlen(val));
			    break;
			case 5:
			    strncat (val, _("Night"), maxlen - strlen(val));
			    break;
			case 6:
			    strncat (val, _("Black & White"), maxlen - strlen(val));
			    break;
			case 7:
			    strncat (val, _("Sepia"), maxlen - strlen(val));
			    break;
			case 8:
			    strncat (val, _("Portrait"), maxlen - strlen(val));
			    break;
			case 9:
			    strncat (val, _("Sports"), maxlen - strlen(val));
			    break;
			case 10:
			    strncat (val, _("Macro / Close-Up"), maxlen - strlen(val));
			    break;
			case 11:
			    strncat (val, _("Pan Focus"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 13:
			strncat (val, _(" / Contrast : "), maxlen - strlen(val));
			switch (vs) {
			case 0xffff:
			    strncat (val, _("Low"), maxlen - strlen(val));
			    break;
			case 0x0000:
			    strncat (val, _("Normal"), maxlen - strlen(val));
			    break;
			case 0x0001:
			    strncat (val, _("High"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 14:
			strncat (val, _(" / Saturation : "), maxlen - strlen(val));
			switch (vs) {
			case 0xffff:
			    strncat (val, _("Low"), maxlen - strlen(val));
			    break;
			case 0x0000:
			    strncat (val, _("Normal"), maxlen - strlen(val));
			    break;
			case 0x0001:
			    strncat (val, _("High"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 15:
			strncat (val, _(" / Sharpness : "), maxlen - strlen(val));
			switch (vs) {
			case 0xffff:
			    strncat (val, _("Low"), maxlen - strlen(val));
			    break;
			case 0x0000:
			    strncat (val, _("Normal"), maxlen - strlen(val));
			    break;
			case 0x0001:
			    strncat (val, _("High"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 16:
			if (vs) {
			    strncat (val, _(" / ISO : "), maxlen - strlen(val));
			    switch (vs) {
			    case 15:
				strncat (val, _("auto"), maxlen - strlen(val));
				break;
			    case 16:
				strncat (val, _("50"), maxlen - strlen(val));
				break;
			    case 17:
				strncat (val, _("100"), maxlen - strlen(val));
				break;
			    case 18:
				strncat (val, _("200"), maxlen - strlen(val));
				break;
			    case 19:
				strncat (val, _("400"), maxlen - strlen(val));
				break;
			    default:
				snprintf (buf, sizeof (buf), _("%i???"), vs);
				strncat (val, buf, maxlen - strlen(val));
			    }
			    break;
			}
		    case 17:
			strncat (val, _(" / Metering mode : "), maxlen - strlen(val));
			switch (vs) {
			case 3:
			    strncat (val, _("Evaluative"), maxlen - strlen(val));
			    break;
			case 4:
			    strncat (val, _("Partial"), maxlen - strlen(val));
			    break;
			case 5:
			    strncat (val, _("Center-weighted"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("%i???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 19:
			strncat (val, _(" / AF point selected : "), maxlen - strlen(val));
			switch (vs) {
			case 0x3000:
			    strncat (val, _("none (MF)"), maxlen - strlen(val));
			    break;
			case 0x3001:
			    strncat (val, _("auto-selected"), maxlen - strlen(val));
			    break;
			case 0x3002:
			    strncat (val, _("right"), maxlen - strlen(val));
			    break;
			case 0x3003:
			    strncat (val, _("center"), maxlen - strlen(val));
			    break;
			case 0x3004:
			    strncat (val, _("left"), maxlen - strlen(val));
			    break;
			default:
			    snprintf (buf, sizeof (buf), _("0x%x???"), vs);
			    strncat (val, buf, maxlen - strlen(val));
			}
			break;
		    case 20:
				strncat (val, _(" / Exposure mode : "), maxlen - strlen(val));
				switch (vs) {
           		case 0:
					strncat (val, _("Easy shooting"), maxlen - strlen(val));
                    break;
                case 1:
					strncat (val, _("Program"), maxlen - strlen(val));
					break;
				case 2:
					strncat (val, _("Tv-priority"), maxlen - strlen(val));
					break;
				case 3:
					strncat (val, _("Av-priority"), maxlen - strlen(val));
					break;
				case 4:
					strncat (val, _("Manual"), maxlen - strlen(val));
					break;
				case 5:
					strncat (val, _("A-DEP"), maxlen - strlen(val));
					break;
				default:
					snprintf (buf, sizeof (buf), _("%i???"), vs);
					strncat (val, buf, maxlen - strlen(val));
                }
				break;
			case 23:
				snprintf (buf, sizeof (buf), _(" / long focal length of lens (in focal units) : %u"), vs);
				strncat (val, buf, maxlen - strlen(val));
				break;
			case 24:
				snprintf (buf, sizeof (buf), _(" / short focal length of lens (in focal units) : %u"), vs);
				strncat (val, buf, maxlen - strlen(val));
				break;
			case 25:
				snprintf (buf, sizeof (buf), _(" / focal units per mm : %u"), vs);
				strncat (val, buf, maxlen - strlen(val));
				break;
			case 29:
				strncat (val, _(" / Flash details : "), maxlen - strlen(val));
				if ((vs>>14)&1)
					strncat (val, _("External E-TTL"), maxlen - strlen(val));
				if ((vs>>13)&1)
					strncat (val, _("Internal flash"), maxlen - strlen(val));
				if ((vs>>11)&1)
					strncat (val, _("FP sync used"), maxlen - strlen(val));
				if ((vs>>4)&1)
					strncat (val, _("FP sync enabled"), maxlen - strlen(val));
#ifdef DEBUG
				printf ("Value29=0x%08x\n", vs);
#endif
				break;
			case 32:
				strncat (val, _(" / Focus mode2 : "), maxlen - strlen(val));
				switch (vs) {
				case 0:
					strncat (val, _("Single"), maxlen - strlen(val));
					break;
				case 1:
					strncat (val, _("Continuous"), maxlen - strlen(val));
					break;
				default:
					snprintf (buf, sizeof (buf), _("%i???"), vs);
					strncat (val, buf, maxlen - strlen(val));
				}
				break;
#ifdef DEBUG
			default:
                        	printf ("Value%d=%d\n", i, vs);
#endif
			}
		}

                break;

	case MNOTE_CANON_TAG_SETTINGS_2:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		n = exif_get_short (data, entry->order)/2;
		data += 2;
		CC (entry->components, n, val, maxlen);
#ifdef DEBUG
		printf ("Setting2 size %d %d\n",n,entry->size);
#endif
		for (i=1;i<n;i++)
		{
			vs = exif_get_short (data, entry->order);
			data+=2;
			switch(i) {
			case 7:
				strncpy (val, _("White balance : "), maxlen - strlen(val));
				switch (vs) {
				case 0:
					strncat (val, _("Auto"), maxlen - strlen(val));
					break;
				case 1:
					strncat (val, _("Sunny"), maxlen - strlen(val));
					break;
				case 2:
					strncat (val, _("Cloudy"), maxlen - strlen(val));
					break;
				case 3:
					strncat (val, _("Tungsten"), maxlen - strlen(val));
					break;
				case 4:
					strncat (val, _("Flourescent"), maxlen - strlen(val));
					break;
				case 5:
					strncat (val, _("Flash"), maxlen - strlen(val));
					break;
				case 6:
					strncat (val, _("Custom"), maxlen - strlen(val));
					break;
				default:
					snprintf (buf, sizeof (buf), _("%i???"), vs);
					strncat (val, buf, maxlen - strlen(val));
				}
				break;
			case 9:
				snprintf (buf, sizeof (buf), _(" / Sequence number : %u"), vs);
				strncat (val, buf, maxlen - strlen(val));
				break;
			case 14:
				if (vs>>12)
				{
					strncat (val, _(" / AF point used : "), maxlen - strlen(val));
					if (vs&1)
						strncat (val, _("Right"), maxlen - strlen(val));
					if ((vs>>1)&1)
						strncat (val, _("Center"), maxlen - strlen(val));
					if ((vs>>2)&1)
						strncat (val, _("Left"), maxlen - strlen(val));
					snprintf (buf, sizeof (buf), _(" (%u available focus point)"), vs>>12);
					strncat (val, buf, maxlen - strlen(val));
				}
#ifdef DEBUG
					printf ("0x%08x\n", vs);
#endif
				break;
			case 15:
				snprintf (buf, sizeof (buf), _(" / Flash bias : %.2f EV"), vs/32.0);
				strncat (val, buf, maxlen - strlen(val));

				break;
			case 19:
				snprintf (buf, sizeof (buf), _(" / Subject Distance (mm) : %u"), vs);
				strncat (val, buf, maxlen - strlen(val));
				break;
#ifdef DEBUG
			default:
				printf ("Value%d=%d\n", i, vs);
#endif
			}
		}

		break;

	case MNOTE_CANON_TAG_IMAGE_TYPE:
	case MNOTE_CANON_TAG_OWNER:
		CF (entry->format, EXIF_FORMAT_ASCII, val, maxlen);
		CC (entry->components, 32, val, maxlen);
		strncpy (val, data, MIN (entry->size, maxlen));
		break;

	case MNOTE_CANON_TAG_FIRMWARE:
		CF (entry->format, EXIF_FORMAT_ASCII, val, maxlen);
		CC2 (entry->components, 24, 32, val, maxlen);
		strncpy (val, data, MIN (entry->size, maxlen));
		break;

	case MNOTE_CANON_TAG_IMAGE_NUMBER:
		CF (entry->format, EXIF_FORMAT_LONG, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vl = exif_get_long (data, entry->order);
		snprintf (val, maxlen, "%03lu-%04lu",
				(unsigned long) vl/10000,
				(unsigned long) vl%10000);
		break;

	case MNOTE_CANON_TAG_SERIAL_NUMBER:
		CF (entry->format, EXIF_FORMAT_LONG, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vl = exif_get_long (data, entry->order);
		snprintf (val, maxlen, "%04X-%05d", (int)vl>>16,(int)vl&0xffff);
		break;

	case MNOTE_CANON_TAG_CUSTOM_FUNCS:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		n = exif_get_short (data, entry->order)/2;
		data+=2;
		CC (entry->components, n, val, maxlen);
#ifdef DEBUG
		printf ("Custom Function size %d %d\n",n,entry->size);
#endif
		for (i=1;i<n;i++)
		{
			vs = exif_get_short (data, entry->order);
			data += 2;
			snprintf (buf, sizeof(buf), _("C.F%d : %u"), i, vs);
			strncat (val, buf, maxlen - strlen(val));
		}
		break;

	default:
#ifdef DEBUG
		if (entry->format == EXIF_FORMAT_SHORT)
		for(i=0;i<entry->components;i++) {
			vs = exif_get_short (data, entry->order);
			data+=2;
			printf ("Value%d=%d\n", i, vs);
		}
		else if (entry->format == EXIF_FORMAT_LONG)
		for(i=0;i<entry->components;i++) {
			vl = exif_get_long (data, entry->order);
			data+=4;
			printf ("Value%d=%d\n", i, vs);
		}
		else if (entry->format == EXIF_FORMAT_ASCII)
		    strncpy (val, data, MIN (entry->size, maxlen));
#endif
		break;
        }

        return val;
}
