/* exif-mnote-data-olympus.c
 *
 * Copyright © 2002, 2003 Lutz Mueller <lutz@users.sourceforge.net>
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
#include "exif-mnote-data-olympus.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libexif/exif-utils.h>
#include <libexif/exif-data.h>

#define DEBUG

static void
exif_mnote_data_olympus_clear (ExifMnoteDataOlympus *n)
{
	unsigned int i;

	if (!n) return;

	if (n->entries) {
		for (i = 0; i < n->count; i++)
			if (n->entries[i].data) {
				free (n->entries[i].data);
				n->entries[i].data = NULL;
			}
		free (n->entries);
		n->entries = NULL;
		n->count = 0;
	}
}

static void
exif_mnote_data_olympus_free (ExifMnoteData *n)
{
	if (!n) return;

	exif_mnote_data_olympus_clear ((ExifMnoteDataOlympus *) n);
}

static char *
exif_mnote_data_olympus_get_value (ExifMnoteData *d, unsigned int i, char *val, unsigned int maxlen)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;

	if (!d) return NULL;
	if (n->count <= i) return NULL;
	return mnote_olympus_entry_get_value (&n->entries[i], val, maxlen);
}

static void
exif_mnote_data_olympus_save (ExifMnoteData *ne,
		unsigned char **buf, unsigned int *buf_size)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) ne;
	unsigned int i, o, s, doff, base = 0, o2 = 6;
	int datao = 0;

	if (!n || !buf || !buf_size) return;

	/*
	 * Allocate enough memory for all entries and the number of entries.
	 */
	*buf_size = 6 + 2 + 2 + n->count * 12;
	switch (n->version) {
	case 0: /* Olympus */
		*buf = malloc (*buf_size);
		if (!*buf) return;
		memset (*buf, 0, *buf_size);

		/* Write the header and the number of entries. */
		strcpy (*buf, "OLYMP");
		o2 += 2;
		datao = n->offset;
		break;
	case 1: /* Nikon v1 */
		base = MNOTE_NIKON1_TAG_BASE;
		*buf_size -= 8;
		/* Fall through */
	case 2: /* Nikon v2 */
		*buf_size += 8;
		*buf = malloc (*buf_size);
		if (!*buf) return;
		memset (*buf, 0, *buf_size);

		/* Write the header and the number of entries. */
		strcpy (*buf, "Nikon");
		(*buf)[6] = n->version;
		o2 += 2; *buf_size += 2;
		if (n->version == 2) {
			exif_set_short (*buf + 10, n->order, (ExifShort) ((n->order == EXIF_BYTE_ORDER_INTEL) ? 'II' : 'MM'));
			exif_set_short (*buf + 12, n->order, (ExifShort) 0x2A);
			exif_set_long (*buf + 14, n->order, (ExifShort) 8);
			o2 += 2 + 8;
		}
		datao = -10;
		break;
	}

	exif_set_short (*buf + o2, n->order, (ExifShort) n->count);
	o2 += 2;

	/* Save each entry */
	for (i = 0; i < n->count; i++) {
		o = o2 + i * 12;
		exif_set_short (*buf + o + 0, n->order,
				(ExifShort) (n->entries[i].tag - base));
		exif_set_short (*buf + o + 2, n->order,
				(ExifShort) n->entries[i].format);
		exif_set_long  (*buf + o + 4, n->order,
				n->entries[i].components);
		o += 8;
		s = exif_format_get_size (n->entries[i].format) *
						n->entries[i].components;
		if (s > 4) {
			doff = *buf_size;
			*buf_size += s;
			*buf = realloc (*buf, *buf_size);
			if (!*buf) return;
			exif_set_long (*buf + o, n->order, datao + doff);
		} else
			doff = o;

		/* Write the data. */
		if (n->entries[i].data) {
			memcpy (*buf + doff, n->entries[i].data, s);
		} else {
			/* Most certainly damaged input file */
			memset (*buf + doff, 0, s);
		}
	}
}

static void
exif_mnote_data_olympus_load (ExifMnoteData *en,
			      const unsigned char *buf, unsigned int buf_size)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) en;
	ExifShort c;
	unsigned int i, s, o, o2, datao = 6, base = 0;

	if (!n || !buf) return;

	exif_log (en->log, EXIF_LOG_CODE_DEBUG, "ExifMnoteOlympus",
		  "Trying to parse Olympus/Nikon maker note...");

	/*
	 * Olympus headers start with "OLYMP" and need to have at least
	 * a size of 22 bytes (6 for 'OLYMP', 2 other bytes, 2 for the
	 * number of entries, and 12 for one entry.
	 *
	 * Nikon v1 headers start with "Nikon", 0, 1, 0, number_of_tags,
	 * or just with number_of_tags (models D1H, D1X...).
	 * Nikon v2 headers start with "Nikon", 0, 2, 0, 0, 0, "MM", 0, 0x2A
	 * or "Nikon", 0, 2, 0, 0, 0, "II", 0x2A, 0.
	 */
	if (buf_size - n->offset < 22) return;
	if (!memcmp (buf + 6 + n->offset, "OLYMP", 5)) {
		o2 = 6 + n->offset + 8 + 2;
		c = exif_get_short (buf + 6 + n->offset + 8, n->order);
		n->version = 0;
	} else if (!memcmp (buf + 6 + n->offset, "Nikon", 5)) {
		o2 = 6 + n->offset + 8 + 2;
		datao = o2;
		if (!memcmp(buf + o2 - 4, "\2\0\0\0II\x2A\0", 8)) {
			n->order = EXIF_BYTE_ORDER_INTEL;
			o2 += exif_get_long(buf + o2 + 4, n->order);
			n->version = 2;
		} else if (!memcmp(buf + o2 - 4, "\2\0\0\0MM\0\x2A", 8)) {
			n->order = EXIF_BYTE_ORDER_MOTOROLA;
			o2 += exif_get_long(buf + o2 + 4, n->order);
			n->version = 2;
		} else if (!memcmp(buf + o2 - 4, "\1\0", 2)) {
			o2 -= 2;
			base = MNOTE_NIKON1_TAG_BASE;
			n->version = 1;
		} else {
			return;
		}
		if (o2 + 2 > buf_size) return;
		c = exif_get_short (buf + o2, n->order);
		o2 += 2;
	} else if (!memcmp (buf + 6 + n->offset, "\0\x1b", 2)) {
		o2 = 6 + n->offset;
		c = exif_get_short (buf + o2, n->order);
		o2 += 2;
		n->version = 2;
	} else {
		return;
	}

	/* Read the number of entries and remove old ones. */
	exif_mnote_data_olympus_clear (n);

	n->entries = malloc (sizeof (MnoteOlympusEntry) * c);
	memset (n->entries, 0, sizeof (MnoteOlympusEntry) * c);

	/* Parse the entries */
	for (i = 0; i < c; i++) {
	    o = o2 + 12 * i;
	    if (o + 12 > buf_size) return;

	    n->count = i + 1;
	    n->entries[i].tag        = exif_get_short (buf + o, n->order) + base;
	    n->entries[i].format     = exif_get_short (buf + o + 2, n->order);
	    n->entries[i].components = exif_get_long (buf + o + 4, n->order);
	    n->entries[i].order      = n->order;

	    exif_log (en->log, EXIF_LOG_CODE_DEBUG, "ExifMnoteOlympus",
		      "Loading entry 0x%x ('%s')...", n->entries[i].tag,
		      mnote_olympus_tag_get_name (n->entries[i].tag));

	    /*
	     * Size? If bigger than 4 bytes, the actual data is not
	     * in the entry but somewhere else (offset).
	     */
	    s = exif_format_get_size (n->entries[i].format) *
		   			 n->entries[i].components;
	    if (!s) continue;
	    o += 8;
	    if (s > 4) o = exif_get_long (buf + o, n->order) + datao;
	    if (o + s > buf_size) continue;

	    /* Sanity check */
	    n->entries[i].data = malloc (s);
	    if (!n->entries[i].data) continue;
	    n->entries[i].size = s;
	    memcpy (n->entries[i].data, buf + o, s);
	}
}

static unsigned int
exif_mnote_data_olympus_count (ExifMnoteData *n)
{
	return n ? ((ExifMnoteDataOlympus *) n)->count : 0;
}

static const char *
exif_mnote_data_olympus_get_name (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;

	if (!n) return NULL;
	if (i >= n->count) return NULL;
	return mnote_olympus_tag_get_title (n->entries[i].tag);
}

static const char *
exif_mnote_data_olympus_get_title (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;
	
	if (!n) return NULL;
	if (i >= n->count) return NULL;
        return mnote_olympus_tag_get_title (n->entries[i].tag);
}

static const char *
exif_mnote_data_olympus_get_description (ExifMnoteData *d, unsigned int i)
{
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;
	
	if (!n) return NULL;
	if (i >= n->count) return NULL;
        return mnote_olympus_tag_get_title (n->entries[i].tag);
}

static void
exif_mnote_data_olympus_set_byte_order (ExifMnoteData *d, ExifByteOrder o)
{
	ExifByteOrder o_orig;
	ExifMnoteDataOlympus *n = (ExifMnoteDataOlympus *) d;
	unsigned int i, fs;
	ExifShort s;
	ExifLong l;
	ExifSLong sl;
	ExifRational r;
	ExifSRational sr;

	if (!n) return;

	o_orig = n->order;
	n->order = o;
	for (i = 0; i < n->count; i++) {
		n->entries[i].order = o;
		fs = exif_format_get_size (n->entries[i].format);
		switch (n->entries[i].format) {
		case EXIF_FORMAT_SHORT:
			for (i = 0; i < n->entries[i].components; i++) {
				s = exif_get_short (n->entries[i].data + (i*fs),
						o_orig);
				exif_set_short (n->entries[i].data + (i * fs),
						o, s);
			}
			break;
		case EXIF_FORMAT_LONG:
			for (i = 0; i < n->entries[i].components; i++) {
				l = exif_get_long (n->entries[i].data + (i*fs),
						o_orig);
				exif_set_long (n->entries[i].data + (i * fs),
						o, l);
			}
			break;
		case EXIF_FORMAT_RATIONAL:
			for (i = 0; i < n->entries[i].components; i++) {
				r = exif_get_rational (n->entries[i].data +
						(i * fs), o_orig);
				exif_set_rational (n->entries[i].data +
						(i * fs), o, r);
			}
			break;
		case EXIF_FORMAT_SLONG:
			for (i = 0; i < n->entries[i].components; i++) {
				sl = exif_get_slong (n->entries[i].data +
						(i * fs), o_orig);
				exif_set_slong (n->entries[i].data +
						(i * fs), o, sl);
			}
			break;
		case EXIF_FORMAT_SRATIONAL:
			for (i = 0; i < n->entries[i].components; i++) {
				sr = exif_get_srational (n->entries[i].data +
						(i * fs), o_orig);
				exif_set_srational (n->entries[i].data +
						(i * fs), o, sr);
			}
			break;
		case EXIF_FORMAT_UNDEFINED:
		case EXIF_FORMAT_BYTE:
		case EXIF_FORMAT_ASCII:
		default:
			/* Nothing here. */
			break;
		}
	}
}

static void
exif_mnote_data_olympus_set_offset (ExifMnoteData *n, unsigned int o)
{
	if (n) ((ExifMnoteDataOlympus *) n)->offset = o;
}

ExifMnoteData *
exif_mnote_data_olympus_new (void)
{
	ExifMnoteData *n;

	n = malloc (sizeof (ExifMnoteDataOlympus));
	if (!n) return NULL;
	memset (n, 0, sizeof (ExifMnoteDataOlympus));

	exif_mnote_data_construct (n);

	/* Set up the function pointers */
	n->methods.free            = exif_mnote_data_olympus_free;
	n->methods.set_byte_order  = exif_mnote_data_olympus_set_byte_order;
	n->methods.set_offset      = exif_mnote_data_olympus_set_offset;
	n->methods.load            = exif_mnote_data_olympus_load;
	n->methods.save            = exif_mnote_data_olympus_save;
	n->methods.count           = exif_mnote_data_olympus_count;
	n->methods.get_name        = exif_mnote_data_olympus_get_name;
	n->methods.get_title       = exif_mnote_data_olympus_get_title;
	n->methods.get_description = exif_mnote_data_olympus_get_description;
	n->methods.get_value       = exif_mnote_data_olympus_get_value;

	return n;
}
