/* exif-data.c
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

#include <config.h>
#include "exif-mnote-data.h"

#include "exif-data.h"
#include "exif-ifd.h"
#include "exif-mnote-data-priv.h"
#include "exif-utils.h"
#include "exif-loader.h"
#include "jpeg-marker.h"

#include <libexif/olympus/exif-mnote-data-olympus.h>
#include <libexif/canon/exif-mnote-data-canon.h>
#include <libexif/pentax/exif-mnote-data-pentax.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#undef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

/* #define DEBUG */

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

struct _ExifDataPrivate
{
	ExifByteOrder order;

	ExifMnoteData *md;

	unsigned int ref_count;

	/* Temporarily used while loading data */
	unsigned int offset_mnote;
};

ExifMnoteData *
exif_data_get_mnote_data (ExifData *d)
{
	return (d && d->priv) ? d->priv->md : NULL;
}

ExifData *
exif_data_new (void)
{
	ExifData *data;
	unsigned int i;

	data = malloc (sizeof (ExifData));
	if (!data)
		return (NULL);
	memset (data, 0, sizeof (ExifData));
	data->priv = malloc (sizeof (ExifDataPrivate));
	if (!data->priv) {
		free (data);
		return (NULL);
	}
	memset (data->priv, 0, sizeof (ExifDataPrivate));
	data->priv->ref_count = 1;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		data->ifd[i] = exif_content_new ();
		if (!data->ifd[i]) {
			exif_data_free (data);
			return (NULL);
		}
		data->ifd[i]->parent = data;
	}

	return (data);
}

ExifData *
exif_data_new_from_data (const unsigned char *data, unsigned int size)
{
	ExifData *edata;

	edata = exif_data_new ();
	exif_data_load_data (edata, data, size);
	return (edata);
}

static void
exif_data_load_data_entry (ExifData *data, ExifEntry *entry,
			   const unsigned char *d,
			   unsigned int size, unsigned int offset)
{
	unsigned int s, doff;

	entry->tag        = exif_get_short (d + offset + 0, data->priv->order);
	entry->format     = exif_get_short (d + offset + 2, data->priv->order);
	entry->components = exif_get_long  (d + offset + 4, data->priv->order);

	/*
	 * Size? If bigger than 4 bytes, the actual data is not
	 * in the entry but somewhere else (offset).
	 */
	s = exif_format_get_size (entry->format) * entry->components;
	if (!s)
		return;
	if (s > 4)
		doff = exif_get_long (d + offset + 8, data->priv->order);
	else
		doff = offset + 8;

	/* Sanity check */
	if (size < doff + s)
		return;

	entry->data = malloc (sizeof (char) * s);
	if (!entry->data)
		return;
	entry->size = s;
	memcpy (entry->data, d + doff, s);

	/* If this is the MakerNote, remember the offset */
	if (entry->tag == EXIF_TAG_MAKER_NOTE) {
#ifdef DEBUG
		printf ("%02x %02x %02x %02x %02x %02x %02x\n",
			entry->data[0], entry->data[1], entry->data[2],
			entry->data[3], entry->data[4], entry->data[5],
			entry->data[6]);
#endif
		data->priv->offset_mnote = doff;
	}
}

static void
exif_data_save_data_entry (ExifData *data, ExifEntry *e,
			   unsigned char **d, unsigned int *ds,
			   unsigned int offset)
{
	unsigned int doff, s;

	/*
	 * Each entry is 12 bytes long. The memory for the entry has
	 * already been allocated.
	 */
	exif_set_short (*d + 6 + offset + 0,
			data->priv->order, (ExifShort) e->tag);
	exif_set_short (*d + 6 + offset + 2,
			data->priv->order, (ExifShort) e->format);
	exif_set_long  (*d + 6 + offset + 4,
			data->priv->order, e->components);

	/*
	 * Size? If bigger than 4 bytes, the actual data is not in
	 * the entry but somewhere else.
	 */
	s = exif_format_get_size (e->format) * e->components;
	if (!s)
		return;
	if (s > 4) {
		*ds += s;
		*d = realloc (*d, sizeof (char) * *ds);
		doff = *ds - 6 - s;
		exif_set_long (*d + 6 + offset + 8,
			       data->priv->order, doff);
	} else
		doff = offset + 8;

	/* If this is the maker note tag, update it. */
	if ((e->tag == EXIF_TAG_MAKER_NOTE) && data->priv->md) {
		free (e->data);
		e->data = NULL;
		e->size = 0;
		exif_mnote_data_set_offset (data->priv->md, doff);
		exif_mnote_data_save (data->priv->md, &e->data, &e->size);
	}

	/* Write the data. Fill unneeded bytes with 0. */
	memcpy (*d + 6 + doff, e->data, e->size);
	if (s < 4) memset (*d + 6 + doff + s, 0, (4 - s));
}

static void
exif_data_load_data_thumbnail (ExifData *data, const unsigned char *d,
			       unsigned int ds, ExifLong offset, ExifLong size)
{
	if (ds < offset + size) {
#ifdef DEBUG
		printf ("Bogus thumbnail offset and size: %i < %i + %i.\n",
			(int) ds, (int) offset, (int) size);
#endif
		return;
	}
	if (data->data)
		free (data->data);
	data->size = size;
	data->data = malloc (sizeof (char) * data->size);
	memcpy (data->data, d + offset, data->size);
}

static void
exif_data_load_data_content (ExifData *data, ExifContent *ifd,
			     const unsigned char *d,
			     unsigned int ds, unsigned int offset)
{
	ExifLong o, thumbnail_offset = 0, thumbnail_length = 0;
	ExifShort n;
	ExifEntry *entry;
	unsigned int i;
	ExifTag tag;

	/* Read the number of entries */
	if (offset >= ds - 1) return;
	n = exif_get_short (d + offset, data->priv->order);
#ifdef DEBUG
	printf ("Loading %i entries...\n", n);
#endif
	offset += 2;

	/* Check if we have enough data. */
	if (offset + 12 * n > ds) n = (ds - offset) / 12;

	for (i = 0; i < n; i++) {

		tag = exif_get_short (d + offset + 12 * i, data->priv->order);
#ifdef DEBUG
		printf ("Loading entry '%s' (%i of %i)...\n",
			exif_tag_get_name (tag), i + 1, n);
#endif
		switch (tag) {
		case EXIF_TAG_EXIF_IFD_POINTER:
		case EXIF_TAG_GPS_INFO_IFD_POINTER:
		case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
		case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
			o = exif_get_long (d + offset + 12 * i + 8,
					   data->priv->order);
			switch (tag) {
			case EXIF_TAG_EXIF_IFD_POINTER:
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_EXIF], d, ds, o);
				break;
			case EXIF_TAG_GPS_INFO_IFD_POINTER:
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_GPS], d, ds, o);
				break;
			case EXIF_TAG_INTEROPERABILITY_IFD_POINTER:
				exif_data_load_data_content (data,
					data->ifd[EXIF_IFD_INTEROPERABILITY], d, ds, o);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT:
#ifdef DEBUG
				printf ("Thumbnail at %i.\n", (int) o);
#endif
				thumbnail_offset = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
						ds, thumbnail_offset,
						thumbnail_length);
				break;
			case EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH:
#ifdef DEBUG
				printf ("Thumbnail size: %i.\n", (int) o);
#endif
				thumbnail_length = o;
				if (thumbnail_offset && thumbnail_length)
					exif_data_load_data_thumbnail (data, d,
						ds, thumbnail_offset,
						thumbnail_length);
				break;
			default:
				return;
			}
			break;
		default:
			entry = exif_entry_new ();
			exif_content_add_entry (ifd, entry);
			exif_data_load_data_entry (data, entry, d, ds,
						   offset + 12 * i);
			exif_entry_unref (entry);
			break;
		}
	}
}

static void
exif_data_save_data_content (ExifData *data, ExifContent *ifd,
			     unsigned char **d, unsigned int *ds,
			     unsigned int offset)
{
	unsigned int j, n_ptr = 0, n_thumb = 0;
	ExifIfd i;

	if (!data || !ifd || !d || !ds)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		if (ifd == data->ifd[i])
			break;
	if (i == EXIF_IFD_COUNT)
		return;

	/*
	 * Check if we need some extra entries for pointers or the thumbnail.
	 */
	switch (i) {
	case EXIF_IFD_0:

		/*
		 * The pointer to IFD_EXIF is in IFD_0. The pointer to
		 * IFD_INTEROPERABILITY is in IFD_EXIF.
		 */
		if (data->ifd[EXIF_IFD_EXIF]->count ||
		    data->ifd[EXIF_IFD_INTEROPERABILITY]->count)
			n_ptr++;

		/* The pointer to IFD_GPS is in IFD_0. */
		if (data->ifd[EXIF_IFD_GPS]->count)
			n_ptr++;

		break;
	case EXIF_IFD_1:
		if (data->size)
			n_thumb = 2;
		break;
	case EXIF_IFD_EXIF:
		if (data->ifd[EXIF_IFD_INTEROPERABILITY]->count)
			n_ptr++;
	default:
		break;
	}

	/*
	 * Allocate enough memory for all entries
	 * and the number of entries.
	 */
	*ds += (2 + (ifd->count + n_ptr + n_thumb) * 12 + 4);
	*d = realloc (*d, sizeof (char) * *ds);

	/* Save the number of entries */
	exif_set_short (*d + 6 + offset, data->priv->order,
			(ExifShort) (ifd->count + n_ptr + n_thumb));
	offset += 2;

#ifdef DEBUG
	printf ("Saving %i entries (IFD '%s', offset: %i)...\n",
		ifd->count, exif_ifd_get_name (i), offset);
#endif

	/* Save each entry */
	for (j = 0; j < ifd->count; j++)
		exif_data_save_data_entry (data, ifd->entries[j],
				d, ds, offset + 12 * j);
	offset += 12 * ifd->count;

	/* Now save special entries. */
	switch (i) {
	case EXIF_IFD_0:

		/*
		 * The pointer to IFD_EXIF is in IFD_0.
		 * However, the pointer to IFD_INTEROPERABILITY is in IFD_EXIF,
		 * therefore, if IFD_INTEROPERABILITY is not empty, we need
		 * IFD_EXIF even if latter is empty.
		 */
		if (data->ifd[EXIF_IFD_EXIF]->count ||
		    data->ifd[EXIF_IFD_INTEROPERABILITY]->count) {
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_EXIF_IFD_POINTER);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			exif_data_save_data_content (data,
				data->ifd[EXIF_IFD_EXIF], d, ds, *ds - 6);
			offset += 12;
		}

		/* The pointer to IFD_GPS is in IFD_0, too. */
		if (data->ifd[EXIF_IFD_GPS]->count) {
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_GPS_INFO_IFD_POINTER);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			exif_data_save_data_content (data,
				data->ifd[EXIF_IFD_GPS], d, ds, *ds - 6);
			offset += 12;
		}

		break;
	case EXIF_IFD_EXIF:

		/*
		 * The pointer to IFD_INTEROPERABILITY is in IFD_EXIF.
		 * See note above.
		 */
		if (data->ifd[EXIF_IFD_INTEROPERABILITY]->count) {
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_INTEROPERABILITY_IFD_POINTER);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			exif_data_save_data_content (data,
				data->ifd[EXIF_IFD_INTEROPERABILITY], d, ds,
				*ds - 6);
			offset += 12;
		}

		break;
	case EXIF_IFD_1:

		/*
		 * Information about the thumbnail (if any) is saved in
		 * IFD_1.
		 */
		if (data->size) {

			/* EXIF_TAG_JPEG_INTERCHANGE_FORMAT */
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
					EXIF_TAG_JPEG_INTERCHANGE_FORMAT);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					*ds - 6);
			*ds += data->size;
			*d = realloc (*d, sizeof (char) * *ds);
			memcpy (*d + *ds - data->size, data->data, data->size);
			offset += 12;
#ifdef DEBUG
			printf ("Wrote %i bytes of thumbnail data at offset "
				"%i.\n", data->size, *ds - data->size);
			printf ("We currently have %i bytes EXIF data.\n", *ds);
#endif

			/* EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH */
			exif_set_short (*d + 6 + offset + 0, data->priv->order,
				EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH);
			exif_set_short (*d + 6 + offset + 2, data->priv->order,
					EXIF_FORMAT_LONG);
			exif_set_long  (*d + 6 + offset + 4, data->priv->order,
					1);
			exif_set_long  (*d + 6 + offset + 8, data->priv->order,
					data->size);
			offset += 12;
		}

		break;
	default:
		break;
	}

	/* Correctly terminate the directory */
	if (i == EXIF_IFD_0 && (data->ifd[EXIF_IFD_1]->count ||
					     data->size)) {

		/*
		 * We are saving IFD 0. Tell where IFD 1 starts and save
		 * IFD 1.
		 */
		exif_set_long (*d + 6 + offset, data->priv->order, *ds - 6);
		exif_data_save_data_content (data, data->ifd[EXIF_IFD_1], d, ds,
					     *ds - 6);
	} else
		exif_set_long (*d + 6 + offset, data->priv->order, 0);
}

void
exif_data_load_data (ExifData *data, const unsigned char *d_orig,
		     unsigned int ds_orig)
{
	unsigned int l;
	ExifLong offset;
	ExifShort n;
	ExifEntry *e, *em;
	const unsigned char *d = d_orig;
	unsigned int ds = ds_orig, len;

	if (!data)
		return;
	if (!d || !ds)
		return;

#ifdef DEBUG
	printf ("Parsing %i byte(s) EXIF data...\n", ds);
#endif

	/*
	 * It can be that the data starts with the EXIF header. If it does
	 * not, search the EXIF marker.
	 */
	if (ds < 6) {
#ifdef DEBUG
		printf ("Size too small.\n");
#endif
		return;
	}
	if (!memcmp (d, ExifHeader, 6)) {
#ifdef DEBUG
		printf ("Found EXIF header.\n");
#endif
	} else {
#ifdef DEBUG
		printf ("Data begins with 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
			"0x%x...\n", d[0], d[1], d[2], d[3], d[4], d[5], d[6]);
#endif
		while (1) {
			while ((d[0] == 0xff) && ds) {
				d++;
				ds--;
			}

			/* JPEG_MARKER_SOI */
			if (d[0] == JPEG_MARKER_SOI) {
				d++;
				ds--;
				continue;
			}

			/* JPEG_MARKER_APP0 */
			if (d[0] == JPEG_MARKER_APP0) {
				d++;
				ds--;
				l = (d[0] << 8) | d[1];
				if (l > ds)
					return;
				d += l;
				ds -= l;
				continue;
			}

			/* JPEG_MARKER_APP1 */
			if (d[0] == JPEG_MARKER_APP1)
				break;

			/* Unknown marker or data. Give up. */
#ifdef DEBUG
			printf ("EXIF marker not found.\n");
#endif
			return;
		}
		d++;
		ds--;
		if (ds < 2) {
#ifdef DEBUG
			printf ("Size too small.\n");
#endif
			return;
		}
		len = (d[0] << 8) | d[1];
#ifdef DEBUG
		printf ("We have to deal with %i byte(s) of EXIF data.\n", len);
#endif
		d += 2;
		ds -= 2;
	}

	/*
	 * Verify the exif header
	 * (offset 2, length 6).
	 */
	if (ds < 6) {
#ifdef DEBUG
		printf ("Size too small.\n");
#endif
		return;
	}
	if (memcmp (d, ExifHeader, 6)) {
#ifdef DEBUG
		printf ("EXIF header not found.\n");
#endif
		return;
	}

#ifdef DEBUG
	printf ("Found EXIF header.\n");
#endif

	/* Byte order (offset 6, length 2) */
	if (ds < 12)
		return;
	if (!memcmp (d + 6, "II", 2))
		data->priv->order = EXIF_BYTE_ORDER_INTEL;
	else if (!memcmp (d + 6, "MM", 2))
		data->priv->order = EXIF_BYTE_ORDER_MOTOROLA;
	else
		return;

	/* Fixed value */
	if (exif_get_short (d + 8, data->priv->order) != 0x002a)
		return;

	/* IFD 0 offset */
	offset = exif_get_long (d + 10, data->priv->order);
#ifdef DEBUG
	printf ("IFD 0 at %i.\n", (int) offset);
#endif

	/* Parse the actual exif data (offset 14) */
	exif_data_load_data_content (data, data->ifd[EXIF_IFD_0], d + 6,
				     ds - 6, offset);

	/* IFD 1 offset */
	n = exif_get_short (d + 6 + offset, data->priv->order);
	offset = exif_get_long (d + 6 + offset + 2 + 12 * n, data->priv->order);
	if (offset) {
#ifdef DEBUG
		printf ("IFD 1 at %i.\n", (int) offset);
#endif

		/* Sanity check. */
		if (offset > ds - 6) {
#ifdef DEBUG
			printf ("Bogus offset!\n");
#endif
			return;
		}

		exif_data_load_data_content (data, data->ifd[EXIF_IFD_1], d + 6,
					     ds - 6, offset);
	}

	/*
	 * If we got an EXIF_TAG_MAKER_NOTE, try to interpret it. Some
	 * cameras use pointers in the maker note tag that point to the
	 * space between IFDs. Here is the only place where we have access
	 * to that data.
	 */
	e = exif_data_get_entry (data, EXIF_TAG_MAKER_NOTE);
	if (e) {

	    /* Olympus */
	    if ((e->size >= 5) && (!memcmp (e->data, "OLYMP", 5)))
		data->priv->md = exif_mnote_data_olympus_new ();

	    /* Pentax */
	    else if ((e->size >= 2) && (e->data[0] == 0x00)
				    && (e->data[1] == 0x1b))
		data->priv->md = exif_mnote_data_pentax_new ();

	    else {
		em = exif_data_get_entry (data, EXIF_TAG_MAKE);
		if (em) {

		    /* Canon */
		    if (!strcmp (exif_entry_get_value (em), "Canon"))
			data->priv->md = exif_mnote_data_canon_new ();
		}
	    }

	    /* 
	     * If we are able to interpret the maker note, do so.
	     */
	    if (data->priv->md) {
		exif_mnote_data_set_byte_order (data->priv->md,
						data->priv->order);
		exif_mnote_data_set_offset (data->priv->md,
					    data->priv->offset_mnote);
		exif_mnote_data_load (data->priv->md, d, ds);
	    }
	}
}

void
exif_data_save_data (ExifData *data, unsigned char **d, unsigned int *ds)
{
	if (!data)
		return;
	if (!d || !ds)
		return;

	/* Header */
	*ds = 6;
	*d = malloc (sizeof (char) * *ds);
	memcpy (*d, ExifHeader, 6);

	/* Order (offset 6) */
	*ds += 2;
	*d = realloc (*d, sizeof (char) * *ds);
	if (data->priv->order == EXIF_BYTE_ORDER_INTEL) {
		memcpy (*d + 6, "II", 2);
	} else {
		memcpy (*d + 6, "MM", 2);
	}

	/* Fixed value (2 bytes, offset 8) */
	*ds += 2;
	*d = realloc (*d, sizeof (char) * *ds);
	exif_set_short (*d + 8, data->priv->order, 0x002a);

	/*
	 * IFD 0 offset (4 bytes, offset 10).
	 * We will start 8 bytes after the
	 * EXIF header (2 bytes for order, another 2 for the test, and
	 * 4 bytes for the IFD 0 offset make 8 bytes together).
	 */
	*ds += 4;
	*d = realloc (*d, sizeof (char) * *ds);
	exif_set_long (*d + 10, data->priv->order, 8);

	/* Now save IFD 0. IFD 1 will be saved automatically. */
#ifdef DEBUG
	printf ("Saving IFDs...\n");
#endif
	exif_data_save_data_content (data, data->ifd[EXIF_IFD_0], d, ds,
				     *ds - 6);

#ifdef DEBUG
	printf ("Saved %i byte(s) EXIF data.\n", *ds);
#endif
}

ExifData *
exif_data_new_from_file (const char *path)
{
	FILE *f;
	int size;
	ExifData *edata;
	ExifLoader *loader;
	unsigned char data[1024];

	f = fopen (path, "rb");
	if (!f)
		return (NULL);

	loader = exif_loader_new ();
	while (1) {
		size = fread (data, 1, 1024, f);
		if (size <= 0) break;
		if (!exif_loader_write (loader, data, size)) break;
	}
	fclose (f);

	edata = exif_loader_get_data (loader);
	exif_loader_unref (loader);

	return (edata);
}

void
exif_data_ref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count++;
}

void
exif_data_unref (ExifData *data)
{
	if (!data)
		return;

	data->priv->ref_count--;
	if (!data->priv->ref_count)
		exif_data_free (data);
}

void
exif_data_free (ExifData *data)
{
	unsigned int i;

	if (!data)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i]) {
			exif_content_unref (data->ifd[i]);
			data->ifd[i] = NULL;
		}
	}
	if (data->data) {
		free (data->data);
		data->data = NULL;
	}
	if (data->priv) {
		if (data->priv->md) {
			exif_mnote_data_unref (data->priv->md);
			data->priv->md = NULL;
		}
		free (data->priv);
		data->priv = NULL;
	}
	free (data);
}

void
exif_data_dump (ExifData *data)
{
	unsigned int i;

	if (!data)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++) {
		if (data->ifd[i] && data->ifd[i]->count) {
			printf ("Dumping IFD '%s'...\n",
				exif_ifd_get_name (i));
			exif_content_dump (data->ifd[i], 0);
		}
	}

	if (data->data) {
		printf ("%i byte(s) thumbnail data available.", data->size);
		if (data->size >= 4) {
			printf ("0x%02x 0x%02x ... 0x%02x 0x%02x\n",
				data->data[0], data->data[1],
				data->data[data->size - 2],
				data->data[data->size - 1]);
		}
	}
}

ExifByteOrder
exif_data_get_byte_order (ExifData *data)
{
	if (!data)
		return (0);

	return (data->priv->order);
}

void
exif_data_foreach_content (ExifData *data, ExifDataForeachContentFunc func,
			   void *user_data)
{
	unsigned int i;

	if (!data || !func)
		return;

	for (i = 0; i < EXIF_IFD_COUNT; i++)
		func (data->ifd[i], user_data);
}

typedef struct _ByteOrderChangeData ByteOrderChangeData;
struct _ByteOrderChangeData {
	ExifByteOrder old, new;
};

static void
entry_set_byte_order (ExifEntry *e, void *data)
{
	ByteOrderChangeData *d = data;
	unsigned int i;
	ExifShort s;
	ExifLong l;
	ExifSLong sl;
	ExifRational r;
	ExifSRational sr;

	if (!e)
		return;

	switch (e->format) {
	case EXIF_FORMAT_SHORT:
		for (i = 0; i < e->components; i++) {
			s = exif_get_short (e->data +
				(i * exif_format_get_size (e->format)),
				d->old);
			exif_set_short (e->data +
				(i * exif_format_get_size (e->format)),
				d->new, s);
		}
		break;
	case EXIF_FORMAT_LONG:
		for (i = 0; i < e->components; i++) {
			l = exif_get_long (e->data +
				(i * exif_format_get_size (e->format)),
				d->old);
			exif_set_long (e->data +
				(i * exif_format_get_size (e->format)),
				d->new, l);
		}
		break;
	case EXIF_FORMAT_RATIONAL:
		for (i = 0; i < e->components; i++) {
			r = exif_get_rational (e->data +
				(i * exif_format_get_size (e->format)),
				d->old);
			exif_set_rational (e->data +
				(i * exif_format_get_size (e->format)),
				d->new, r);
		}
		break;
	case EXIF_FORMAT_SLONG:
		for (i = 0; i < e->components; i++) {
			sl = exif_get_slong (e->data +
				(i * exif_format_get_size (e->format)),
				d->old);
			exif_set_slong (e->data +
				(i * exif_format_get_size (e->format)),
				d->new, sl);
		}
		break;
	case EXIF_FORMAT_SRATIONAL:
		for (i = 0; i < e->components; i++) {
			sr = exif_get_srational (e->data +
				(i * exif_format_get_size (e->format)),
				d->old);
			exif_set_srational (e->data +
				(i * exif_format_get_size (e->format)),
				d->new, sr);
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

static void
content_set_byte_order (ExifContent *content, void *data)
{
	exif_content_foreach_entry (content, entry_set_byte_order, data);
}

void
exif_data_set_byte_order (ExifData *data, ExifByteOrder order)
{
	ByteOrderChangeData d;

	if (!data || (order == data->priv->order))
		return;

	d.old = data->priv->order;
	d.new = order;
	exif_data_foreach_content (data, content_set_byte_order, &d);
	data->priv->order = order;
	if (data->priv->md)
		exif_mnote_data_set_byte_order (data->priv->md, order);
}
