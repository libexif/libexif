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
#define CC2(number,t1,t2,v)                                             \
{                                                                       \
	if ((number != t1) && (number != t2)) {                         \
		snprintf (v, sizeof (v),                                \
			_("Invalid number of components (%i, "          \
			"expected %i or %i)."), (int) number,		\
			(int) t1, (int) t2);  				\
		break;                                                  \
	}                                                               \
}

char *
mnote_canon_entry_get_value (const MnoteCanonEntry *entry)
{
        char v[1024], buf[1024];
        ExifLong vl;
        ExifShort vs, n;
	int i;
	unsigned char *data = entry->data;

        if (!entry) return NULL;

        memset (v, 0, sizeof (v));
	switch (entry->tag) {
	case MNOTE_CANON_TAG_SETTINGS_1:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		n = exif_get_short (data, entry->order) / 2;
		data += 2;
		CC (entry->components, n, v);
		for (i = 1; i < n; i++) {
		    vs = exif_get_short (data, entry->order);
		    data += 2;
		    switch (i) {
		    case 1:
			strncpy (v, _("Macro mode : "), sizeof (v) - 1);
			switch (vs) {
			case 1:
			    strncat (v, _("Macro"), sizeof (v) - 1 - strlen(v));
			    break;
			case 2:
			    strncat (v, _("Normal"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 2:
			if (vs) {
				snprintf (buf, sizeof (buf),
					_(" / Self Timer : %i (ms)"), vs*100);
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 4:
			strncat (v,_(" / Flash mode : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0:
			    strncat (v, _("Flash not fired"), sizeof (v) - 1 - strlen(v));
			    break;
			case 1:
			    strncat (v, _("auto"), sizeof (v) - 1 - strlen(v));
			    break;
			case 2:
			    strncat (v, _("on"), sizeof (v) - 1 - strlen(v));
			    break;
			case 3:
			    strncat (v, _("red eyes reduction"), sizeof (v) - 1 - strlen(v));
			    break;
			case 4:
			    strncat (v, _("slow synchro"), sizeof (v) - 1 - strlen(v));
			    break;
			case 5:
			    strncat (v, _("auto + red eyes reduction"), sizeof (v) - 1 - strlen(v));
			    break;
			case 6:
			    strncat (v, _("on + red eyes reduction"), sizeof (v) - 1 - strlen(v));
			    break;
			case 16:
			    strncat (v, _("external"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 5:
			strncat (v, _(" / Continuous drive mode : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0:
			    strncat (v, _("single or timer"), sizeof (v) - 1 - strlen(v));
			    break;
			case 1:
			    strncat (v, _("continuous"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v, buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 7:
			strncat (v, _(" / Focus mode : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0:
			    strncat (v, _("One-Shot"), sizeof (v) - 1 - strlen(v));
			    break;
			case 1:
			    strncat (v, _("AI Servo"), sizeof (v) - 1 - strlen(v));
			    break;
			case 2:
			    strncat (v, _("AI Focus"), sizeof (v) - 1 - strlen(v));
			    break;
			case 3:
			    strncat (v, _("MF"), sizeof (v) - 1 - strlen(v));
			    break;
			case 4:
			    strncat (v, _("Single"), sizeof (v) - 1 - strlen(v));
			    break;
			case 5:
			    strncat (v, _("Continuous"), sizeof (v) - 1 - strlen(v));
			    break;
			case 6:
			    strncat (v, _("MF"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 10:
			strncat (v, _(" / Image size : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0:
			    strncat (v, _("Large"), sizeof (v) - 1 - strlen(v));
			    break;
			case 1:
			    strncat (v, _("Medium"), sizeof (v) - 1 - strlen(v));
			    break;
			case 2:
			    strncat (v, _("Small"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 11:
			strncat (v, _(" / Easy shooting mode : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0:
			    strncat (v, _("Full Auto"), sizeof (v) - 1 - strlen(v));
			    break;
			case 1:
			    strncat (v, _("Manual"), sizeof (v) - 1 - strlen(v));
			    break;
			case 2:
			    strncat (v, _("Landscape"), sizeof (v) - 1 - strlen(v));
			    break;
			case 3:
			    strncat (v, _("Fast Shutter"), sizeof (v) - 1 - strlen(v));
			    break;
			case 4:
			    strncat (v, _("Slow Shutter"), sizeof (v) - 1 - strlen(v));
			    break;
			case 5:
			    strncat (v, _("Night"), sizeof (v) - 1 - strlen(v));
			    break;
			case 6:
			    strncat (v, _("Black & White"), sizeof (v) - 1 - strlen(v));
			    break;
			case 7:
			    strncat (v, _("Sepia"), sizeof (v) - 1 - strlen(v));
			    break;
			case 8:
			    strncat (v, _("Portrait"), sizeof (v) - 1 - strlen(v));
			    break;
			case 9:
			    strncat (v, _("Sports"), sizeof (v) - 1 - strlen(v));
			    break;
			case 10:
			    strncat (v, _("Macro / Close-Up"), sizeof (v) - 1 - strlen(v));
			    break;
			case 11:
			    strncat (v, _("Pan Focus"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 13:
			strncat (v, _(" / Contrast : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0xffff:
			    strncat (v, _("Low"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x0000:
			    strncat (v, _("Normal"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x0001:
			    strncat (v, _("High"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 14:
			strncat (v, _(" / Saturation : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0xffff:
			    strncat (v, _("Low"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x0000:
			    strncat (v, _("Normal"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x0001:
			    strncat (v, _("High"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 15:
			strncat (v, _(" / Sharpness : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0xffff:
			    strncat (v, _("Low"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x0000:
			    strncat (v, _("Normal"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x0001:
			    strncat (v, _("High"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 16:
			if (vs) {
			    strncat (v, _(" / ISO : "), sizeof (v) - 1 - strlen(v));
			    switch (vs) {
			    case 15:
				strncat (v, _("auto"), sizeof (v) - 1 - strlen(v));
				break;
			    case 16:
				strncat (v, _("50"), sizeof (v) - 1 - strlen(v));
				break;
			    case 17:
				strncat (v, _("100"), sizeof (v) - 1 - strlen(v));
				break;
			    case 18:
				strncat (v, _("200"), sizeof (v) - 1 - strlen(v));
				break;
			    case 19:
				strncat (v, _("400"), sizeof (v) - 1 - strlen(v));
				break;
			    default:
				snprintf (buf, sizeof (buf), "%i???", vs);
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
			    }
			    break;
			}
		    case 17:
			strncat (v, _(" / Metering mode : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 3:
			    strncat (v, _("Evaluative"), sizeof (v) - 1 - strlen(v));
			    break;
			case 4:
			    strncat (v, _("Partial"), sizeof (v) - 1 - strlen(v));
			    break;
			case 5:
			    strncat (v, _("Center-weighted"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "%i???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 19:
			strncat (v, _(" / AF point selected : "), sizeof (v) - 1 - strlen(v));
			switch (vs) {
			case 0x3000:
			    strncat (v, _("none (MF)"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x3001:
			    strncat (v, _("auto-selected"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x3002:
			    strncat (v, _("right"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x3003:
			    strncat (v, _("center"), sizeof (v) - 1 - strlen(v));
			    break;
			case 0x3004:
			    strncat (v, _("left"), sizeof (v) - 1 - strlen(v));
			    break;
			default:
			    snprintf (buf, sizeof (buf), "0x%x???", vs);
			    strncat (v,buf, sizeof (v) - 1 - strlen(v));
			}
			break;
		    case 20:
				strncat (v, _(" / Exposure mode : "), sizeof (v) - 1 - strlen(v));
				switch (vs) {
                		case 0:
					strncat (v, _("Easy shooting"), sizeof (v) - 1 - strlen(v));
                        		break;
                		case 1:
                        		strncat (v, _("Program"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 2:
                        		strncat (v, _("Tv-priority"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 3:
                        		strncat (v, _("Av-priority"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 4:
                        		strncat (v, _("Manual"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 5:
                        		strncat (v, _("A-DEP"), sizeof (v) - 1 - strlen(v));
                        		break;
				default:
                        		snprintf (buf, sizeof (buf), "%i???", vs);
					strncat (v,buf, sizeof (v) - 1 - strlen(v));
                		}
				break;
			case 23:
				snprintf (buf, sizeof (buf), " / %s : %u", _("long focal length of lens (in focal units)"), vs);
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
				break;
			case 24:
				snprintf (buf, sizeof (buf), " / %s : %u", _("short focal length of lens (in focal units)"), vs);
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
				break;
			case 25:
				snprintf (buf, sizeof (buf), " / %s : %u", _("focal units per mm"), vs);
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
				break;
			case 29:
				strncat (v, _(" / Flash details : "), sizeof (v) - 1 - strlen(v));
				if ((vs>>14)&1)
					strncat (v, _("External E-TTL"), sizeof (v) - 1 - strlen(v));
                		if ((vs>>13)&1)
                        		strncat (v, _("Internal flash"), sizeof (v) - 1 - strlen(v));
				if ((vs>>11)&1)
                        		strncat (v, _("FP sync used"), sizeof (v) - 1 - strlen(v));
				if ((vs>>4)&1)
                        		strncat (v, _("FP sync enabled"), sizeof (v) - 1 - strlen(v));
#ifdef DEBUG
                        	printf ("Value29=0x%08x\n", vs);
#endif
				break;
			case 32:
				snprintf (buf, sizeof (buf), " / %s : ", _("Focus mode2"));
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
				switch (vs) {
                		case 0:
					strncat (v, _("Single"), sizeof (v) - 1 - strlen(v));
                        		break;
                		case 1:
                        		strncat (v, _("Continuous"), sizeof (v) - 1 - strlen(v));
                        		break;
				default:
                        		snprintf (buf, sizeof (buf), "%i???", vs);
					strncat (v,buf, sizeof (v) - 1 - strlen(v));
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
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		n = exif_get_short (data, entry->order)/2;
		data += 2;
		CC (entry->components, n, v);
#ifdef DEBUG
		printf ("Setting2 size %d %d\n",n,entry->size);
#endif
		for (i=1;i<n;i++)
		{
			vs = exif_get_short (data, entry->order);
			data+=2;
			switch(i) {
			case 7:
				strncpy (v, _("White balance : "), sizeof (v) - 1 - strlen(v));
				switch (vs) {
                		case 0:
					strncat (v, _("Auto"), sizeof (v) - 1 - strlen(v));
                        		break;
                		case 1:
                        		strncat (v, _("Sunny"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 2:
                        		strncat (v, _("Cloudy"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 3:
                        		strncat (v, _("Tungsten"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 4:
                        		strncat (v, _("Flourescent"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 5:
                        		strncat (v, _("Flash"), sizeof (v) - 1 - strlen(v));
                        		break;
				case 6:
                        		strncat (v, _("Custom"), sizeof (v) - 1 - strlen(v));
                        		break;
				default:
                        		snprintf (buf, sizeof (buf), "%i???", vs);
					strncat (v,buf, sizeof (v) - 1 - strlen(v));
                		}
				break;
			case 9:
				snprintf (buf, sizeof (buf), " / %s : %u", _("Sequence number"), vs);
				strncat (v, buf, sizeof (v) - 1 - strlen(v));

				break;
			case 14:
				if (vs>>12)
				{
					strncat (v, _(" / AF point used : "), sizeof (v) - 1 - strlen(v));
					if (vs&1)
						strncat (v, _("Right"), sizeof (v) - 1 - strlen(v));
                			if ((vs>>1)&1)
                        			strncat (v, _("Center"), sizeof (v) - 1 - strlen(v));
					if ((vs>>2)&1)
                        			strncat (v, _("Left"), sizeof (v) - 1 - strlen(v));
					snprintf (buf, sizeof (buf), " (%u %s)", vs>>12, _("available focus point"));
					strncat (v, buf, sizeof (v) - 1 - strlen(v));
				}
#ifdef DEBUG
                        	printf ("0x%08x\n", vs);
#endif
				break;
			case 15:
				snprintf (buf, sizeof (buf), " / %s : %.2f EV", _("Flash bias"), vs/32.0);
				strncat (v, buf, sizeof (v) - 1 - strlen(v));

				break;
			case 19:
			        snprintf (buf, sizeof (buf), " / %s : %u", _("Subject Distance (mm)"), vs);
				strncat (v,buf, sizeof (v) - 1 - strlen(v));
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
		CF (entry->format, EXIF_FORMAT_ASCII, v);
		CC (entry->components, 32, v);
		strncpy (v,data,MIN (entry->size, sizeof (v) - 1));
		break;

	case MNOTE_CANON_TAG_FIRMWARE:
		CF (entry->format, EXIF_FORMAT_ASCII, v);
		CC2 (entry->components, 24, 32, v);
		strncpy (v,data,MIN (entry->size, sizeof (v) - 1));
		break;

	case MNOTE_CANON_TAG_IMAGE_NUMBER:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 1, v);
		vl = exif_get_long (data, entry->order);
		snprintf (v, sizeof (v), "%03lu-%04lu", vl/10000,vl%10000);
		break;

	case MNOTE_CANON_TAG_SERIAL_NUMBER:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 1, v);
		vl = exif_get_long (data, entry->order);
		snprintf (v, sizeof (v), "%04X-%05d", (int)vl>>16,(int)vl&0xffff);
		break;

	case MNOTE_CANON_TAG_CUSTOM_FUNCS:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		n = exif_get_short (data, entry->order)/2;
		data+=2;
		CC (entry->components, n, v);
#ifdef DEBUG
		printf ("Custom Function size %d %d\n",n,entry->size);
#endif
		for (i=1;i<n;i++)
		{
			vs = exif_get_short (data, entry->order);
			data += 2;
			snprintf (buf, sizeof(buf), "C.F%d : %u", i, vs);
			strncat (v,buf, sizeof (v) - 1 - strlen(v));
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
		    strncpy (v,data,MIN (entry->size, sizeof (v) - 1));
#endif
		break;
        }

        return strdup (v);
}
