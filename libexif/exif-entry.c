/* exif-entry.c
 *
 * Copyright (C) 2001 Lutz Müller <lutz@users.sourceforge.net>
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
#include "exif-entry.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

static struct {
        ExifFormat format;
        unsigned char size;
} ExifFormatSize[] = {
        {EXIF_FORMAT_BYTE,     1},
        {EXIF_FORMAT_ASCII,    1},
        {EXIF_FORMAT_SHORT,    2},
        {EXIF_FORMAT_LONG,     4},
        {EXIF_FORMAT_RATIONAL, 8},
        {EXIF_FORMAT_SLONG,     4},
        {EXIF_FORMAT_SRATIONAL, 8},
	{EXIF_FORMAT_UNDEFINED, 1},
        {0, 0}
};

struct _ExifEntryPrivate
{
	unsigned int ref_count;
};

ExifEntry *
exif_entry_new (void)
{
	ExifEntry *entry;

	entry = malloc (sizeof (ExifEntry));
	if (!entry)
		return (NULL);
	memset (entry, 0, sizeof (ExifEntry));
	entry->priv = malloc (sizeof (ExifEntryPrivate));
	if (!entry->priv) {
		free (entry);
		return (NULL);
	}
	memset (entry->priv, 0, sizeof (ExifEntryPrivate));
	entry->priv->ref_count = 1;

	entry->content = exif_content_new ();
	if (!entry->content) {
		exif_entry_free (entry);
		return (NULL);
	}
	entry->content->parent = entry;

	return (entry);
}

void
exif_entry_ref (ExifEntry *entry)
{
	entry->priv->ref_count++;
}

void
exif_entry_unref (ExifEntry *entry)
{
	entry->priv->ref_count--;
	if (!entry->priv->ref_count)
		exif_entry_free (entry);
}

void
exif_entry_free (ExifEntry *entry)
{
	if (entry->data)
		free (entry->data);
	exif_content_unref (entry->content);
	free (entry->priv);
	free (entry);
}

void
exif_entry_parse (ExifEntry *entry, const unsigned char *data,
		  unsigned int size, unsigned int offset, ExifByteOrder order)
{
	unsigned int j, s, doff;

	entry->order = order;
	entry->tag         = exif_get_short (data + offset + 0, order);
	entry->format      = exif_get_short (data + offset + 2, order);
	entry->components  = exif_get_long  (data + offset + 4, order);

#ifdef DEBUG
	printf ("Parsing entry (tag 0x%x - '%s')...\n", entry->tag,
		exif_tag_get_name (entry->tag));
#endif

	/*
	 * Size? If bigger than 4 bytes, the actual data is not
	 * in the entry but somewhere else (offset). Beware of subdirectories.
	 */
	for (s = j = 0; ExifFormatSize[j].size; j++)
		if (ExifFormatSize[j].format == entry->format) {
			s = ExifFormatSize[j].size * entry->components;
			break;
		}
	if (!s)
		return;
	if ((s > 4) || (entry->tag == EXIF_TAG_EXIF_IFD_POINTER) ||
		       (entry->tag == EXIF_TAG_GPS_INFO_IFD_POINTER) ||
		       (entry->tag == EXIF_TAG_INTEROPERABILITY_IFD_POINTER))
		doff = exif_get_long (data + offset + 8, order);
	else
		doff = offset + 8;

	/* Sanity check */
	if (size < doff + s)
		return;

	entry->data = malloc (sizeof (char) * s);
	if (!entry->data)
		return;
	entry->size = s;
	memcpy (entry->data, data + doff, s);

	if ((entry->tag == EXIF_TAG_EXIF_IFD_POINTER) ||
	    (entry->tag == EXIF_TAG_GPS_INFO_IFD_POINTER) ||
	    (entry->tag == EXIF_TAG_INTEROPERABILITY_IFD_POINTER))
		exif_content_parse (entry->content, data, size, doff, order);
}

void
exif_entry_dump (ExifEntry *entry, unsigned int indent)
{
	char buf[1024];
	unsigned int i;

	for (i = 0; i < 2 * indent; i++)
		buf[i] = ' ';
	buf[i] = '\0';

	if (!entry)
		return;

	printf ("%s  Tag: 0x%x ('%s')\n", buf, entry->tag,
		exif_tag_get_name (entry->tag));
	printf ("%s  Format: %i\n", buf, entry->format);
	printf ("%s  Components: %i\n", buf, (int) entry->components);
	printf ("%s  Size: %i\n", buf, entry->size);
	if (entry->content->count)
		exif_content_dump (entry->content, indent + 1);
}

const char *
exif_entry_get_value (ExifEntry *entry)
{
	unsigned int i;
	ExifByte v_byte;
	ExifShort v_short;
	ExifLong v_long;
	ExifSLong v_slong;
	ExifRational v_rat;
	ExifSRational v_srat;
	static char v[1024], b[1024];

	memset (v, 0, sizeof (v));
	switch (entry->tag) {
	case EXIF_TAG_EXIF_VERSION:
		if (!memcmp (entry->data, "0200", 4))
			strncpy (v, "Exif Version 2.0", sizeof (v));
		else if (!memcmp (entry->data, "0210", 4))
			strncpy (v, "Exif Version 2.1", sizeof (v));
		else
			strncpy (v, "Unknown Exif Version", sizeof (v));
		break;
	case EXIF_TAG_FLASH_PIX_VERSION:
		if (!memcmp (entry->data, "0100", 4))
			strncpy (v, "FlashPix Version 1.0", sizeof (v));
		else
			strncpy (v, "Unknown FlashPix Version", sizeof (v));
		break;
	default:
		switch (entry->format) {
		case EXIF_FORMAT_UNDEFINED:
			break;
		case EXIF_FORMAT_BYTE:
			v_byte = entry->data[0];
			snprintf (v, sizeof (v), "%i", v_byte);
			break;
		case EXIF_FORMAT_SHORT:
			v_short = exif_get_short (entry->data, entry->order);
			snprintf (v, sizeof (v), "%i", v_short);
			break;
		case EXIF_FORMAT_LONG:
			v_long = exif_get_long (entry->data, entry->order);
			snprintf (v, sizeof (v), "%i", (int) v_long);
			break;
		case EXIF_FORMAT_SLONG:
			v_slong = exif_get_slong (entry->data, entry->order);
			snprintf (v, sizeof (v), "%i", (int) v_slong);
			break;
		case EXIF_FORMAT_ASCII:
			strncpy (v, entry->data, MIN (sizeof (v), entry->size));
			break;
		case EXIF_FORMAT_RATIONAL:
			v_rat = exif_get_rational (entry->data, entry->order);
			snprintf (v, sizeof (v), "%i/%i",
				  (int) v_rat.numerator,
				  (int) v_rat.denominator);
			for (i = 1; i < entry->components; i++) {
				v_rat = exif_get_rational (
					entry->data + 8 * i, entry->order);
				snprintf (b, sizeof (b), "%i/%i",
					  (int) v_rat.numerator,
					  (int) v_rat.denominator);
				strncat (v, ", ", sizeof (v));
				strncat (v, b, sizeof (v));
			}
			break;
		case EXIF_FORMAT_SRATIONAL:
			v_srat = exif_get_srational (entry->data, entry->order);
			snprintf (v, sizeof (v), "%i/%i",
				  (int) v_srat.numerator,
				  (int) v_srat.denominator);
			for (i = 1; i < entry->components; i++) {
				v_srat = exif_get_srational (
					entry->data + 8 * i, entry->order);
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

void
exif_entry_initialize (ExifEntry *entry, ExifTag tag)
{
	if (!entry)
		return;
	if (!entry->content || entry->data)
		return;

	entry->order = entry->content->order;
	entry->tag = tag;
	switch (tag) {
	case EXIF_TAG_X_RESOLUTION:
	case EXIF_TAG_Y_RESOLUTION:
		entry->components = 1;
		entry->format = EXIF_FORMAT_RATIONAL;
		entry->data = malloc (sizeof (ExifRational));
		entry->size = sizeof (ExifRational);
		exif_set_rational (entry->data, entry->order, 72, 1);
		break;
	case EXIF_TAG_RESOLUTION_UNIT:
		entry->components = 1;
		entry->format = EXIF_FORMAT_SHORT;
		entry->data = malloc (sizeof (ExifShort));
		entry->size = sizeof (ExifShort);
		exif_set_short (entry->data, entry->order, 2);
		break;
	default:
		break;
	}
}
