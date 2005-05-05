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
#include <libexif/exif-entry.h>


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

static struct {
	ExifTag tag;
	struct {
		int index;
		const char *string;
	} elem[7];
} items[] = {
  { MNOTE_PENTAX_TAG_MODE,
    { {0, N_("Auto")},
      {1, N_("Night-scene")},
      {2, N_("Manual")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_QUALITY,
    { {0, N_("Good")},
      {1, N_("Better")},
      {2, N_("Best")},{0,NULL}}},
  { MNOTE_PENTAX_TAG_FOCUS,
    { {2, N_("Custom")},
      {3, N_("Auto")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_FLASH,
    { {1, N_("Auto")},
      {2, N_("Flash on")},
      {4, N_("Flash off")},
      {6, N_("Red-eye Reduction")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_WHITE_BALANCE,
    { {0, N_("Auto")},
      {1, N_("Daylight")},
      {2, N_("Shade")},
      {3, N_("Tungsten")},
      {4, N_("Fluorescent")},
      {5, N_("Manual")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_SHARPNESS,
    { {0, N_("Normal")},
      {1, N_("Soft")},
      {2, N_("Hard")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_CONTRAST,
    { {0, N_("Normal")},
      {1, N_("Low")},
      {2, N_("High")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_SATURATION,
    { {0, N_("Normal")},
      {1, N_("Low")},
      {2, N_("High")},
      {0, NULL}}},
  { MNOTE_PENTAX_TAG_ISO_SPEED,
    { {10,  N_("100")},
      {16,  N_("200")},
      {100, N_("100")},
      {200, N_("200")},
      { 0,  NULL}}},
  { MNOTE_PENTAX_TAG_COLOR,
    { {1, N_("Full")},
      {2, N_("Black & White")},
      {3, N_("Sepia")},
      {0, NULL}}},
};

char *
mnote_pentax_entry_get_value (MnotePentaxEntry *entry,
			      char *val, unsigned int maxlen)
{
	ExifLong vl;
	ExifShort vs;
	int i = 0, j = 0;

	if (!entry) return (NULL);

	memset (val, 0, maxlen);
	maxlen--;

	switch (entry->tag) {
	  case MNOTE_PENTAX_TAG_MODE:
	  case MNOTE_PENTAX_TAG_QUALITY:
	  case MNOTE_PENTAX_TAG_FOCUS:
	  case MNOTE_PENTAX_TAG_FLASH:
	  case MNOTE_PENTAX_TAG_WHITE_BALANCE:
	  case MNOTE_PENTAX_TAG_SHARPNESS:
	  case MNOTE_PENTAX_TAG_CONTRAST:
	  case MNOTE_PENTAX_TAG_SATURATION:
	  case MNOTE_PENTAX_TAG_ISO_SPEED:
	  case MNOTE_PENTAX_TAG_COLOR:
		CF (entry->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vs = exif_get_short (entry->data, entry->order);

		/* search the tag */
		for (i = 0; (items[i].tag && items[i].tag != entry->tag); i++);
		if (!items[i].tag) {
		  	strncpy (val, "Internal error", maxlen);
		  	break;
		}

		/* find the value */
		for (j = 0; items[i].elem[j].string &&
			    (items[i].elem[j].index < vs); j++);
		if (items[i].elem[j].index != vs) {
			snprintf (val, maxlen,
				  "Internal error (unknown value %i)", vs);
			break;
		}
		snprintf (val, maxlen, "%s", items[i].elem[j].string);
		break;

	case MNOTE_PENTAX_TAG_ZOOM:
		CF (entry->format, EXIF_FORMAT_LONG, val, maxlen);
		CC (entry->components, 1, val, maxlen);
		vl = exif_get_long (entry->data, entry->order);
		snprintf (val, maxlen, "%li", (long int) vl);
		break;
	case MNOTE_PENTAX_TAG_PRINTIM:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (entry->components, 124, val, maxlen);
		snprintf (val, maxlen, "%li bytes unknown data",
			  entry->components);
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
	default:
		switch (entry->format) {
		case EXIF_FORMAT_ASCII:
		  strncpy (val, (char *)entry->data, MIN(maxlen, entry->components));
		  break;
		case EXIF_FORMAT_SHORT:
		  vs = exif_get_short (entry->data, entry->order);
		  snprintf (val, maxlen, "%i", vs);
		  break;
		case EXIF_FORMAT_LONG:
		  vl = exif_get_long (entry->data, entry->order);
		  snprintf (val, maxlen, "%li", (long int) vl);
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
