/* exif-entry.c
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
#include "exif-entry.h"
#include "exif-ifd.h"
#include "exif-utils.h"
#include "i18n.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* #define DEBUG */

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

struct _ExifEntryPrivate
{
	unsigned int ref_count;
};

ExifEntry *
exif_entry_new (void)
{
	ExifEntry *e;

	e = malloc (sizeof (ExifEntry));
	if (!e)
		return (NULL);
	memset (e, 0, sizeof (ExifEntry));
	e->priv = malloc (sizeof (ExifEntryPrivate));
	if (!e->priv) {
		free (e);
		return (NULL);
	}
	memset (e->priv, 0, sizeof (ExifEntryPrivate));
	e->priv->ref_count = 1;

	return (e);
}

void
exif_entry_ref (ExifEntry *e)
{
	if (!e)
		return;

	e->priv->ref_count++;
}

void
exif_entry_unref (ExifEntry *e)
{
	if (!e)
		return;

	e->priv->ref_count--;
	if (!e->priv->ref_count)
		exif_entry_free (e);
}

void
exif_entry_free (ExifEntry *e)
{
	if (!e)
		return;

	if (e->data)
		free (e->data);
	free (e->priv);
	free (e);
}

void
exif_entry_dump (ExifEntry *e, unsigned int indent)
{
	char buf[1024];
	char value[1024];
	unsigned int i;

	for (i = 0; i < 2 * indent; i++)
		buf[i] = ' ';
	buf[i] = '\0';

	if (!e)
		return;

	printf ("%sTag: 0x%x ('%s')\n", buf, e->tag,
		exif_tag_get_name (e->tag));
	printf ("%s  Format: %i ('%s')\n", buf, e->format,
		exif_format_get_name (e->format));
	printf ("%s  Components: %i\n", buf, (int) e->components);
	printf ("%s  Size: %i\n", buf, e->size);
	printf ("%s  Value: %s\n", buf, exif_entry_get_value (e, value, sizeof(value)));
}

#define CF(format,target,v,maxlen)				\
{								\
	if (format != target) {					\
		snprintf (v, maxlen,				\
			_("Invalid format '%s', "		\
			"expected '%s'."),			\
			exif_format_get_name (format),		\
			exif_format_get_name (target));		\
		break;						\
	}							\
}

#define CC(number,target,v,maxlen)					\
{									\
	if (number != target) {						\
		snprintf (v, maxlen,					\
			_("Invalid number of components (%i, "		\
			"expected %i)."), (int) number, (int) target);	\
		break;							\
	}								\
}

const char *
exif_entry_get_value (ExifEntry *e, char *val, unsigned int maxlen)
{
	unsigned int i;
	ExifByte v_byte;
	ExifShort v_short, v_short2, v_short3, v_short4;
	ExifLong v_long;
	ExifSLong v_slong;
	ExifRational v_rat;
	ExifSRational v_srat;
	char b[64];
	const char *c;
	ExifByteOrder o;
	double d;
	ExifEntry *entry;
	static struct {
		char *label;
		char major, minor;
	} versions[] = {
		{"0110", 1,  1},
		{"0120", 1,  2},
		{"0200", 2,  0},
		{"0210", 2,  1},
		{"0220", 2,  2},
		{"0221", 2, 21},
		{NULL  , 0,  0}
	};

	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	bindtextdomain (GETTEXT_PACKAGE, LIBEXIF_LOCALEDIR);

	/* We need the byte order */
	if (!e || !e->parent || !e->parent->parent)
		return (NULL);
	o = exif_data_get_byte_order (e->parent->parent);

	memset (val, 0, maxlen);
	memset (b, 0, sizeof (b));
	maxlen--;

	/* Sanity check */
	if (e->size != e->components * exif_format_get_size (e->format)) {
		snprintf (val, maxlen, _("Invalid size of entry (%i, "
			"expected %li x %i)."), e->size, e->components,
				exif_format_get_size (e->format));
		return val;
	}

	switch (e->tag) {
	case EXIF_TAG_USER_COMMENT:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		if (e->size < 8) break;
		strncpy (val, e->data + 8, MIN (e->size - 8, maxlen));
		break;
	case EXIF_TAG_EXIF_VERSION:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (e->components, 4, val, maxlen);
		strncpy (val, _("Unknown Exif Version"), maxlen);
		for (i = 0; versions[i].label; i++) {
			if (!memcmp (e->data, versions[i].label, 4)) {
    				snprintf (val, maxlen,
					_("Exif Version %d.%d"),
					versions[i].major,
					versions[i].minor);
    				break;
			}
		}
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (e->components, 4, val, maxlen);
		if (!memcmp (e->data, "0100", 4))
			strncpy (val, _("FlashPix Version 1.0"), maxlen);
		else if (!memcmp (e->data, "0101", 4))
			strncpy (val, _("FlashPix Version 1.01"), maxlen);
		else
			strncpy (val, _("Unknown FlashPix Version"), maxlen);
		break;
	case EXIF_TAG_COPYRIGHT:
		CF (e->format, EXIF_FORMAT_ASCII, val, maxlen);

		/*
		 * First part: Photographer.
		 * Some cameras store a string like "   " here. Ignore it.
		 */
		if (e->size && e->data &&
		    (strspn (e->data, " ") != strlen ((char *) e->data)))
			strncpy (val, e->data, MIN (maxlen, e->size));
		else
			strncpy (val, _("[None]"), maxlen);
		strncat (val, " ", maxlen);
		strncat (val, _("(Photographer)"), maxlen);

		/* Second part: Editor. */
		strncat (val, " - ", maxlen);
		if (e->size && e->data &&
		    (strlen ((char *) e->data) + 1 < e->size) &&
		    (strspn (e->data, " ") != strlen ((char *) e->data)))
			strncat (val, e->data + strlen (e->data) + 1, maxlen);
		else
			strncat (val, _("[None]"), maxlen);
		strncat (val, " ", maxlen);
		strncat (val, _("(Editor)"), maxlen);

		break;
	case EXIF_TAG_FNUMBER:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return (NULL);
		snprintf (val, maxlen, "f/%.01f", (float) v_rat.numerator /
						    (float) v_rat.denominator);
		break;
	case EXIF_TAG_APERTURE_VALUE:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return (NULL);
		snprintf (val, maxlen, "f/%.01f",
			  pow (2 , ((float) v_rat.numerator /
				    (float) v_rat.denominator) / 2.));
		break;
	case EXIF_TAG_FOCAL_LENGTH:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return (NULL);

		/*
		 * For calculation of the 35mm equivalent,
		 * Minolta cameras need a multiplier that depends on the
		 * camera model.
		 */
		d = 0.;
		entry = exif_content_get_entry (
			e->parent->parent->ifd[EXIF_IFD_0], EXIF_TAG_MAKE);
		if (entry && entry->data &&
		    !strncmp (entry->data, "Minolta", 7)) {
			entry = exif_content_get_entry (
					e->parent->parent->ifd[EXIF_IFD_0],
					EXIF_TAG_MODEL);
			if (entry && entry->data) {
				if (!strncmp (entry->data, "DiMAGE 7", 8))
					d = 3.9;
				else if (!strncmp (entry->data, "DiMAGE 5", 8))
					d = 4.9;
			}
		}
		if (d)
			snprintf (b, sizeof (b), _(" (35 equivalent: %d mm)"),
				  (int) (d * (double) v_rat.numerator /
				  	     (double) v_rat.denominator));

		snprintf (val, maxlen, "%.1f mm%s",
			  (float) v_rat.numerator / (float) v_rat.denominator,
			  b);
		break;
	case EXIF_TAG_SUBJECT_DISTANCE:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return (NULL);
		snprintf (val, maxlen, "%.1f m", (float) v_rat.numerator /
						   (float) v_rat.denominator);
		break;
	case EXIF_TAG_EXPOSURE_TIME:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return (NULL);
		d = (double) v_rat.numerator / (double) v_rat.denominator;
		if (d < 1)
			snprintf (val, maxlen, _("1/%d sec."),
				  (int) (1. / d));
		else
			snprintf (val, maxlen, _("%d sec."), (int) d);
		break;
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator) return (NULL);
		snprintf (val, maxlen, "%.0f/%.0f sec. (APEX: %i)",
              (float) v_srat.numerator, (float) v_srat.denominator,
			  (int) pow (sqrt(2), (float) v_srat.numerator /
					      (float) v_srat.denominator));
		break;
	case EXIF_TAG_BRIGHTNESS_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		snprintf (val, maxlen, "%i/%i", (int) v_srat.numerator,
						  (int) v_srat.denominator);
		/* FIXME: How do I calculate the APEX value? */
		break;
	case EXIF_TAG_METERING_MODE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0: strncpy (val, _("Unknown"), maxlen); break;
		case 1: strncpy (val, _("Average"), maxlen); break;
		case 2: strncpy (val, _("Center-Weighted Average"), maxlen); break;
		case 3: strncpy (val, _("Spot"), maxlen); break;
		case 4: strncpy (val, _("Multi Spot"), maxlen); break;
		case 5: strncpy (val, _("Pattern"), maxlen); break;
		case 6: strncpy (val, _("Partial"), maxlen); break;
		case 255: strncpy (val, _("Other"), maxlen); break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_COMPRESSION:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1: strncpy (val, _("Uncompressed"), maxlen); break;
		case 6: strncpy (val, _("JPEG compression"), maxlen); break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_FILE_SOURCE:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (e->components, 1, val, maxlen);
		switch (e->data[0]) {
		case 0x03: strncpy (val, _("DSC"), maxlen); break;
		default: snprintf (val, maxlen, "0x%02x", e->data[0]); break;
		}
		break;
	case EXIF_TAG_PLANAR_CONFIGURATION:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1: strncpy (val, _("chunky format"), maxlen); break;
		case 2: strncpy (val, _("planar format"), maxlen); break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_COMPONENTS_CONFIGURATION:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (e->components, 4, val, maxlen);
		for (i = 0; i < 4; i++) {
			switch (e->data[i]) {
			case 0: c = _("-"); break;
			case 1: c = _("Y"); break;
			case 2: c = _("Cb"); break;
			case 3: c = _("Cr"); break;
			case 4: c = _("R"); break;
			case 5: c = _("G"); break;
			case 6: c = _("B"); break;
			default: c = _("reserved"); break;
			}
			strncat (val, c, maxlen);
			if (i < 3) strncat (val, " ", maxlen);
		}
		break;
	case EXIF_TAG_SENSING_METHOD:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1: strncpy (val, _("Not defined"), maxlen); break;
		case 2:
			strncpy (val, _("One-chip color area sensor"), maxlen);
			break;
		case 3:
			strncpy (val, _("Two-chip color area sensor"), maxlen);
			break;
		case 4:
			strncpy (val, _("Three-chip color area sensor"), maxlen);
			break;
		case 5:
			strncpy (val, _("Color sequential area sensor"), maxlen);
			break;
		case 7: strncpy (val, _("Trilinear sensor"), maxlen); break;
		case 8:
			strncpy (val, _("Color sequential linear sensor"), maxlen);
			break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0: strncpy (val, _("Unknown"), maxlen); break;
		case 1: strncpy (val, _("Daylight"), maxlen); break;
		case 2: strncpy (val, _("Fluorescent"), maxlen); break;
		case 3:
			strncpy (val, _("Tungsten (incandescent light)"), maxlen);
			break;
		case 4: strncpy (val, _("Flash"), maxlen); break;
		case 9: strncpy (val, _("Fine weather"), maxlen); break;
		case 10: strncpy (val, _("Cloudy weather"), maxlen); break;
		case 11: strncpy (val, _("Shade"), maxlen); break;
		case 12:
			 strncpy (val, _("Daylight fluorescent"), maxlen);
			 break;
		case 13:
			strncpy (val, _("Day white fluorescent"), maxlen);
			break;
		case 14:
			strncpy (val, _("Cool white fluorescent"), maxlen);
			break;
		case 15:
			strncpy (val, _("White fluorescent"), maxlen);
			break;
		case 17:
			strncpy (val, _("Standard light A"), maxlen); break;
		case 18: strncpy (val, _("Standard light B"), maxlen); break;
		case 19: strncpy (val, _("Standard light C"), maxlen); break;
		case 20: strncpy (val, _("D55"), maxlen); break;
		case 21: strncpy (val, _("D65"), maxlen); break;
		case 22: strncpy (val, _("D75"), maxlen); break;
		case 24:
			strncpy (val, _("ISO studio tungsten"), maxlen);
			break;
		case 255: strncpy (val, _("Other"), maxlen); break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
	case EXIF_TAG_RESOLUTION_UNIT:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 2: strncpy (val, _("Inch"), maxlen); break;
		case 3: strncpy (val, _("Centimeter"), maxlen); break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0: strncpy (val, _("Not defined"), maxlen); break;
		case 1: strncpy (val, _("Manual"), maxlen); break;
		case 2: strncpy (val, _("Normal program"), maxlen); break;
		case 3: strncpy (val, _("Aperture priority"), maxlen); break;
		case 4: strncpy (val, _("Shutter priority"), maxlen); break;
		case 5:
			strncpy (val, _("Creative program (biased toward "
				      "depth of field)"), maxlen);
			break;
		case 6:
			strncpy (val, _("Action program (biased toward "
				      "fast shutter speed)"), maxlen);
			break;
		case 7:
			strncpy (val, _("Portrait mode (for closeup photos "
				      "with the background out of focus"),
				 maxlen);
			break;
		case 8:
			strncpy (val, _("Landscape mode (for landscape "
				      "photos with the background in focus"),
				 maxlen);
			break;
		default: snprintf (val, maxlen, "%i", v_short); break;
		}
		break;
	case EXIF_TAG_EXPOSURE_BIAS_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator)
			return (NULL);
		snprintf (val, maxlen, "%s%.01f",
			  v_srat.denominator * v_srat.numerator > 0 ? "+" : "",
			  (double) v_srat.numerator /
			  (double) v_srat.denominator);
		break;
	case EXIF_TAG_ORIENTATION:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (val, _("top - left"), maxlen);
			break;
		case 2:
			strncpy (val, _("top - right"), maxlen);
			break;
		case 3:
			strncpy (val, _("bottom - right"), maxlen);
			break;
		case 4:
			strncpy (val, _("bottom - left"), maxlen);
			break;
		case 5:
			strncpy (val, _("left - top"), maxlen);
			break;
		case 6:
			strncpy (val, _("right - top"), maxlen);
			break;
		case 7:
			strncpy (val, _("right - bottom"), maxlen);
			break;
		case 8:
			strncpy (val, _("left - bottom"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_YCBCR_POSITIONING:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (val, _("centered"), maxlen);
			break;
		case 2:
			strncpy (val, _("co-sited"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_YCBCR_SUB_SAMPLING:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 2, val, maxlen);
		v_short  = exif_get_short (e->data, o);
		v_short2 = exif_get_short (
			e->data + exif_format_get_size (e->format),
			o);
		if ((v_short == 2) && (v_short2 == 1))
			strncpy (val, _("YCbCr4:2:2"), maxlen);
		else if ((v_short == 2) && (v_short2 == 2))
			strncpy (val, _("YCbCr4:2:0"), maxlen);
		else
			snprintf (val, maxlen, "%i, %i", v_short, v_short2);
		break;
	case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short  = exif_get_short (e->data, o);
		switch (v_short) {
		case 2:
			strncpy (val, _("RGB"), maxlen);
			break;
		case 6:
			strncpy (val, _("YCbCr"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_COLOR_SPACE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (val, _("sRGB"), maxlen);
			break;
		case 0xffff:
			strncpy (val, _("Uncalibrated"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_FLASH:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0x000:
			strncpy (val, _("Flash did not fire."), maxlen);
			break;
		case 0x0001:
			strncpy (val, _("Flash fired."), maxlen);
			break;
		case 0x0005:
			strncpy (val, _("Strobe return light not detected."), maxlen);
			break;
		case 0x0007:
			strncpy (val, _("Strobe return light detected."), maxlen);
			break;
		case 0x000d:
			strncpy (val, _("Flash fired, compulsory flash mode, "
				"return light not detected."), maxlen);
			break;
		case 0x000f:
			strncpy (val, _("Flash fired, compulsory flash mode, "
				"return light detected."), maxlen);
			break;
		case 0x0010:
			strncpy (val, _("Flash did not fire, compulsory flash "
				"mode."), maxlen);
			break;
		case 0x0018:
			strncpy (val, _("Flash did not fire, auto mode."), maxlen);
			break;
		case 0x0019:
			strncpy (val, _("Flash fired, auto mode."), maxlen);
			break;
		case 0x001d:
			strncpy (val, _("Flash fired, auto mode, return light "
				"not detected."), maxlen);
			break;
		case 0x001f:
			strncpy (val, _("Flash fired, auto mode, return light "
				"detected."), maxlen);
			break;
		case 0x0020:
			strncpy (val, _("No flash function."), maxlen);
			break;
		case 0x0041:
			strncpy (val, _("Flash fired, red-eye reduction mode."), maxlen);
			break;
		case 0x0045:
			strncpy (val, _("Flash fired, red-eye reduction mode, "
				"return light not detected."), maxlen);
			break;
		case 0x0047:
			strncpy (val, _("Flash fired, red-eye reduction mode, "
				"return light detected."), maxlen);
			break;
		case 0x0049:
			strncpy (val, _("Flash fired, compulsory flash mode, "
				"red-eye reduction mode."), maxlen);
			break;
		case 0x004d:
			strncpy (val, _("Flash fired, compulsory flash mode, "
				"red-eye reduction mode, return light not "
				"detected."), maxlen);
			break;
		case 0x004f:
			strncpy (val, _("Flash fired, compulsory flash mode, "
				"red-eye reduction mode, return light "
				"detected."), maxlen);
			break;
		case 0x0059:
			strncpy (val, _("Flash fired, auto mode, red-eye "
				"reduction mode."), maxlen);
			break;
		case 0x005d:
			strncpy (val, _("Flash fired, auto mode, return light "
				"not detected, red-eye reduction mode."), maxlen);
			break;
		case 0x005f:
			strncpy (val, _("Flash fired, auto mode, return light "
				"detected, red-eye reduction mode."), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_MAKER_NOTE:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		snprintf (val, maxlen, _("%i bytes unknown data"),
			  (int) e->components);
		break;
	case EXIF_TAG_CUSTOM_RENDERED:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Normal process"), maxlen);
			break;
		case 1:
			strncpy (val, _("Custom process"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_EXPOSURE_MODE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Auto exposure"), maxlen);
			break;
		case 1:
			strncpy (val, _("Manual exposure"), maxlen);
			break;
		case 2:
			strncpy (val, _("Auto bracket"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_WHITE_BALANCE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Auto white balance"), maxlen);
			break;
		case 1:
			strncpy (val, _("Manual white balance"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SCENE_CAPTURE_TYPE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Standard"), maxlen);
			break;
		case 1:
			strncpy (val, _("Landscape"), maxlen);
			break;
		case 2:
			strncpy (val, _("Portrait"), maxlen);
			break;
		case 3:
			strncpy (val, _("Night scene"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_GAIN_CONTROL:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("Low gain up"), maxlen);
			break;
		case 2:
			strncpy (val, _("High gain up"), maxlen);
			break;
		case 3:
			strncpy (val, _("Low gain down"), maxlen);
			break;
		case 4:
			strncpy (val, _("High gain down"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SATURATION:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Normal"), maxlen);
			break;
		case 1:
			strncpy (val, _("Low saturation"), maxlen);
			break;
		case 2:
			strncpy (val, _("High saturation"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_CONTRAST:
	case EXIF_TAG_SHARPNESS:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
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
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SUBJECT_DISTANCE_RANGE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("Unknown"), maxlen);
			break;
		case 1:
			strncpy (val, _("Macro"), maxlen);
			break;
		case 2:
			strncpy (val, _("Close view"), maxlen);
			break;
		case 3:
			strncpy (val, _("Distant view"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SUBJECT_AREA:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		switch (e->components) {
		case 2:
			v_short  = exif_get_short (e->data, o);
			v_short2 = exif_get_short (e->data + 2, o);
			snprintf (val, maxlen, "(x,y) = (%i,%i)",
				  v_short, v_short2);
			break;
		case 3:
			v_short  = exif_get_short (e->data, o);
			v_short2 = exif_get_short (e->data + 2, o);
			v_short3 = exif_get_short (e->data + 4, o);
			snprintf (val, maxlen, _("Within distance %i of "
				"(x,y) = (%i,%i)"), v_short3, v_short,
				v_short2);
			break;
		case 4:
			v_short  = exif_get_short (e->data, o);
			v_short2 = exif_get_short (e->data + 2, o);
			v_short3 = exif_get_short (e->data + 4, o);
			v_short4 = exif_get_short (e->data + 6, o);
			snprintf (val, maxlen, _("Within rectangle "
				"(width %i, height %i) around "
				"(x,y) = (%i,%i)"), v_short3, v_short4,
				v_short, v_short2);
			break;
		default:
			snprintf (val, maxlen, _("Unexpected number "
				"of components (%li, expected 2, 3, or 4)."),
				e->components);
		}
	default:
		if (!e->components) break;
		switch (e->format) {
		case EXIF_FORMAT_UNDEFINED:
			break;
		case EXIF_FORMAT_BYTE:
		case EXIF_FORMAT_SBYTE:
			v_byte = e->data[0];
			snprintf (val, maxlen, "0x%02x", v_byte);
			maxlen -= strlen(val);
			for (i = 1; i < e->components; i++) {
				v_byte = e->data[i];
				snprintf (b, sizeof (b), ", 0x%02x", v_byte);
				strncat (val, b, maxlen);
				maxlen -= strlen(b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_SHORT:
		case EXIF_FORMAT_SSHORT:
			v_short = exif_get_short (e->data, o);
			snprintf (val, maxlen, "%i", v_short);
			maxlen -= strlen(val);
			for (i = 1; i < e->components; i++) {
				v_short = exif_get_short (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), ", %i", v_short);
				strncat (val, b, maxlen);
				maxlen -= strlen(b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_LONG:
			v_long = exif_get_long (e->data, o);
			snprintf (val, maxlen, "%i", (int) v_long);
			maxlen -= strlen(val);
			for (i = 1; i < e->components; i++) {
				v_long = exif_get_long (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), ", %li", v_long);
				strncat (val, b, maxlen);
				maxlen -= strlen(b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_SLONG:
			v_slong = exif_get_slong (e->data, o);
			snprintf (val, maxlen, "%li", v_slong);
			maxlen -= strlen(val);
			for (i = 1; i < e->components; i++) {
				v_long = exif_get_slong (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), ", %li", v_long);
				strncat (val, b, maxlen);
				maxlen -= strlen(b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_ASCII:
			strncpy (val, e->data, MIN (maxlen, e->size));
			break;
		case EXIF_FORMAT_RATIONAL:
			v_rat = exif_get_rational (e->data, o);
			snprintf (val, maxlen, "%i/%i",
				  (int) v_rat.numerator,
				  (int) v_rat.denominator);
			maxlen -= strlen(val);
			for (i = 1; i < e->components; i++) {
				v_rat = exif_get_rational (
					e->data + 8 * i, o);
				snprintf (b, sizeof (b), ", %i/%i",
					  (int) v_rat.numerator,
					  (int) v_rat.denominator);
				strncat (val, b, maxlen);
				maxlen -= strlen(b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_SRATIONAL:
			v_srat = exif_get_srational (e->data, o);
			snprintf (val, maxlen, "%i/%i",
				  (int) v_srat.numerator,
				  (int) v_srat.denominator);
			maxlen -= strlen(val);
			for (i = 1; i < e->components; i++) {
				v_srat = exif_get_srational (
					e->data + 8 * i, o);
				snprintf (b, sizeof (b), ", %i/%i",
					  (int) v_srat.numerator,
					  (int) v_srat.denominator);
				strncat (val, b, maxlen);
				maxlen -= strlen(b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_DOUBLE:
		case EXIF_FORMAT_FLOAT:
		default:
			/* What to do here? */
			break;
		}
	}

	return (val);
}

const char *
exif_entry_get_value_brief (ExifEntry *e, char *val, unsigned int maxlen)
{
	ExifByteOrder o;
	ExifEntry *entry;
	ExifRational v_rat;
	ExifSRational v_srat;
	ExifShort v_short;
	double d;

	/* We need the byte order */
	if (!e || !e->parent || !e->parent->parent)
		return (NULL);
	o = exif_data_get_byte_order (e->parent->parent);

	memset (val, 0, maxlen);
    maxlen--;

	switch (e->tag) {
	case EXIF_TAG_FOCAL_LENGTH:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);

		/*
		 * For calculation of the 35mm equivalent,
		 * Minolta cameras need a multiplier that depends on the
		 * camera model.
		 */
		d = (double) v_rat.numerator / (double) v_rat.denominator;
		entry = exif_content_get_entry (
			e->parent->parent->ifd[EXIF_IFD_0], EXIF_TAG_MAKE);
		if (entry && entry->data &&
		    !strncmp (entry->data, "Minolta", 7)) {
			entry = exif_content_get_entry (
					e->parent->parent->ifd[EXIF_IFD_0],
					EXIF_TAG_MODEL);
			if (entry && entry->data) {
				if (!strncmp (entry->data, "DiMAGE 7", 8))
					d *= 3.9;
				else if (!strncmp (entry->data, "DiMAGE 5", 8))
					d *= 4.9;
			}
		}
		snprintf (val, maxlen, "%dmm",
				(int) (d * (double) v_rat.numerator /
				           (double) v_rat.denominator));
		break;
	case EXIF_TAG_EXPOSURE_TIME:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);
		d = (double) v_rat.numerator / (double) v_rat.denominator;
		if (d < 1.)
			snprintf (val, maxlen, "1/%d", (int) (1. / d));
		else
			snprintf (val, maxlen, "%d", (int) d);
		break;
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator)
			return (NULL);
		d = 1. / pow (2., (double) v_srat.numerator /
				  (double) v_srat.denominator);
		if (d < 1.)
			snprintf (val, maxlen, "1/%d", (int) (1. / d));
		else
			snprintf (val, maxlen, "%d", (int) d);
		break;
	case EXIF_TAG_METERING_MODE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("unknown"), maxlen);
			break;
		case 1:
			strncpy (val, _("average"), maxlen);
			break;
		case 2:
			strncpy (val, _("center-weight"), maxlen);
			break;
		case 3:
			strncpy (val, _("spot"), maxlen);
			break;
		case 4:
			strncpy (val, _("multi-spot"), maxlen);
			break;
		case 5:
			strncpy (val, _("matrix"), maxlen);
			break;
		case 6:
			strncpy (val, _("partial"), maxlen);
			break;
		case 255:
			strncpy (val, _("other"), maxlen);
			break;
		default:
			snprintf (val, maxlen, "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (val, _("sunny"), maxlen);
			break;
		case 1:
			strncpy (val, _("fluorescent"), maxlen);
			break;
		case 2:
			strncpy (val, _("tungsten"), maxlen);
			break;
		case 3:
			strncpy (val, _("cloudy"), maxlen);
			break;
		default:
			return (exif_entry_get_value (e, val, maxlen));
		}
		break;
	case EXIF_TAG_RESOLUTION_UNIT:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 2:
			strncpy (val, _("in"), maxlen);
			break;
		case 3:
			strncpy (val, _("cm"), maxlen);
			break;
		default:
			return (NULL);
		}
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (val, _("manual"), maxlen);
			break;
		case 2:
			strncpy (val, _("normal"), maxlen);
			break;
		case 3:
			strncpy (val, _("aperture"), maxlen);
			break;
		case 4:
			strncpy (val, _("shutter"), maxlen);
			break;
		case 5:
			strncpy (val, _("creative"), maxlen);
			break;
		case 6:
			strncpy (val, _("action"), maxlen);
			break;
		case 7:
			strncpy (val, _("portrait"), maxlen);
			break;
		case 8:
			strncpy (val, _("landscape"), maxlen);
			break;
		default:
			return (exif_entry_get_value (e, val, maxlen));
		}
		break;
	case EXIF_TAG_FLASH:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0x0001:
			strncpy (val, _("yes"), maxlen);
			break;
		case 0x0005:
			strncpy (val, _("w/o strobe"), maxlen);
			break;
		case 0x0007:
			strncpy (val, _("w. strobe"), maxlen);
			break;
		default:
			return (exif_entry_get_value (e, val, maxlen));
		}
		break;
	default:
		return (exif_entry_get_value (e, val, maxlen));
	}

	return (val);
}

void
exif_entry_initialize (ExifEntry *e, ExifTag tag)
{
	time_t t;
	struct tm *tm;
	ExifRational r;
	ExifByteOrder o;

	/* We need the byte order */
	if (!e || !e->parent || e->data || !e->parent->parent)
		return;
	o = exif_data_get_byte_order (e->parent->parent);

	e->tag = tag;
	switch (tag) {

	/* LONG, 1 component, no default */
	case EXIF_TAG_EXIF_IFD_POINTER:
	case EXIF_TAG_GPS_INFO_IFD_POINTER:
	case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
	case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
	case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
		e->components = 1;
		e->format = EXIF_FORMAT_LONG;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		memset (e->data, 0, e->size);
		break;

	/* SHORT, 1 component, no default */
	case EXIF_TAG_SUBJECT_LOCATION:
	case EXIF_TAG_SENSING_METHOD:
	case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
	case EXIF_TAG_COMPRESSION:
	case EXIF_TAG_EXPOSURE_MODE:
	case EXIF_TAG_WHITE_BALANCE:
	case EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM:
	case EXIF_TAG_GAIN_CONTROL:
	case EXIF_TAG_SUBJECT_DISTANCE_RANGE:

	/* SHORT, 1 component, default 0 */
	case EXIF_TAG_IMAGE_WIDTH:
	case EXIF_TAG_IMAGE_LENGTH:
	case EXIF_TAG_EXPOSURE_PROGRAM:
	case EXIF_TAG_LIGHT_SOURCE:
	case EXIF_TAG_METERING_MODE:
	case EXIF_TAG_CUSTOM_RENDERED:
	case EXIF_TAG_SCENE_CAPTURE_TYPE:
	case EXIF_TAG_CONTRAST:
	case EXIF_TAG_SATURATION:
	case EXIF_TAG_SHARPNESS:
		e->components = 1;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		exif_set_short (e->data, o, 0);
		break;

	/* SHORT, 1 component, default 1 */
        case EXIF_TAG_ORIENTATION:
        case EXIF_TAG_PLANAR_CONFIGURATION:
        case EXIF_TAG_YCBCR_POSITIONING:
                e->components = 1;
                e->format = EXIF_FORMAT_SHORT;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = malloc (e->size);
                exif_set_short (e->data, o, 1);
                break;

	/* SHORT, 1 component, default 2 */
        case EXIF_TAG_RESOLUTION_UNIT:
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
                e->components = 1;
                e->format = EXIF_FORMAT_SHORT;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = malloc (e->size);
                exif_set_short (e->data, o, 2);
                break;

	/* SHORT, 1 component, default 3 */
        case EXIF_TAG_SAMPLES_PER_PIXEL:
                e->components = 1;
                e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
                e->data = malloc (e->size);
                exif_set_short (e->data, o, 3);
                break;

	case EXIF_TAG_BITS_PER_SAMPLE:
		e->components = 3;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		exif_set_short (e->data, o, 8);
		exif_set_short (
			e->data + exif_format_get_size (e->format),
			o, 8);
		exif_set_short (
			e->data + 2 * exif_format_get_size (e->format),
			o, 8);
		break;
	case EXIF_TAG_YCBCR_SUB_SAMPLING:
		e->components = 2;
		e->format = EXIF_FORMAT_SHORT;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		exif_set_short (e->data, o, 2);
		exif_set_short (
			e->data + exif_format_get_size (e->format),
			o, 1);
		break;

	/* SRATIONAL, 1 component, no default */
	case EXIF_TAG_EXPOSURE_BIAS_VALUE:
	case EXIF_TAG_BRIGHTNESS_VALUE:
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		e->components = 1;
		e->format = EXIF_FORMAT_SRATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		memset (e->data, 0, e->size);
		break;

	/* RATIONAL, 1 component, no default */
	case EXIF_TAG_EXPOSURE_TIME:
	case EXIF_TAG_FOCAL_PLANE_X_RESOLUTION:
	case EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION:
	case EXIF_TAG_EXPOSURE_INDEX:
	case EXIF_TAG_FLASH_ENERGY:
	case EXIF_TAG_FNUMBER:
	case EXIF_TAG_FOCAL_LENGTH:
	case EXIF_TAG_SUBJECT_DISTANCE:
	case EXIF_TAG_MAX_APERTURE_VALUE:
	case EXIF_TAG_APERTURE_VALUE:
	case EXIF_TAG_COMPRESSED_BITS_PER_PIXEL:
	case EXIF_TAG_PRIMARY_CHROMATICITIES:
	case EXIF_TAG_DIGITAL_ZOOM_RATIO:
		e->components = 1;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		memset (e->data, 0, e->size);
		break;

	/* RATIONAL, 1 component, default 72/1 */
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_Y_RESOLUTION:
		e->components = 1;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		r.numerator = 72;
		r.denominator = 1;
		exif_set_rational (e->data, o, r);
		break;

	/* RATIONAL, 2 components, no default */
	case EXIF_TAG_WHITE_POINT:
		e->components = 2;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		memset (e->data, 0, e->size);
		break;

	/* RATIONAL, 6 components */
	case EXIF_TAG_REFERENCE_BLACK_WHITE:
		e->components = 6;
		e->format = EXIF_FORMAT_RATIONAL;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		r.denominator = 1;
		r.numerator = 0;
		exif_set_rational (e->data, o, r);
		r.numerator = 255;
		exif_set_rational (
			e->data + exif_format_get_size (e->format), o, r);
		r.numerator = 0;
		exif_set_rational (
			e->data + 2 * exif_format_get_size (e->format), o, r);
		r.numerator = 255;
		exif_set_rational (
			e->data + 3 * exif_format_get_size (e->format), o, r);
		r.numerator = 0;
		exif_set_rational (
			e->data + 4 * exif_format_get_size (e->format), o, r);
		r.numerator = 255;
		exif_set_rational (
			e->data + 5 * exif_format_get_size (e->format), o, r);
		break;

	/* ASCII, 20 components */
	case EXIF_TAG_DATE_TIME:
	case EXIF_TAG_DATE_TIME_ORIGINAL:
	case EXIF_TAG_DATE_TIME_DIGITIZED:
		t = time (NULL);
		tm = localtime (&t);
		e->components = 20;
		e->format = EXIF_FORMAT_ASCII;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		snprintf ((char *) e->data, e->size,
			  "%04i:%02i:%02i %02i:%02i:%02i",
			  tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
			  tm->tm_hour, tm->tm_min, tm->tm_sec);
		break;

	/* ASCII, no default */
	case EXIF_TAG_SUB_SEC_TIME:
	case EXIF_TAG_SUB_SEC_TIME_ORIGINAL:
	case EXIF_TAG_SUB_SEC_TIME_DIGITIZED:
		e->components = 0;
		e->format = EXIF_FORMAT_ASCII;
		e->size = 0;
		e->data = NULL;
		break;
	case EXIF_TAG_IMAGE_DESCRIPTION:
	case EXIF_TAG_MAKE:
	case EXIF_TAG_MODEL:
	case EXIF_TAG_SOFTWARE:
	case EXIF_TAG_ARTIST:
		e->components = strlen ("[None]") + 1;
		e->format = EXIF_FORMAT_ASCII;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		strncpy (e->data, "[None]", e->size);
		break;
	case EXIF_TAG_COPYRIGHT:
		e->components = (strlen ("[None]") + 1) * 2;
		e->format = EXIF_FORMAT_ASCII;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		strcpy (e->data +                     0, "[None]");
		strcpy (e->data + strlen ("[None]") + 1, "[None]");
		break;

	/* UNDEFINED, no components, no default */
	case EXIF_TAG_MAKER_NOTE:
	case EXIF_TAG_USER_COMMENT:
		e->components = 0;
		e->format = EXIF_FORMAT_UNDEFINED;
		e->size = 0;
		e->data = 0;
		break;

	/* UNDEFINED, 1 component, default 1 */
	case EXIF_TAG_SCENE_TYPE:
		e->components = 1;
		e->format = EXIF_FORMAT_UNDEFINED;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		e->data[0] = 0x01;
		break;

	/* UNDEFINED, 1 component, default 3 */
	case EXIF_TAG_FILE_SOURCE:
		e->components = 1;
		e->format = EXIF_FORMAT_UNDEFINED;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		e->data[0] = 0x03;
		break;

	/* UNDEFINED, 4 components, default 0 1 0 0 */
        case EXIF_TAG_FLASH_PIX_VERSION:
                e->components = 4;
                e->format = EXIF_FORMAT_UNDEFINED;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = malloc (e->size);
                memcpy (e->data, "0100", 4);
                break;

        /* UNDEFINED, 4 components, default 0 2 1 0 */
        case EXIF_TAG_EXIF_VERSION:
                e->components = 4;
                e->format = EXIF_FORMAT_UNDEFINED;
                e->size = exif_format_get_size (e->format) * e->components;
                e->data = malloc (e->size);
                memcpy (e->data, "0210", 4);
                break;

	default:
		break;
	}
}
