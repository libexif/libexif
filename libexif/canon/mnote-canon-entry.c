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

#define UNDEFINED 0xFF
    
static struct  {
  unsigned int subtag;
  ExifShort value;
  char *name;
} entries [] = {
  { 1,  1, N_("macro")},
  { 1,  2, N_("normal")},
  { 4,  0, N_("flash did not fire")},
  { 4,  1, N_("auto")},
  { 4,  2, N_("on")},
  { 4,  3, N_("red eyes reduction")},
  { 4,  4, N_("slow synchro")},
  { 4,  5, N_("auto + red yes reduction")},
  { 4,  6, N_("on + red eyes reduction")},
  { 4, 16, N_("external")},
  { 5,  0, N_("single or timer")},
  { 5,  1, N_("continuous")},
  { 7,  0, N_("one-Shot")},
  { 7,  1, N_("AI Servo")},
  { 7,  2, N_("AI Focus")},
  { 7,  3, N_("MF")},
  { 7,  4, N_("Single")},
  { 7,  5, N_("Continuous")},
  { 7,  6, N_("MF")},
  {10,  0, N_("large")},
  {10,  1, N_("medium")},
  {10,  2, N_("small")},
  {11,  0, N_("full auto")},
	{11,  1, N_("manual")},
	{11,  2, N_("landscape")},
	{11,  3, N_("fast shutter")},
	{11,  4, N_("slow shutter")},
	{11,  5, N_("night")},
	{11,  6, N_("Black & White")},
	{11,  7, N_("Sepia")},
	{11,  8, N_("Portrait")},
	{11,  9, N_("Sports")},
	{11, 10, N_("Macro / Close-Up")},
	{11, 11, N_("Pan Focus")},
	{13, 0xffff, N_("low")},
	{13, 0x0000, N_("normal")},
	{13, 0x0001, N_("high")},
	{14, 0xffff, N_("low")},
	{14, 0x0000, N_("normal")},
	{14, 0x0001, N_("high")},
	{15, 0xffff, N_("low")},
	{15, 0x0000, N_("normal")},
	{15, 0x0001, N_("high")},
	{16, 15, N_("auto")},
	{16, 16, N_("50")},
	{16, 17, N_("100")},
	{16, 18, N_("200")},
	{16, 19, N_("400")},
	{17,  3, N_("evaluative")},
	{17,  4, N_("partial")},
	{17,  5, N_("center-weighted")},
	{19, 0x3000, N_("none (manual focus)")},
	{19, 0x3001, N_("auto-selected")},
	{19, 0x3002, N_("right")},
	{19, 0x3003, N_("center")},
	{19, 0x3004, N_("left")},
	{20,  0, N_("easy shooting")},
	{20,  1, N_("program")},
	{20,  2, N_("Tv-priority")},
	{20,  3, N_("Av-priority")},
	{20,  4, N_("manual")},
	{20,  5, N_("A-DEP")},
	{22, 0x001, N_("Canon EF 50mm f/1.8")},
	{22, 0x002, N_("Canon EF 28mm f/2.8")},
	{22, 0x004, N_("Sigma UC Zoom 35-135mm f/4-5.6")},
	{22, 0x006, N_("Tokina AF193-2 19-35mm f/3.5-4.5")},
	{22, 0x010, N_("Sigma 50mm f/2.8 EX or 28mm f/1.8")},
	{22, 0x011, N_("Canon EF 35mm f/2")},
	{22, 0x013, N_("Canon EF 15mm f/2.8")},
	{22, 0x021, N_("Canon EF 80-200mm f/2.8L")},
	{22, 0x026, N_("Cosina 100mm f/3.5 Macro AF")},
	{22, 0x028, N_("Tamron AF Aspherical 28-200mm f/3.8-5.6")},
	{22, 0x029, N_("Canon EF 50mm f/1.8 MkII")},
	{22, 0x039, N_("Canon EF 75-300mm f/4-5.6")},
	{22, 0x040, N_("Canon EF 28-80mm f/3.5-5.6")},
	{22, 0x124, N_("Canon MP-E 65mm f/2.8 1-5x Macro Photo")},
	{22, 0x125, N_("Canon TS-E 24mm f/3.5L")},
	{22, 0x131, N_("Sigma 17-35mm f2.8-4 EX Aspherical HSM")},
	{22, 0x135, N_("Canon EF 200mm f/1.8L")},
	{22, 0x136, N_("Canon EF 300mm f/2.8L")},
	{22, 0x139, N_("Canon EF 400mm f/2.8L")},
	{22, 0x141, N_("Canon EF 500mm f/4.5L")},
	{22, 0x149, N_("Canon EF 100mm f/2")},
	{22, 0x150, N_("Sigma 20mm EX f/1.8")},
	{22, 0x151, N_("Canon EF 200mm f/2.8L")},
	{22, 0x153, N_("Canon EF 35-350mm f/3.5-5.6L")},
	{22, 0x155, N_("Canon EF 85mm f/1.8 USM")},
	{22, 0x156, N_("Canon EF 28-105mm f/3.5-4.5 USM")},
	{22, 0x160, N_("Canon EF 20-35mm f/3.5-4.5 USM")},
	{22, 0x161, N_("Canon EF 28-70mm f/2.8L or Sigma 24-70mm EX f/2.8")},
	{22, 0x165, N_("Canon EF 70-200mm f/2.8 L")},
	{22, 0x166, N_("Canon EF 70-200mm f/2.8 L + x1.4")},
	{22, 0x167, N_("Canon EF 70-200mm f/2.8 L + x2")},
	{22, 0x169, N_("Sigma 15-30mm f/3.5-4.5 EX DG Aspherical")},
	{22, 0x170, N_("Canon EF 200mm f/2.8L II")},
	{22, 0x173, N_("Canon EF 180mm Macro f/3.5L or Sigma 180mm EX HSM Macro f/3.5")},
	{22, 0x174, N_("Canon EF 135mm f/2L")},
	{22, 0x176, N_("Canon EF 24-85mm f/3.5-4.5 USM")},
	{22, 0x177, N_("Canon EF 300mm f/4L IS")},
	{22, 0x178, N_("Canon EF 28-135mm f/3.5-5.6 IS")},
	{22, 0x180, N_("Canon EF 35mm f/1.4L")},
	{22, 0x182, N_("Canon EF 100-400mm f/4.5-5.6L IS + x2")},
	{22, 0x183, N_("Canon EF 100-400mm f/4.5-5.6L IS")},
	{22, 0x186, N_("Canon EF 70-200mm f/4L")},
	{22, 0x190, N_("Canon EF 100mm f/2.8 Macro")},
	{22, 0x191, N_("Canon EF 400mm f/4 DO IS")},
	{22, 0x197, N_("Canon EF 75-300mm f/4-5.6 IS")},
	{22, 0x198, N_("Canon EF 50mm f/1.4")},
	{22, 0x202, N_("Canon EF 28-80 f/3.5-5.6 USM IV")},
	{22, 0x213, N_("Canon EF 90-300mm f/4.5-5.6")},
	{22, 0x229, N_("Canon EF 16-35mm f/2.8L")},
	{22, 0x230, N_("Canon EF 24-70mm f/2.8L")},
	{22, 0x231, N_("Canon EF 17-40mm f/4L")},
  {32,  0, N_("single")},
  {32,  1, N_("continuous")},
  { 0,  0, NULL}
};

unsigned int
mnote_canon_entry_count_values (const MnoteCanonEntry *entry)
{
	if (!entry) return 0;

	switch (entry->tag) {
	case MNOTE_CANON_TAG_SETTINGS_1:
	case MNOTE_CANON_TAG_SETTINGS_2:
	case MNOTE_CANON_TAG_CUSTOM_FUNCS:
		if (entry->format != EXIF_FORMAT_SHORT) return 0;
		return exif_get_short (entry->data, entry->order) / 2;
	default:
		return 1;
	}
}

char *
mnote_canon_entry_get_value (const MnoteCanonEntry *entry, unsigned int t, char *val, unsigned int maxlen)
{
	char buf[128];
	ExifLong vl;
	ExifShort vs, n;
	unsigned int j;
	unsigned char *data = entry->data;

	if (!entry) return NULL;

	memset (val, 0, maxlen);
	maxlen--;

	switch (entry->tag) {
	case MNOTE_CANON_TAG_SETTINGS_1:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		n = exif_get_short (data, entry->order) / 2;
		if (t >= n) return NULL;
		CC (entry->components, n, val, maxlen);
		vs = exif_get_short (entry->data + 2 + t * 2, entry->order);
		switch (t) {
		case 0:
			break;
		case 2:
			if (!vs) break;
			snprintf (buf, sizeof (buf), _("%i (ms)"), vs * 100);
			strncpy (val, buf, maxlen - strlen (val));
			break;
		case 23:
		case 24:
			snprintf (buf, sizeof (buf), "%u", vs);
			strncpy (val, buf, maxlen - strlen (val));
			break;
		case 25:
			snprintf (buf, sizeof (buf), _("%u mm"), vs);
			strncpy (val, buf, maxlen - strlen(val));
			break;
		case 29:
			if ((vs >> 14) & 1)
				strncpy (val, _("External E-TTL"), maxlen - strlen (val));
			if ((vs >> 13) & 1)
				strncpy (val, _("Internal flash"), maxlen - strlen (val));
			if ((vs >> 11) & 1)
				strncpy (val, _("FP sync used"), maxlen - strlen (val));
			if ((vs >> 4) & 1)
				strncpy (val, _("FP sync enabled"), maxlen - strlen (val));
			break;

		default:
			for (j = 0; entries[j].name && ((entries[j].subtag < t) ||
						((entries[j].subtag == t) && entries[j].value <= vs)); j++)
				if ((entries[j].subtag == t) && (entries[j].value == vs)) break;
			if ((entries[j].subtag == t) &&
					(entries[j].value == vs) && entries[j].name)
				strncpy (val, entries[j]. name, maxlen - strlen (val));
			else {
				snprintf (buf, sizeof (buf), "0x%04x", vs);
				strncpy (val, buf, maxlen - strlen (val));
			}
		}
		break;

	case MNOTE_CANON_TAG_SETTINGS_2:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		n = exif_get_short (data, entry->order) / 2;
		if (t >= n) return NULL;
		CC (entry->components, n, val, maxlen);
		vs = exif_get_short (entry->data + 2 + t * 2, entry->order);
		switch (t) {
		case 7:
			switch (vs) {
			case 0:
				strncpy (val, _("Auto"), maxlen - strlen (val));
				break;
			case 1:
				strncpy (val, _("Sunny"), maxlen - strlen (val));
				break;
			case 2:
				strncpy (val, _("Cloudy"), maxlen - strlen (val));
				break;
			case 3:
				strncpy (val, _("Tungsten"), maxlen - strlen (val));
				break;
			case 4:
				strncpy (val, _("Flourescent"), maxlen - strlen (val));
				break;
			case 5:
				strncpy (val, _("Flash"), maxlen - strlen (val));
				break;
			case 6:
				strncpy (val, _("Custom"), maxlen - strlen (val));
				break;
			default:
				snprintf (buf, sizeof (buf), "%i", vs);
				strncpy (val, buf, maxlen - strlen (val));
			}
			break;
		case 9:
			snprintf (buf, sizeof (buf), "%u", vs);
			strncpy (val, buf, maxlen - strlen (val));
			break;
		case 14:
			if (vs >> 12) {
				if (vs & 1)
					strncpy (val, _("right"), maxlen - strlen (val));
				if ((vs >> 1) & 1)
					strncpy (val, _("center"), maxlen - strlen (val));
				if ((vs >> 2) & 1)
					strncpy (val, _("left"), maxlen - strlen (val));
				if (vs >> 12 == 1)
					snprintf (buf, sizeof (buf), _(" (1 available focus point)"));
				else
					snprintf (buf, sizeof (buf), _(" (%u available focus points)"), vs >> 12);
				strncat (val, buf, maxlen - strlen (val));
			}
			break;
		case 15:
			snprintf (buf, sizeof (buf), _("%.2f EV"), vs / 32.);
			strncpy (val, buf, maxlen - strlen (val));
			break;
		case 19:
			snprintf (buf, sizeof (buf), _("%u mm"), vs);
			strncpy (val, buf, maxlen - strlen (val));
			break;
		default:
			snprintf (buf, sizeof (buf), "0x%04x", vs);
			strncpy (val, buf, maxlen - strlen (val));
			break;
		}
		break;

	case MNOTE_CANON_TAG_IMAGE_TYPE:
	case MNOTE_CANON_TAG_OWNER:
		CF (entry->format, EXIF_FORMAT_ASCII, val, maxlen);
		CC (entry->components, 32, val, maxlen);
		strncpy (val, (char *)data, MIN (entry->size, maxlen));
		break;

	case MNOTE_CANON_TAG_FIRMWARE:
		CF (entry->format, EXIF_FORMAT_ASCII, val, maxlen);
		CC2 (entry->components, 24, 32, val, maxlen);
		strncpy (val, (char *)data, MIN (entry->size, maxlen));
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
		n = exif_get_short (data, entry->order) / 2;
		if (t >= n) return NULL;
		CC (entry->components, n, val, maxlen);
		vs = exif_get_short (data + 2 + 2 * t, entry->order);
		snprintf (buf, sizeof (buf), "%u", vs);
		strncat (val, buf, maxlen - strlen (val));
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
