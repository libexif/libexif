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

//#define DEBUG

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

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
	printf ("%s  Value: %s\n", buf, exif_entry_get_value (e));
}

#define CF(format,target,v)					\
{								\
	if (format != target) {					\
		snprintf (v, sizeof (v),			\
			_("Invalid format '%s', "		\
			"expected '%s'."),			\
			exif_format_get_name (format),		\
			exif_format_get_name (target));		\
		break;						\
	}							\
}

#define CC(number,target,v)						\
{									\
	if (number != target) {						\
		snprintf (v, sizeof (v),				\
			_("Invalid number of components (%i, "		\
			"expected %i)."), (int) number, (int) target);	\
		break;							\
	}								\
}

const char *
exif_entry_get_value (ExifEntry *e)
{
	unsigned int i;
	ExifByte v_byte;
	ExifShort v_short, v_short2, v_short3, v_short4;
	ExifLong v_long;
	ExifSLong v_slong;
	ExifRational v_rat;
	ExifSRational v_srat;
	static char v[1024], b[1024];
	const char *c;
	ExifByteOrder o;
	double d;
	ExifEntry *entry;

	bindtextdomain (GETTEXT_PACKAGE, LIBEXIF_LOCALEDIR);

	/* We need the byte order */
	if (!e || !e->parent || !e->parent->parent)
		return (NULL);
	o = exif_data_get_byte_order (e->parent->parent);

	memset (v, 0, sizeof (v));
	memset (b, 0, sizeof (b));

	switch (e->tag) {
	case EXIF_TAG_EXIF_VERSION:
		CF (e->format, EXIF_FORMAT_UNDEFINED, v);
		CC (e->components, 4, v);
		if (!memcmp (e->data, "0200", 4))
			strncpy (v, "Exif Version 2.0", sizeof (v));
		else if (!memcmp (e->data, "0210", 4))
			strncpy (v, "Exif Version 2.1", sizeof (v));
		else if (!memcmp (e->data, "0220", 4))
			strncpy (v, "Exif Version 2.2", sizeof (v));
		else
			strncpy (v, "Unknown Exif Version", sizeof (v));
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		CF (e->format, EXIF_FORMAT_UNDEFINED, v);
		CC (e->components, 4, v);
		if (!memcmp (e->data, "0100", 4))
			strncpy (v, "FlashPix Version 1.0", sizeof (v));
		else
			strncpy (v, "Unknown FlashPix Version", sizeof (v));
		break;
	case EXIF_TAG_COPYRIGHT:
		CF (e->format, EXIF_FORMAT_ASCII, v);
		if (strlen ((char *) e->data))
			strncpy (v, e->data, sizeof (v));
		else
			strncpy (v, "[None]", sizeof (v));
		strncat (v, " (Photographer) - ", sizeof (v));
		if (strlen ((char *) e->data + strlen ((char *) e->data) + 1))
			strncat (v, e->data + strlen (e->data) + 1,
				 sizeof (v));
		else
			strncat (v, "[None]", sizeof (v));
		strncat (v, " (Editor)", sizeof (v));
		break;
	case EXIF_TAG_FNUMBER:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);
		snprintf (v, sizeof (v), "f/%.01f", (float) v_rat.numerator /
						    (float) v_rat.denominator);
		break;
	case EXIF_TAG_APERTURE_VALUE:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);
		snprintf (v, sizeof (v), "f/%.01f",
			  pow (2 , ((float) v_rat.numerator /
				    (float) v_rat.denominator) / 2.));
		break;
	case EXIF_TAG_FOCAL_LENGTH:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);

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

		snprintf (v, sizeof (v), "%.1f mm%s",
			  (float) v_rat.numerator / (float) v_rat.denominator,
			  b);
		break;
	case EXIF_TAG_SUBJECT_DISTANCE:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);
		snprintf (v, sizeof (v), "%.1f m", (float) v_rat.numerator /
						   (float) v_rat.denominator);
		break;
	case EXIF_TAG_EXPOSURE_TIME:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);
		d = (double) v_rat.numerator / (double) v_rat.denominator;
		if (d < 1)
			snprintf (v, sizeof (v), _("1/%d sec."),
				  (int) (1. / d));
		else
			snprintf (v, sizeof (v), _("%d sec."), (int) d);
		break;
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, v);
		CC (e->components, 1, v);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator)
			return (NULL);
		snprintf (b, sizeof (b), "%.0f/%.0f sec.",
			  (float) v_srat.numerator, (float) v_srat.denominator);
		snprintf (v, sizeof (v), "%s (APEX: %i)", b,
			  (int) pow (sqrt(2), (float) v_srat.numerator /
					      (float) v_srat.denominator));
		break;
	case EXIF_TAG_BRIGHTNESS_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, v);
		CC (e->components, 1, v);
		v_srat = exif_get_srational (e->data, o);
		snprintf (v, sizeof (v), "%i/%i", (int) v_srat.numerator, 
						  (int) v_srat.denominator);
		//FIXME: How do I calculate the APEX value?
		break;
	case EXIF_TAG_METERING_MODE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Unknown"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Average"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Center-Weighted Average"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Spot"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("Multi Spot"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("Pattern"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("Partial"), sizeof (v));
			break;
		case 255:
			strncpy (v, _("Other"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_COMPRESSION:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (v, _("Uncompressed"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("JPEG compression"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_FILE_SOURCE:
		CF (e->format, EXIF_FORMAT_UNDEFINED, v);
		CC (e->components, 1, v);
		switch (e->data[0]) {
		case 0x03:
			strncpy (v, _("DSC"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "0x%02x", e->data[0]);
			break;
		}
		break;
	case EXIF_TAG_PLANAR_CONFIGURATION:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (v, _("chunky format"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("planar format"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_COMPONENTS_CONFIGURATION:
		CF (e->format, EXIF_FORMAT_UNDEFINED, v);
		CC (e->components, 4, v);
		for (i = 0; i < 4; i++) {
			switch (e->data[i]) {
			case 0:
				c = _("-");
				break;
			case 1:
				c = _("Y");
				break;
			case 2:
				c = _("Cb");
				break;
			case 3:
				c = _("Cr");
				break;
			case 4:
				c = _("R");
				break;
			case 5:
				c = _("G");
				break;
			case 6:
				c = _("B");
				break;
			default:
				c = _("reserved");
				break;
			}
			strncat (v, c, sizeof (v));
			if (i < 3)
				strncat (v, " ", sizeof (v));
		}
		break;
	case EXIF_TAG_SENSING_METHOD:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (v, _("Not defined"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("One-chip color area sensor"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Two-chip color area sensor"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("Three-chip color area sensor"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("Color sequential area sensor"), sizeof (v));
			break;
		case 7:
			strncpy (v, _("Trilinear sensor"), sizeof (v));
			break;
		case 8:
			strncpy (v, _("Color sequential linear sensor"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Unknown"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Daylight"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Fluorescent"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Tungsten (incandescent light)"),
				 sizeof (v));
			break;
		case 4:
			strncpy (v, _("Flash"), sizeof (v));
			break;
		case 9:
			strncpy (v, _("Fine weather"), sizeof (v));
			break;
		case 10:
			strncpy (v, _("Cloudy weather"), sizeof (v));
			break;
		case 11:
			strncpy (v, _("Shade"), sizeof (v));
			break;
		case 12:
			strncpy (v, _("Daylight fluorescent"), sizeof (v));
			break;
		case 13:
			strncpy (v, _("Day white fluorescent"), sizeof (v));
			break;
		case 14:
			strncpy (v, _("Cool white fluorescent"), sizeof (v));
			break;
		case 15:
			strncpy (v, _("White fluorescent"), sizeof (v));
			break;
		case 17:
			strncpy (v, _("Standard light A"), sizeof (v));
			break;
		case 18:
			strncpy (v, _("Standard light B"), sizeof (v));
			break;
		case 19:
			strncpy (v, _("Standard light C"), sizeof (v));
			break;
		case 20:
			strncpy (v, _("D55"), sizeof (v));
			break;
		case 21:
			strncpy (v, _("D65"), sizeof (v));
			break;
		case 22:
			strncpy (v, _("D75"), sizeof (v));
			break;
		case 24:
			strncpy (v, _("ISO studio tungsten"), sizeof (v));
			break;
		case 255:
			strncpy (v, _("Other"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
	case EXIF_TAG_RESOLUTION_UNIT:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 2:
			strncpy (v, _("Inch"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Centimeter"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Not defined"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Manual"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Normal program"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Aperture priority"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("Shutter priority"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("Creative program (biased toward "
				      "depth of field)"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("Action program (biased toward "
				      "fast shutter speed)"), sizeof (v));
			break;
		case 7:
			strncpy (v, _("Portrait mode (for closeup photos "
				      "with the background out of focus"),
				 sizeof (v));
			break;
		case 8:
			strncpy (v, _("Landscape mode (for landscape "
				      "photos with the background in focus"),
				 sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_EXPOSURE_BIAS_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, v);
		CC (e->components, 1, v);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator)
			return (NULL);
		snprintf (v, sizeof (v), "%s%.01f",
			  v_srat.denominator * v_srat.numerator > 0 ? "+" : "",
			  (double) v_srat.numerator /
			  (double) v_srat.denominator);
		break;
	case EXIF_TAG_ORIENTATION:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (v, _("top - left"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("top - right"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("bottom - right"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("bottom - left"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("left - top"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("right - top"), sizeof (v));
			break;
		case 7:
			strncpy (v, _("right - bottom"), sizeof (v));
			break;
		case 8:
			strncpy (v, _("left - bottom"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_YCBCR_POSITIONING:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (v, _("centered"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("co-sited"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_YCBCR_SUB_SAMPLING:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 2, v);
		v_short  = exif_get_short (e->data, o);
		v_short2 = exif_get_short (
			e->data + exif_format_get_size (e->format),
			o);
		if ((v_short == 2) && (v_short2 == 1))
			strncpy (v, _("YCbCr4:2:2"), sizeof (v));
		else if ((v_short == 2) && (v_short2 == 2))
			strncpy (v, _("YCbCr4:2:0"), sizeof (v));
		else
			snprintf (v, sizeof (v), "%i, %i", v_short, v_short2);
		break;
	case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short  = exif_get_short (e->data, o);
		switch (v_short) {
		case 2:
			strncpy (v, _("RGB"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("YCbCr"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_COLOR_SPACE:
		CF (e->format, EXIF_FORMAT_SHORT, v); 
		CC (e->components, 1, v); 
		v_short = exif_get_short (e->data, o); 
		switch (v_short) { 
		case 1:
			strncpy (v, _("sRGB"), sizeof (v));
			break;
		case 0xffff:
			strncpy (v, _("Uncalibrated"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_FLASH:
		CF (e->format, EXIF_FORMAT_SHORT, v); 
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0x000:
			strncpy (v, _("Flash did not fire."), sizeof (v));
			break;
		case 0x0001:
			strncpy (v, _("Flash fired."), sizeof (v));
			break;
		case 0x0005:
			strncpy (v, _("Strobe return light not detected."),
				 sizeof (v));
			break;
		case 0x0007:
			strncpy (v, _("Strobe return light detected."),
				 sizeof (v));
			break;
		case 0x000d:
			strncpy (v, _("Flash fired, compulsory flash mode, "
				"return light not detected."), sizeof (v));
			break;
		case 0x000f:
			strncpy (v, _("Flash fired, compulsory flash mode, "
				"return light detected."), sizeof (v));
			break;
		case 0x0010:
			strncpy (v, _("Flash did not fire, compulsory flash "
				"mode."), sizeof (v));
			break;
		case 0x0018:
			strncpy (v, _("Flash did not fire, auto mode."),
				 sizeof (v));
			break;
		case 0x0019:
			strncpy (v, _("Flash fired, auto mode."), sizeof (v));
			break;
		case 0x001d:
			strncpy (v, _("Flash fired, auto mode, return light "
				"not detected."), sizeof (v));
			break;
		case 0x001f:
			strncpy (v, _("Flash fired, auto mode, return light "
				"detected."), sizeof (v));
			break;
		case 0x0020:
			strncpy (v, _("No flash function."), sizeof (v));
			break;
		case 0x0041:
			strncpy (v, _("Flash fired, red-eye reduction mode."),
				 sizeof (v));
			break;
		case 0x0045:
			strncpy (v, _("Flash fired, red-eye reduction mode, "
				"return light not detected."), sizeof (v));
			break;
		case 0x0047:
			strncpy (v, _("Flash fired, red-eye reduction mode, "
				"return light detected."), sizeof (v));
			break;
		case 0x0049:
			strncpy (v, _("Flash fired, compulsory flash mode, "
				"red-eye reduction mode."), sizeof (v));
			break;
		case 0x004d:
			strncpy (v, _("Flash fired, compulsory flash mode, "
				"red-eye reduction mode, return light not "
				"detected."), sizeof (v));
			break;
		case 0x004f:
			strncpy (v, _("Flash fired, compulsory flash mode, "
				"red-eye reduction mode, return light "
				"detected."), sizeof (v));
			break;
		case 0x0059:
			strncpy (v, _("Flash fired, auto mode, red-eye "
				"reduction mode."), sizeof (v));
			break;
		case 0x005d:
			strncpy (v, _("Flash fired, auto mode, return light "
				"not detected, red-eye reduction mode."),
				sizeof (v));
			break;
		case 0x005f:
			strncpy (v, _("Flash fired, auto mode, return light "
				"detected, red-eye reduction mode."),
				sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_MAKER_NOTE:
		CF (e->format, EXIF_FORMAT_UNDEFINED, v);
		snprintf (v, sizeof (v), _("%i bytes unknown data"),
			  (int) e->components);
		break;
	case EXIF_TAG_CUSTOM_RENDERED:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Normal process"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Custom process"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_EXPOSURE_MODE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Auto exposure"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Manual exposure"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Auto bracket"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_WHITE_BALANCE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Auto white balance"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Manual white balance"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SCENE_CAPTURE_TYPE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Standard"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Landscape"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Portrait"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Night scene"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_GAIN_CONTROL:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Low gain up"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("High gain up"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Low gain down"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("High gain down"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SATURATION:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Normal"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Low saturation"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("High saturation"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_CONTRAST:
	case EXIF_TAG_SHARPNESS:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
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
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SUBJECT_DISTANCE_RANGE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("Unknown"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("Macro"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("Close view"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("Distant view"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_SUBJECT_AREA:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		switch (e->components) {
		case 2:
			v_short  = exif_get_short (e->data, o);
			v_short2 = exif_get_short (e->data + 2, o);
			snprintf (v, sizeof (v), "(x,y) = (%i,%i)",
				  v_short, v_short2);
			break;
		case 3:
			v_short  = exif_get_short (e->data, o);
			v_short2 = exif_get_short (e->data + 2, o);
			v_short3 = exif_get_short (e->data + 4, o);
			snprintf (v, sizeof (v), _("Within distance %i of "
				"(x,y) = (%i,%i)"), v_short3, v_short,
				v_short2);
			break;
		case 4:
			v_short  = exif_get_short (e->data, o);
			v_short2 = exif_get_short (e->data + 2, o);
			v_short3 = exif_get_short (e->data + 4, o);
			v_short4 = exif_get_short (e->data + 6, o);
			snprintf (v, sizeof (v), _("Within rectangle "
				"(width %i, height %i) around "
				"(x,y) = (%i,%i)"), v_short3, v_short4,
				v_short, v_short2);
			break;
		default:
			snprintf (v, sizeof (v), _("Unexpected number "
				"of components (%li, expected 2, 3, or 4)."),
				e->components);
		}
	default:
		switch (e->format) {
		case EXIF_FORMAT_UNDEFINED:
			break;
		case EXIF_FORMAT_BYTE:
			v_byte = e->data[0];
			snprintf (v, sizeof (v), "0x%02x", v_byte);
			for (i = 1; i < e->components; i++) {
				v_byte = e->data[i];
				snprintf (b, sizeof (b), "0x%02x", v_byte);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		case EXIF_FORMAT_SHORT:
			v_short = exif_get_short (e->data, o);
			snprintf (v, sizeof (v), "%i", v_short);
			for (i = 1; i < e->components; i++) {
				v_short = exif_get_short (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), "%i", v_short);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		case EXIF_FORMAT_LONG:
			v_long = exif_get_long (e->data, o);
			snprintf (v, sizeof (v), "%i", (int) v_long);
			for (i = 1; i < e->components; i++) {
				v_long = exif_get_long (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), "%li", v_long);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		case EXIF_FORMAT_SLONG:
			v_slong = exif_get_slong (e->data, o);
			snprintf (v, sizeof (v), "%li", v_slong);
			for (i = 1; i < e->components; i++) {
				v_long = exif_get_slong (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), "%li", v_long);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		case EXIF_FORMAT_ASCII:
			strncpy (v, e->data, MIN (sizeof (v), e->size));
			break;
		case EXIF_FORMAT_RATIONAL:
			v_rat = exif_get_rational (e->data, o);
			snprintf (v, sizeof (v), "%i/%i",
				  (int) v_rat.numerator,
				  (int) v_rat.denominator);
			for (i = 1; i < e->components; i++) {
				v_rat = exif_get_rational (
					e->data + 8 * i, o);
				snprintf (b, sizeof (b), "%i/%i",
					  (int) v_rat.numerator,
					  (int) v_rat.denominator);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		case EXIF_FORMAT_SRATIONAL:
			v_srat = exif_get_srational (e->data, o);
			snprintf (v, sizeof (v), "%i/%i",
				  (int) v_srat.numerator,
				  (int) v_srat.denominator);
			for (i = 1; i < e->components; i++) {
				v_srat = exif_get_srational (
					e->data + 8 * i, o);
				snprintf (b, sizeof (b), "%i/%i",
					  (int) v_srat.numerator,
					  (int) v_srat.denominator);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		}
	}

	return (v);
}

const char *
exif_entry_get_value_brief (ExifEntry *e)
{
	ExifByteOrder o;
	ExifEntry *entry;
	static char v[1024];
	ExifRational v_rat;
	ExifSRational v_srat;
	ExifShort v_short;
	double d;

	/* We need the byte order */
	if (!e || !e->parent || !e->parent->parent)
		return (NULL);
	o = exif_data_get_byte_order (e->parent->parent);

	memset (v, 0, sizeof (v));

	switch (e->tag) {
	case EXIF_TAG_FOCAL_LENGTH:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
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
		snprintf (v, sizeof (v), "%dmm",
				(int) (d * (double) v_rat.numerator /
				           (double) v_rat.denominator));
		break;
	case EXIF_TAG_EXPOSURE_TIME:
		CF (e->format, EXIF_FORMAT_RATIONAL, v);
		CC (e->components, 1, v);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator)
			return (NULL);
		d = (double) v_rat.numerator / (double) v_rat.denominator;
		if (d < 1.)
			snprintf (v, sizeof (v), "1/%d", (int) (1. / d));
		else
			snprintf (v, sizeof (v), "%d", (int) d);
		break;
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, v);
		CC (e->components, 1, v);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator)
			return (NULL);
		d = 1. / pow (2., (double) v_srat.numerator /
				  (double) v_srat.denominator);
		if (d < 1.)
			snprintf (v, sizeof (v), "1/%d", (int) (1. / d));
		else
			snprintf (v, sizeof (v), "%d", (int) d);
		break;
	case EXIF_TAG_METERING_MODE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("unknown"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("average"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("center-weight"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("spot"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("multi-spot"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("matrix"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("partial"), sizeof (v));
			break;
		case 255:
			strncpy (v, _("other"), sizeof (v));
			break;
		default:
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		}
		break;
	case EXIF_TAG_LIGHT_SOURCE:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0:
			strncpy (v, _("sunny"), sizeof (v));
			break;
		case 1:
			strncpy (v, _("fluorescent"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("tungsten"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("cloudy"), sizeof (v));
			break;
		default:
			return (exif_entry_get_value (e));
		}
		break;
	case EXIF_TAG_RESOLUTION_UNIT:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 2:
			strncpy (v, _("in"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("cm"), sizeof (v));
			break;
		default:
			return (NULL);
		}
		break;
	case EXIF_TAG_EXPOSURE_PROGRAM:
		CF (e->format, EXIF_FORMAT_SHORT, v);
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 1:
			strncpy (v, _("manual"), sizeof (v));
			break;
		case 2:
			strncpy (v, _("normal"), sizeof (v));
			break;
		case 3:
			strncpy (v, _("aperture"), sizeof (v));
			break;
		case 4:
			strncpy (v, _("shutter"), sizeof (v));
			break;
		case 5:
			strncpy (v, _("creative"), sizeof (v));
			break;
		case 6:
			strncpy (v, _("action"), sizeof (v));
			break;
		case 7:
			strncpy (v, _("portrait"), sizeof (v));
			break;
		case 8:
			strncpy (v, _("landscape"), sizeof (v));
			break;
		default:
			return (exif_entry_get_value (e));
		}
		break;
	case EXIF_TAG_FLASH:
		CF (e->format, EXIF_FORMAT_SHORT, v); 
		CC (e->components, 1, v);
		v_short = exif_get_short (e->data, o);
		switch (v_short) {
		case 0x0001:
			strncpy (v, _("yes"), sizeof (v));
			break;
		case 0x0005:
			strncpy (v, _("w/o strobe"), sizeof (v));
			break;
		case 0x0007:
			strncpy (v, _("w. strobe"), sizeof (v));
			break;
		default:
			return (exif_entry_get_value (e));
		}
		break;
	default:
		return (exif_entry_get_value (e));
	}

	return (v);
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
