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
	if (!e) return NULL;
	memset (e, 0, sizeof (ExifEntry));
	e->priv = malloc (sizeof (ExifEntryPrivate));
	if (!e->priv) { free (e); return NULL; }
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

static struct {
	ExifTag tag;
	const char *strings[10];
} list[] = {
  { EXIF_TAG_PLANAR_CONFIGURATION,
    { N_("chunky format"), N_("planar format"), NULL}},
  { EXIF_TAG_SENSING_METHOD,
    { "", N_("Not defined"), N_("One-chip color area sensor"),
      N_("Two-chip color area sensor"), N_("Three-chip color area sensor"),
      N_("Color sequential area sensor"), "", N_("Trilinear sensor"),
      N_("Color sequential linear sensor"), NULL}},
  { EXIF_TAG_ORIENTATION,
    { "", N_("top - left"), N_("top - right"), N_("bottom - right"),
      N_("bottom - left"), N_("left - top"), N_("right - top"),
      N_("right - bottom"), N_("left - bottom"), NULL}},
  { EXIF_TAG_YCBCR_POSITIONING,
    { "", N_("centered"), N_("co-sited"), NULL}},
  { EXIF_TAG_PHOTOMETRIC_INTERPRETATION, {"", N_("RGB"), N_("YCbCr"), NULL}},
  { EXIF_TAG_CUSTOM_RENDERED,
    { N_("Normal process"), N_("Custom process"), NULL}},
  { EXIF_TAG_EXPOSURE_MODE,
    { N_("Auto exposure"), N_("Manual exposure"), N_("Auto bracket"), NULL}},
  { EXIF_TAG_WHITE_BALANCE,
    { N_("Auto white balance"), N_("Manual white balance"), NULL}},
  { EXIF_TAG_SCENE_CAPTURE_TYPE,
    { N_("Standard"), N_("Landscape"), N_("Portrait"),
      N_("Night scene"), NULL}},
  { EXIF_TAG_GAIN_CONTROL,
    { N_("Normal"), N_("Low gain up"), N_("High gain up"),
      N_("Low gain down"), N_("High gain down"), NULL}},
  { EXIF_TAG_SATURATION,
    { N_("Normal"), N_("Low saturation"), N_("High saturation"), NULL}},
  { EXIF_TAG_CONTRAST , {N_("Normal"), N_("Soft"), N_("Hard"), NULL}},
  { EXIF_TAG_SHARPNESS, {N_("Normal"), N_("Soft"), N_("Hard"), NULL}},
  { 0, {NULL}}
};

static struct {
  ExifTag tag;
  struct {
    int index;
    const char *values[4];
  } elem[25];
} list2[] = {
  { EXIF_TAG_METERING_MODE,
    { {  0, {N_("Unknown"), NULL}},
      {  1, {N_("Average"), N_("avg"), NULL}},
      {  2, {N_("Center-Weighted Average"), N_("Center-Weight"), NULL}},
      {  3, {N_("Spot"), NULL}},
      {  4, {N_("Multi Spot"), NULL}},
      {  5, {N_("Pattern"), NULL}},
      {  6, {N_("Partial"), NULL}},
      {255, {N_("Other"), NULL}},
      {  0, {NULL}}}},
  { EXIF_TAG_COMPRESSION,
    { {1, {N_("Uncompressed"), NULL}},
      {6, {N_("JPEG compression"), NULL}},
      {0, {NULL}}}},
  { EXIF_TAG_LIGHT_SOURCE,
    { {  0, {N_("Unknown"), NULL}},
      {  1, {N_("Daylight"), NULL}},
      {  2, {N_("Fluorescent"), NULL}},
      {  3, {N_("Tungsten incandescent light"), N_("Tungsten"), NULL}},
      {  4, {N_("Flash"), NULL}},
      {  9, {N_("Fine weather"), NULL}},
      { 10, {N_("Cloudy weather"), N_("cloudy"), NULL}},
      { 11, {N_("Shade"), NULL}},
      { 12, {N_("Daylight fluorescent"), NULL}},
      { 13, {N_("Day white fluorescent"), NULL}},
      { 14, {N_("Cool white fluorescent"), NULL}},
      { 15, {N_("White fluorescent"), NULL}},
      { 17, {N_("Standard light A"), NULL}},
      { 18, {N_("Standard light B"), NULL}},
      { 19, {N_("Standard light C"), NULL}},
      { 20, {N_("D55"), NULL}},
      { 21, {N_("D65"), NULL}},
      { 22, {N_("D75"), NULL}},
      { 24, {N_("ISO studio tungsten"),NULL}},
      {255, {N_("Other"), NULL}},
      {  0, {NULL}}}},
  { EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT,
    { {2, {N_("Inch"), N_("in"), NULL}},
      {3, {N_("Centimeter"), N_("cm"), NULL}},
      {0, {NULL}}}},
  { EXIF_TAG_RESOLUTION_UNIT,
    { {2, {N_("Inch"), N_("in"), NULL}},
      {3, {N_("Centimeter"), N_("cm"), NULL}}, 
      {0, {NULL}}}},
  { EXIF_TAG_EXPOSURE_PROGRAM,
    { {0, {N_("Not defined"), NULL}},
      {1, {N_("Manual"), NULL}},
      {2, {N_("Normal program"), N_("Normal"), NULL}},
      {3, {N_("Aperture priority"), N_("Aperture"), NULL}},
      {4, {N_("Shutter priority"),N_("Shutter"), NULL}},
      {5, {N_("Creative program (biased toward depth of field)"),
	   N_("Creative"), NULL}},
      {6, {N_("Creative program (biased toward fast shutter speed"),
	   N_("Action"), NULL}},
      {7, {N_("Portrait mode (for closeup photos with the background out "
	      "of focus)"), N_("Portrait"), NULL}},
      {8, {N_("Landscape mode (for landscape photos with the background "
	      "in focus)"), N_("Landscape"), NULL}},
      {0, {NULL}}}},
  { EXIF_TAG_FLASH,
    { {0x0000, {N_("Flash did not fire."), N_("no flash"), NULL}},
      {0x0001, {N_("Flash fired."), N_("flash"), N_("Yes"), NULL}},
      {0x0005, {N_("Strobe return light not detected."), N_("W/o strobe"),
		NULL}},
      {0x0007, {N_("Strobe return light detected."), N_("W. strobe"), NULL}},
      {0x0009, {N_("Flash fired, compulsatory flash mode"), NULL}},
      {0x000d, {N_("Flash fired, compulsatory flash mode, return light "
		   "not detected."), NULL}},
      {0x000f, {N_("Flash fired, compulsatory flash mode, return light "
		   "detected."), NULL}},
      {0x0010, {N_("Flash did not fire, compulsatory flash mode."), NULL}},
      {0x0018, {N_("Flash did not fire, auto mode."), NULL}},
      {0x0019, {N_("Flash fired, auto mode."), NULL}},
      {0x001d, {N_("Flash fired, auto mode, return light not detected."),
		NULL}},
      {0x001f, {N_("Flash fired, auto mode, return light detected."), NULL}},
      {0x0020, {N_("No flash function."),NULL}},
      {0x0041, {N_("Flash fired, red-eye reduction mode."), NULL}},
      {0x0045, {N_("Flash fired, red-eye reduction mode, return light "
		   "not detected."), NULL}},
      {0x0047, {N_("Flash fired, red-eye reduction mode, return light "
		   "detected."), NULL}},
      {0x0049, {N_("Flash fired, compulsory flash mode, red-eye reduction "
		   "mode."), NULL}},
      {0x004d, {N_("Flash fired, compulsory flash mode, red-eye reduction "
		  "mode, return light not detected"), NULL}},
      {0x004f, {N_("Flash fired, compulsory flash mode, red-eye reduction, "
		   "return light detected"), NULL}},
      {0x0058, {N_("Flash did not fire, auto mode, red-eye reduction mode"), NULL}},
      {0x0059, {N_("Flash fired, auto mode, red-eye reduction mode"), NULL}},
      {0x005d, {N_("Flash fired, auto mode, return light not detected, "
		   "red-eye reduction mode."), NULL}},
      {0x005f, {N_("Flash fired, auto mode, return light detected, "
		   "red-eye reduction mode."), NULL}},
      {0x0000, {NULL}}}},
  {EXIF_TAG_SUBJECT_DISTANCE_RANGE, 
    { {0, {N_("Unknown"), N_("?"), NULL}},
      {1, {N_("Macro"), NULL}},
      {2, {N_("Close view"), N_("close"), NULL}},
      {3, {N_("Distant view"), N_("distant"), NULL}},
      {0, {NULL}}}},
  { EXIF_TAG_COLOR_SPACE,
    { {1, {N_("sRGB"), NULL}},
      {0xffff, {N_("Uncalibrated"), NULL}}}},
  {0, }
};

const char *
exif_entry_get_value (ExifEntry *e, char *val, unsigned int maxlen)
{
	unsigned int i, j, k, l;
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
		return NULL;
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

		/*
		 * According to Ralf Holzer <rholzer@cmu.edu>,
		 * the user comment field does not have to be 
		 * NULL terminated.
		 */
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
		strncat (val, " ", maxlen - strlen (val));
		strncat (val, _("(Photographer)"), maxlen - strlen (val));

		/* Second part: Editor. */
		strncat (val, " - ", maxlen - strlen (val));
		if (e->size && e->data &&
		    (strlen ((char *) e->data) + 1 < e->size) &&
		    (strspn (e->data, " ") != strlen ((char *) e->data)))
			strncat (val, e->data + strlen (e->data) + 1,
				 maxlen - strlen (val));
		else
			strncat (val, _("[None]"), maxlen - strlen (val));
		strncat (val, " ", maxlen - strlen (val));
		strncat (val, _("(Editor)"), maxlen - strlen (val));

		break;
	case EXIF_TAG_FNUMBER:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return NULL;
		snprintf (val, maxlen, "f/%.01f", (float) v_rat.numerator /
						    (float) v_rat.denominator);
		break;
	case EXIF_TAG_APERTURE_VALUE:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return NULL;
		snprintf (val, maxlen, "f/%.01f",
			  pow (2 , ((float) v_rat.numerator /
				    (float) v_rat.denominator) / 2.));
		break;
	case EXIF_TAG_FOCAL_LENGTH:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return NULL;

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

		snprintf (val, maxlen, "%.1f mm",
			(float) v_rat.numerator / (float) v_rat.denominator);
		if (maxlen > strlen (val) + strlen (b))
			strncat (val, b, maxlen - strlen (val) - 1);
		break;
	case EXIF_TAG_SUBJECT_DISTANCE:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return NULL;
		snprintf (val, maxlen, "%.1f m", (float) v_rat.numerator /
						   (float) v_rat.denominator);
		break;
	case EXIF_TAG_EXPOSURE_TIME:
		CF (e->format, EXIF_FORMAT_RATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_rat = exif_get_rational (e->data, o);
		if (!v_rat.denominator) return NULL;
		d = (double) v_rat.numerator / (double) v_rat.denominator;
		if (d < 1)
			snprintf (val, maxlen, _("1/%d"),
				  (int) (1. / d));
		else
			snprintf (val, maxlen, _("%d"), (int) d);
		if (maxlen > strlen (val) + strlen (" sec."))
			strncat (val, " sec.", maxlen - strlen (val) - 1);
		break;
	case EXIF_TAG_SHUTTER_SPEED_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator) return NULL;
		snprintf (val, maxlen, "%.0f/%.0f", (float) v_srat.numerator,
			  (float) v_srat.denominator);
		if (maxlen > strlen (val) + strlen (" sec."))
			strncat (val, " sec.", maxlen - strlen (val) - 1);
		snprintf (b, sizeof (b), " (APEX: %i)",
			(int) pow (sqrt(2), (float) v_srat.numerator /
				            (float) v_srat.denominator));
		if (maxlen > strlen (val) + strlen (b))
			strncat (val, b, maxlen - strlen (val) - 1);
		break;
	case EXIF_TAG_BRIGHTNESS_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		snprintf (val, maxlen, "%i/%i", (int) v_srat.numerator,
						  (int) v_srat.denominator);
		/* FIXME: How do I calculate the APEX value? */
		break;
	case EXIF_TAG_FILE_SOURCE:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		CC (e->components, 1, val, maxlen);
		switch (e->data[0]) {
		case 0x03: strncpy (val, _("DSC"), maxlen); break;
		default: snprintf (val, maxlen, "0x%02x", e->data[0]); break;
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
			strncat (val, c, maxlen - strlen (val));
			if (i < 3) strncat (val, " ", maxlen - strlen (val));
		}
		break;
	case EXIF_TAG_EXPOSURE_BIAS_VALUE:
		CF (e->format, EXIF_FORMAT_SRATIONAL, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_srat = exif_get_srational (e->data, o);
		if (!v_srat.denominator)
			return NULL;
		snprintf (val, maxlen, "%s%.01f",
			  v_srat.denominator * v_srat.numerator > 0 ? "+" : "",
			  (double) v_srat.numerator /
			  (double) v_srat.denominator);
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
	case EXIF_TAG_MAKER_NOTE:
		CF (e->format, EXIF_FORMAT_UNDEFINED, val, maxlen);
		snprintf (val, maxlen, _("%i bytes unknown data"),
			  (int) e->components);
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
		break;

	case EXIF_TAG_METERING_MODE:
	case EXIF_TAG_COMPRESSION:
	case EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT:
	case EXIF_TAG_RESOLUTION_UNIT:
	case EXIF_TAG_EXPOSURE_PROGRAM:
	case EXIF_TAG_FLASH:
	case EXIF_TAG_SUBJECT_DISTANCE_RANGE:
	case EXIF_TAG_COLOR_SPACE:
		CF (e->format,EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);

		/* Search the tag */
		for (i = 0; list2[i].tag && (list2[i].tag != e->tag); i++);
		if (!list2[i].tag) {
			strncpy (val, "Internal error.", maxlen - 1);
			break;
		}

		/* Find the value */
		for (j = 0; list2[i].elem[j].values &&
			    (list2[i].elem[j].index < v_short); j++);
		if (list2[i].elem[j].index != v_short) {
			snprintf (val, maxlen, "Internal error (unknown "
				  "value %i).", v_short);
			break;
		}

		/* Find a short enough value */
		memset (val, 0, maxlen);
		for (k = 0; list2[i].elem[j].values &&
			    list2[i].elem[j].values[k]; k++) {
		  l = strlen (_(list2[i].elem[j].values[k]));
		  if ((maxlen > l) && (strlen (val) < l))
		    strncpy (val, _(list2[i].elem[j].values[k]), maxlen - 1);
		}
		if (!strlen (val)) snprintf (val, maxlen, "%i", v_short);

		break;
	case EXIF_TAG_PLANAR_CONFIGURATION:
	case EXIF_TAG_SENSING_METHOD:
	case EXIF_TAG_ORIENTATION:
	case EXIF_TAG_YCBCR_POSITIONING:
	case EXIF_TAG_PHOTOMETRIC_INTERPRETATION:
	case EXIF_TAG_CUSTOM_RENDERED:
	case EXIF_TAG_EXPOSURE_MODE:
	case EXIF_TAG_WHITE_BALANCE:
	case EXIF_TAG_SCENE_CAPTURE_TYPE:
	case EXIF_TAG_GAIN_CONTROL:
	case EXIF_TAG_SATURATION:
	case EXIF_TAG_CONTRAST:
	case EXIF_TAG_SHARPNESS:
		CF (e->format, EXIF_FORMAT_SHORT, val, maxlen);
		CC (e->components, 1, val, maxlen);
		v_short = exif_get_short (e->data, o);

		/* Search the tag */
		for (i = 0; list[i].tag && (list[i].tag != e->tag); i++);
		if (!list[i].tag) {
			strncpy (val, "Internal error.", maxlen - 1);
			break;
		}

		/* Find the value */
		for (j = 0; list[i].strings[j] && (j < v_short); j++);
		if (!list[i].strings[j])
			snprintf (val, maxlen, "%i", v_short);
		else
			strncpy (val, _(list[i].strings[j]), maxlen - 1);
		break; 	
	default:
		if (!e->components) break;
		switch (e->format) {
		case EXIF_FORMAT_UNDEFINED:
			break;
		case EXIF_FORMAT_BYTE:
		case EXIF_FORMAT_SBYTE:
			v_byte = e->data[0];
			snprintf (val, maxlen, "0x%02x", v_byte);
			maxlen -= strlen (val);
			for (i = 1; i < e->components; i++) {
				v_byte = e->data[i];
				snprintf (b, sizeof (b), ", 0x%02x", v_byte);
				strncat (val, b, maxlen);
				maxlen -= strlen (b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_SHORT:
		case EXIF_FORMAT_SSHORT:
			v_short = exif_get_short (e->data, o);
			snprintf (val, maxlen, "%i", v_short);
			maxlen -= strlen (val);
			for (i = 1; i < e->components; i++) {
				v_short = exif_get_short (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), ", %i", v_short);
				strncat (val, b, maxlen);
				maxlen -= strlen (b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_LONG:
			v_long = exif_get_long (e->data, o);
			snprintf (val, maxlen, "%i", (int) v_long);
			maxlen -= strlen (val);
			for (i = 1; i < e->components; i++) {
				v_long = exif_get_long (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), ", %li", v_long);
				strncat (val, b, maxlen);
				maxlen -= strlen (b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_SLONG:
			v_slong = exif_get_slong (e->data, o);
			snprintf (val, maxlen, "%li", v_slong);
			maxlen -= strlen (val);
			for (i = 1; i < e->components; i++) {
				v_long = exif_get_slong (e->data +
					exif_format_get_size (e->format) *
					i, o);
				snprintf (b, sizeof (b), ", %li", v_long);
				strncat (val, b, maxlen);
				maxlen -= strlen (b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_ASCII:
			strncpy (val, e->data, MIN (maxlen, e->size));
			break;
		case EXIF_FORMAT_RATIONAL:
			v_rat = exif_get_rational (e->data, o);
			if (v_rat.denominator) {
				snprintf (val, maxlen, "%2.2f", (double)v_rat.numerator / v_rat.denominator);
			} else {
				snprintf (val, maxlen, "%i/%i", v_rat.numerator,  v_rat.denominator);
			}
			maxlen -= strlen (val);
			for (i = 1; i < e->components; i++) {
				v_rat = exif_get_rational (
					e->data + 8 * i, o);
				snprintf (b, sizeof (b), ", %2.2f",
					   (double)v_rat.numerator / v_rat.denominator);
				strncat (val, b, maxlen);
				maxlen -= strlen (b);
				if ((signed)maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_SRATIONAL:
			v_srat = exif_get_srational (e->data, o);
			if (v_srat.denominator) {
				snprintf (val, maxlen, "%2.2f", (double)v_srat.numerator / v_srat.denominator);
			} else {
				snprintf (val, maxlen, "%i/%i", v_srat.numerator,  v_srat.denominator);
			}
			maxlen -= strlen (val);
			for (i = 1; i < e->components; i++) {
				v_srat = exif_get_srational (
					e->data + 8 * i, o);
				snprintf (b, sizeof (b), ", %2.2f",
					  (double)v_srat.numerator / v_srat.denominator);
				strncat (val, b, maxlen);
				maxlen -= strlen (b);
				if ((signed) maxlen <= 0) break;
			}
			break;
		case EXIF_FORMAT_DOUBLE:
		case EXIF_FORMAT_FLOAT:
		default:
			/* What to do here? */
			break;
		}
	}

	return strlen (val) ? val : NULL;
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
	case EXIF_TAG_PIXEL_X_DIMENSION:
	case EXIF_TAG_PIXEL_Y_DIMENSION:
	case EXIF_TAG_EXIF_IFD_POINTER:
	case EXIF_TAG_GPS_INFO_IFD_POINTER:
	case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
	case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
	case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
		e->components = 1;
		e->format = EXIF_FORMAT_LONG;
		e->size = exif_format_get_size (e->format) * e->components;
		e->data = malloc (e->size);
		if (e->data) memset (e->data, 0, e->size);
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
		if (e->data) memset (e->data, 0, e->size);
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
		if (e->data) memset (e->data, 0, e->size);
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
		if (e->data) memset (e->data, 0, e->size);
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
