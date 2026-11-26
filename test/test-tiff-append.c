/* test-tiff-append.c
 *
 * Tests exif_data_save_tiff_append(): writing GPS data into a TIFF file by
 * appending to the file. Generates small synthetic TIFF files in both byte
 * orders; no test image is needed.
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
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "libexif/exif-data.h"
#include "libexif/exif-entry.h"
#include "libexif/exif-mem.h"
#include "libexif/exif-utils.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

static int failures;

#define CHECK(cond) do { \
	if (!(cond)) { \
		fprintf (stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #cond); \
		failures++; \
	} \
} while (0)

/* ---- little helpers to read and write TIFF structures ---- */

static unsigned get16 (const uint8_t *p, int le)
{ return le ? (unsigned)(p[0] | p[1] << 8) : (unsigned)(p[1] | p[0] << 8); }

static uint32_t get32 (const uint8_t *p, int le)
{
	return le ? ((uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24)
		  : ((uint32_t)p[3] | (uint32_t)p[2] << 8 | (uint32_t)p[1] << 16 | (uint32_t)p[0] << 24);
}

static void put16 (uint8_t *p, int le, unsigned v)
{
	p[le ? 0 : 1] = v & 0xff;
	p[le ? 1 : 0] = (v >> 8) & 0xff;
}

static void put32 (uint8_t *p, int le, uint32_t v)
{
	int i;
	for (i = 0; i < 4; i++)
		p[le ? i : 3 - i] = (v >> (8 * i)) & 0xff;
}

/* A growing memory buffer. */
typedef struct {
	uint8_t *b;
	size_t n;
	int le;
} Buf;

static void put_ent (Buf *B, unsigned tag, unsigned type, uint32_t count, uint32_t val_or_off, int short_inline)
{
	uint8_t *e = B->b + B->n;
	put16 (e, B->le, tag);
	put16 (e + 2, B->le, type);
	put32 (e + 4, B->le, count);
	if (short_inline) {
		memset (e + 8, 0, 4);
		put16 (e + 8, B->le, val_or_off);
	} else {
		put32 (e + 8, B->le, val_or_off);
	}
	B->n += 12;
}

/* Layout of the synthetic file (offsets from the TIFF header at 0):
 *   8        IFD0 (entries are sorted by tag; two have out-of-line values,
 *            one more is PrintImageMatching-like and sorts after the GPS tag;
 *            has an Exif IFD pointer and a next-IFD pointer to IFD1)
 *   ...      out-of-line values, Exif IFD, IFD1, optional old GPS IFD,
 *            100 bytes of "image data" (+1 odd byte if odd_size)
 */
static size_t image_len = 100;
#define IMAGE_DATA_LEN image_len

static uint8_t *
build_tiff (int le, int with_gps, int odd_size, size_t *len_out)
{
	Buf B;
	uint32_t n0 = with_gps ? 7 : 6;
	uint32_t ifd0_len = 2 + 12 * n0 + 4;
	uint32_t make_off = 8 + ifd0_len;
	uint32_t model_off = make_off + 10;
	uint32_t xres_off = model_off + 12;
	uint32_t pim_off = xres_off + 8;
	uint32_t exif_off = pim_off + 8;
	uint32_t expo_off = exif_off + 30;
	uint32_t ifd1_off = expo_off + 8;
	uint32_t gps_off = ifd1_off + 18;
	uint32_t gps_len = with_gps ? 54 : 0;
	uint32_t blob_off = gps_off + gps_len;
	size_t total = blob_off + IMAGE_DATA_LEN + (odd_size ? 1 : 0);
	size_t i;

	B.b = calloc (1, total + 16);
	B.le = le;
	B.n = 0;
	memcpy (B.b, le ? "II" : "MM", 2);
	put16 (B.b + 2, le, 42);
	put32 (B.b + 4, le, 8);
	B.n = 8;

	/* IFD0 */
	put16 (B.b + B.n, le, n0); B.n += 2;
	put_ent (&B, 0x010f, 2, 9, make_off, 0);	/* Make */
	put_ent (&B, 0x0110, 2, 11, model_off, 0);	/* Model */
	put_ent (&B, 0x0112, 3, 1, 6, 1);		/* Orientation, inline */
	put_ent (&B, 0x011a, 5, 1, xres_off, 0);	/* XResolution */
	put_ent (&B, 0x8769, 4, 1, exif_off, 0);	/* Exif IFD pointer */
	if (with_gps)
		put_ent (&B, 0x8825, 4, 1, gps_off, 0);	/* GPS IFD pointer */
	put_ent (&B, 0xc4a5, 7, 8, pim_off, 0);		/* PrintImageMatching-like */
	put32 (B.b + B.n, le, ifd1_off); B.n += 4;	/* next IFD */
	CHECK (B.n == 8 + ifd0_len);

	memcpy (B.b + make_off, "DroneCam", 9);
	memcpy (B.b + model_off, "Model-X123", 11);
	put32 (B.b + xres_off, le, 300);
	put32 (B.b + xres_off + 4, le, 1);
	memcpy (B.b + pim_off, "PIM\0\1\2\3\4", 8);

	/* Exif IFD */
	B.n = exif_off;
	put16 (B.b + B.n, le, 2); B.n += 2;
	put_ent (&B, 0x829a, 5, 1, expo_off, 0);	/* ExposureTime */
	{
		uint8_t *e = B.b + B.n;
		put16 (e, le, 0x9000); put16 (e + 2, le, 7); put32 (e + 4, le, 4);
		memcpy (e + 8, "0230", 4);
		B.n += 12;
	}
	put32 (B.b + B.n, le, 0); B.n += 4;
	put32 (B.b + expo_off, le, 1);
	put32 (B.b + expo_off + 4, le, 250);

	/* IFD1 */
	B.n = ifd1_off;
	put16 (B.b + B.n, le, 1); B.n += 2;
	put_ent (&B, 0x0100, 4, 1, 640, 0);
	put32 (B.b + B.n, le, 0);

	/* Old GPS IFD, written by "someone else": south, 3 rationals */
	if (with_gps) {
		B.n = gps_off;
		put16 (B.b + B.n, le, 2); B.n += 2;
		{
			uint8_t *e = B.b + B.n;
			put16 (e, le, 1); put16 (e + 2, le, 2); put32 (e + 4, le, 2);
			memcpy (e + 8, "S\0\0\0", 4);
			B.n += 12;
		}
		put_ent (&B, 0x0002, 5, 3, gps_off + 30, 0);
		put32 (B.b + B.n, le, 0); B.n += 4;
		for (i = 0; i < 6; i++)
			put32 (B.b + gps_off + 30 + 4 * i, le, 7 + i);
	}

	for (i = 0; i < IMAGE_DATA_LEN + (odd_size ? 1 : 0); i++)
		B.b[blob_off + i] = (uint8_t)(0xA0 + i);
	*len_out = total;
	return B.b;
}

/* ---- file helpers ---- */

static char tmpname[64];

static int
make_file (const uint8_t *buf, size_t len)
{
	int fd;
	snprintf (tmpname, sizeof (tmpname), "test-tiff-append-XXXXXX");
	fd = mkstemp (tmpname);
	if (fd < 0) {
		perror ("mkstemp");
		exit (1);
	}
	if (write (fd, buf, len) != (ssize_t) len) {
		perror ("write");
		exit (1);
	}
	return fd;
}

static uint8_t *
slurp (int fd, size_t *len)
{
	struct stat st;
	uint8_t *b;
	size_t got = 0;
	if (fstat (fd, &st) != 0)
		exit (1);
	b = malloc (st.st_size + 1);
	while (got < (size_t) st.st_size) {
		ssize_t r = pread (fd, b + got, st.st_size - got, got);
		if (r <= 0)
			exit (1);
		got += r;
	}
	*len = st.st_size;
	return b;
}

/* ---- an independent TIFF reader, used to verify what was written ---- */

typedef struct {
	unsigned tag, type;
	uint32_t count;
	const uint8_t *raw;	/* the 12 raw bytes of the entry */
} TEnt;

static const unsigned fmt_size[] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};

/* Size in bytes of the value of an entry, 0 on error */
static size_t
vsize (const TEnt *e)
{
	if (e->type < 1 || e->type > 12)
		return 0;
	return (size_t) fmt_size[e->type] * e->count;
}

/* Returns a pointer to the value bytes of entry e inside buf (inline or
 * out-of-line), or NULL if it is outside of the buffer. */
static const uint8_t *
value_ptr (const uint8_t *buf, size_t len, int le, const TEnt *e)
{
	size_t sz = vsize (e);
	uint32_t off;
	if (sz <= 4)
		return e->raw + 8;
	off = get32 (e->raw + 8, le);
	if ((size_t) off + sz > len)
		return NULL;
	return buf + off;
}

/* Parses the IFD at off; fills ents (max max_n); returns the entry count, or
 * -1 if broken. *next gets the next-IFD pointer. */
static int
parse_ifd (const uint8_t *buf, size_t len, int le, uint32_t off, TEnt *ents, int max_n, uint32_t *next)
{
	unsigned n, i;
	if ((size_t) off + 2 > len)
		return -1;
	n = get16 (buf + off, le);
	if ((size_t) off + 2 + 12 * n + 4 > len || (int) n > max_n)
		return -1;
	for (i = 0; i < n; i++) {
		const uint8_t *r = buf + off + 2 + 12 * i;
		ents[i].raw = r;
		ents[i].tag = get16 (r, le);
		ents[i].type = get16 (r + 2, le);
		ents[i].count = get32 (r + 4, le);
	}
	if (next)
		*next = get32 (buf + off + 2 + 12 * n, le);
	return (int) n;
}

static const TEnt *
find_ent (const TEnt *ents, int n, unsigned tag)
{
	int i;
	for (i = 0; i < n; i++)
		if (ents[i].tag == tag)
			return &ents[i];
	return NULL;
}

/* Appends to out a textual "reader's view" of an IFD and all of its
 * sub-IFDs and next IFDs: tag, type, count and the resolved value bytes, but
 * no offsets. Entries of tag skip_tag are left out (used for GPSInfo). */
static void
dump_view (const uint8_t *buf, size_t len, int le, uint32_t off, int depth, unsigned skip_tag,
	   char *out, size_t outsz)
{
	TEnt e[64];
	uint32_t next;
	int n, i;
	size_t pos = strlen (out);

	if (depth > 4)
		return;
	n = parse_ifd (buf, len, le, off, e, 64, &next);
	if (n < 0) {
		snprintf (out + pos, outsz - pos, "[BROKEN IFD]");
		return;
	}
	for (i = 0; i < n; i++) {
		size_t sz = vsize (&e[i]), k;
		const uint8_t *v;
		if (e[i].tag == skip_tag)
			continue;
		pos = strlen (out);
		snprintf (out + pos, outsz - pos, "(%04x,%u,%u:", e[i].tag, e[i].type, (unsigned) e[i].count);
		v = value_ptr (buf, len, le, &e[i]);
		if (!v) {
			snprintf (out + strlen (out), outsz - strlen (out), "BAD");
		} else if (e[i].tag == 0x8769 || e[i].tag == 0x8825) {
			dump_view (buf, len, le, get32 (v, le), depth + 1, 0, out, outsz);
		} else {
			for (k = 0; k < sz; k++)
				snprintf (out + strlen (out), outsz - strlen (out), "%02x", v[k]);
		}
		snprintf (out + strlen (out), outsz - strlen (out), ")");
	}
	if (next)
		dump_view (buf, len, le, next, depth, skip_tag, out, outsz);
}

/* ---- GPS data for the tests ---- */

typedef struct {
	char lat_ref;
	uint32_t lat[6];	/* num/den for d, m, s */
	char lon_ref;
	uint32_t lon[6];
	uint32_t alt[2];
	uint8_t alt_ref;
} Gps;

static const Gps gps_a = { 'N', { 38, 1, 43, 1, 12345, 1000 }, 'W', { 9, 1, 8, 1, 4321, 1000 }, { 12345, 100 }, 0 };
static const Gps gps_b = { 'S', { 1, 1, 2, 1, 3, 1 }, 'E', { 100, 1, 59, 1, 59999, 1000 }, { 7, 1 }, 1 };

/* Adds an entry to the GPS IFD the way applications do: the entry gets its
 * own data buffer of the right size. */
static ExifEntry *
add_gps_entry (ExifData *d, ExifTag tag, ExifFormat format, unsigned long components)
{
	ExifEntry *e = exif_entry_new ();
	ExifMem *mem = exif_mem_new_default ();

	e->tag = tag;
	e->format = format;
	e->components = components;
	e->size = exif_format_get_size (format) * components;
	e->data = exif_mem_alloc (mem, e->size);
	memset (e->data, 0, e->size);
	exif_content_add_entry (d->ifd[EXIF_IFD_GPS], e);
	exif_entry_unref (e);
	exif_mem_unref (mem);
	return e;
}

static void
set_rationals (ExifEntry *e, ExifByteOrder o, const uint32_t *v, int n)
{
	int i;
	for (i = 0; i < n; i++) {
		ExifRational r;
		r.numerator = v[2 * i];
		r.denominator = v[2 * i + 1];
		exif_set_rational (e->data + 8 * i, o, r);
	}
}

static ExifData *
make_data (int le, const Gps *g)
{
	ExifData *d = exif_data_new ();
	ExifByteOrder o = le ? EXIF_BYTE_ORDER_INTEL : EXIF_BYTE_ORDER_MOTOROLA;
	ExifEntry *e;

	exif_data_set_byte_order (d, o);
	e = add_gps_entry (d, (ExifTag) EXIF_TAG_GPS_LATITUDE_REF, EXIF_FORMAT_ASCII, 2);
	e->data[0] = g->lat_ref;
	e = add_gps_entry (d, (ExifTag) EXIF_TAG_GPS_LATITUDE, EXIF_FORMAT_RATIONAL, 3);
	set_rationals (e, o, g->lat, 3);
	e = add_gps_entry (d, (ExifTag) EXIF_TAG_GPS_LONGITUDE_REF, EXIF_FORMAT_ASCII, 2);
	e->data[0] = g->lon_ref;
	e = add_gps_entry (d, (ExifTag) EXIF_TAG_GPS_LONGITUDE, EXIF_FORMAT_RATIONAL, 3);
	set_rationals (e, o, g->lon, 3);
	e = add_gps_entry (d, (ExifTag) EXIF_TAG_GPS_ALTITUDE, EXIF_FORMAT_RATIONAL, 1);
	set_rationals (e, o, g->alt, 1);
	e = add_gps_entry (d, (ExifTag) EXIF_TAG_GPS_ALTITUDE_REF, EXIF_FORMAT_BYTE, 1);
	e->data[0] = g->alt_ref;
	return d;
}

/* Verifies the GPS IFD in the file image using the independent reader. */
static void
check_gps_in_file (const uint8_t *buf, size_t len, int le, const Gps *g)
{
	TEnt e0[64], eg[64];
	uint32_t off0 = get32 (buf + 4, le), gps_off;
	int n0 = parse_ifd (buf, len, le, off0, e0, 64, NULL), ng, i, cnt = 0;
	const TEnt *p, *t;
	const uint8_t *v;
	uint32_t prev_tag = 0;

	CHECK (n0 > 0);
	if (n0 <= 0)
		return;
	for (i = 0; i < n0; i++)
		cnt += e0[i].tag == 0x8825;
	CHECK (cnt == 1);
	p = find_ent (e0, n0, 0x8825);
	CHECK (p != NULL);
	if (!p)
		return;
	CHECK (p->type == 4 && p->count == 1);
	gps_off = get32 (p->raw + 8, le);
	CHECK (gps_off % 2 == 0);
	ng = parse_ifd (buf, len, le, gps_off, eg, 64, NULL);
	CHECK (ng == 6);
	if (ng != 6)
		return;
	for (i = 0; i < ng; i++) {	/* sorted, TIFF requires it */
		CHECK (eg[i].tag > prev_tag || i == 0);
		prev_tag = eg[i].tag;
		if (eg[i].tag != 1 && vsize (&eg[i]) > 4)
			CHECK (get32 (eg[i].raw + 8, le) % 2 == 0);
	}
	t = find_ent (eg, ng, 1);	/* GPSLatitudeRef */
	CHECK (t && t->type == 2 && t->count == 2 && t->raw[8] == (uint8_t) g->lat_ref && t->raw[9] == 0);
	t = find_ent (eg, ng, 3);
	CHECK (t && t->type == 2 && t->raw[8] == (uint8_t) g->lon_ref);
	t = find_ent (eg, ng, 2);	/* GPSLatitude */
	CHECK (t && t->type == 5 && t->count == 3);
	v = t ? value_ptr (buf, len, le, t) : NULL;
	CHECK (v != NULL);
	for (i = 0; v && i < 6; i++)
		CHECK (get32 (v + 4 * i, le) == g->lat[i]);
	t = find_ent (eg, ng, 4);	/* GPSLongitude */
	CHECK (t && t->type == 5 && t->count == 3);
	v = t ? value_ptr (buf, len, le, t) : NULL;
	CHECK (v != NULL);
	for (i = 0; v && i < 6; i++)
		CHECK (get32 (v + 4 * i, le) == g->lon[i]);
	t = find_ent (eg, ng, 6);	/* GPSAltitude */
	v = t ? value_ptr (buf, len, le, t) : NULL;
	CHECK (v && get32 (v, le) == g->alt[0] && get32 (v + 4, le) == g->alt[1]);
	t = find_ent (eg, ng, 5);	/* GPSAltitudeRef, BYTE */
	CHECK (t && t->type == 1 && t->raw[8] == g->alt_ref);
}

/* Checks that libexif itself reads the GPS data back from the file image
 * (the images in this test are below 64 KiB). */
static void
check_gps_via_libexif (const uint8_t *buf, size_t len, int le, const Gps *g)
{
	ExifData *d = exif_data_new_from_data (buf, (unsigned) len);
	ExifByteOrder o = le ? EXIF_BYTE_ORDER_INTEL : EXIF_BYTE_ORDER_MOTOROLA;
	ExifEntry *e;
	int i;

	CHECK (d != NULL);
	if (!d)
		return;
	CHECK (exif_data_get_byte_order (d) == o);
	CHECK (d->ifd[EXIF_IFD_GPS]->count == 6);
	e = exif_content_get_entry (d->ifd[EXIF_IFD_GPS], (ExifTag) EXIF_TAG_GPS_LATITUDE);
	CHECK (e && e->components == 3);
	for (i = 0; e && i < 3; i++) {
		ExifRational r = exif_get_rational (e->data + 8 * i, o);
		CHECK (r.numerator == g->lat[2 * i] && r.denominator == g->lat[2 * i + 1]);
	}
	e = exif_content_get_entry (d->ifd[EXIF_IFD_GPS], (ExifTag) EXIF_TAG_GPS_LONGITUDE_REF);
	CHECK (e && e->data[0] == (unsigned char) g->lon_ref);
	e = exif_content_get_entry (d->ifd[EXIF_IFD_0], EXIF_TAG_MAKE);
	CHECK (e && e->size == 9 && !memcmp (e->data, "DroneCam", 9));
	/* the last entry of IFD0, and IFD1 behind the next-IFD pointer */
	e = exif_content_get_entry (d->ifd[EXIF_IFD_0], EXIF_TAG_PRINT_IMAGE_MATCHING);
	CHECK (e && e->size == 8 && !memcmp (e->data, "PIM\0\1\2\3\4", 8));
	e = exif_content_get_entry (d->ifd[EXIF_IFD_1], EXIF_TAG_IMAGE_WIDTH);
	CHECK (e != NULL);
	e = exif_content_get_entry (d->ifd[EXIF_IFD_EXIF], EXIF_TAG_EXIF_VERSION);
	CHECK (e && e->size == 4 && !memcmp (e->data, "0230", 4));
	exif_data_unref (d);
}

/* Everything the old file had must still be there: only the 4 byte first-IFD
 * offset changed, the file only grew at the end, IFD0 got exactly one more
 * entry, and all the other entries still resolve to the same data. */
static void
check_untouched (const uint8_t *orig, size_t olen, const uint8_t *now, size_t nlen, int le,
		 int orig_had_gps)
{
	TEnt eo[64], en[64];
	uint32_t off_o = get32 (orig + 4, le), off_n = get32 (now + 4, le);
	uint32_t nxt_o, nxt_n;
	int no, nn, i, j;
	char vo[4096] = "", vn[4096] = "";

	CHECK (nlen > olen);
	CHECK (memcmp (orig, now, 4) == 0);
	CHECK (memcmp (orig + 8, now + 8, olen - 8) == 0);
	CHECK (off_n >= olen && off_n % 2 == 0 && (size_t) off_n < nlen);
	CHECK (off_n != off_o);

	no = parse_ifd (orig, olen, le, off_o, eo, 64, &nxt_o);
	nn = parse_ifd (now, nlen, le, off_n, en, 64, &nxt_n);
	CHECK (no > 0 && nn > 0);
	if (no <= 0 || nn <= 0)
		return;
	CHECK (nn == no + (orig_had_gps ? 0 : 1));
	CHECK (nxt_o == nxt_n);	/* next-IFD pointer copied */
	/* entries are still sorted */
	for (i = 1; i < nn; i++)
		CHECK (en[i].tag > en[i - 1].tag);
	/* all entries but the GPS one are byte-identical */
	for (i = 0, j = 0; i < no; i++) {
		if (eo[i].tag == 0x8825)
			continue;
		while (j < nn && en[j].tag != eo[i].tag)
			j++;
		CHECK (j < nn);
		if (j < nn)
			CHECK (memcmp (eo[i].raw, en[j].raw, 12) == 0);
	}
	/* and what readers see through them is identical */
	dump_view (orig, olen, le, off_o, 0, 0x8825, vo, sizeof (vo));
	dump_view (now, nlen, le, off_n, 0, 0x8825, vn, sizeof (vn));
	CHECK (strstr (vo, "BAD") == NULL && strstr (vo, "BROKEN") == NULL);
	CHECK (strcmp (vo, vn) == 0);
	if (strcmp (vo, vn))
		fprintf (stderr, "before: %s\nafter:  %s\n", vo, vn);
}

/* ---- the tests ---- */

static void
test_basic (int le, int odd_size, int with_gps)
{
	size_t olen, nlen, n2len;
	uint8_t *orig = build_tiff (le, with_gps, odd_size, &olen), *now, *now2;
	int fd = make_file (orig, olen);
	ExifData *d = make_data (le, &gps_a);
	char v1[4096] = "", v2[4096] = "";

	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_OK);
	now = slurp (fd, &nlen);
	check_untouched (orig, olen, now, nlen, le, with_gps);
	CHECK (nlen < olen + 400);	/* grows by a few hundred bytes at most */
	check_gps_in_file (now, nlen, le, &gps_a);
	check_gps_via_libexif (now, nlen, le, &gps_a);

	/* the old GPS IFD, if any, is dead space but still byte for byte there */
	if (with_gps)
		CHECK (memcmp (orig + 8, now + 8, olen - 8) == 0);

	/* Re-tagging with other values: exactly one effective GPS IFD with the
	 * latest values, the rest as before. */
	exif_data_unref (d);
	d = make_data (le, &gps_b);
	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_OK);
	now2 = slurp (fd, &n2len);
	CHECK (n2len > nlen);
	CHECK (memcmp (now, now2, 4) == 0 && memcmp (now + 8, now2 + 8, nlen - 8) == 0);
	check_untouched (now, nlen, now2, n2len, le, 1);
	check_untouched (orig, olen, now2, n2len, le, with_gps);
	check_gps_in_file (now2, n2len, le, &gps_b);
	check_gps_via_libexif (now2, n2len, le, &gps_b);

	/* Idempotent in what readers see: tagging again with the same values
	 * gives the same view of everything, GPS included. */
	dump_view (now2, n2len, le, get32 (now2 + 4, le), 0, 0, v1, sizeof (v1));
	{
		size_t n3len;
		uint8_t *now3;
		CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_OK);
		now3 = slurp (fd, &n3len);
		dump_view (now3, n3len, le, get32 (now3 + 4, le), 0, 0, v2, sizeof (v2));
		CHECK (strcmp (v1, v2) == 0);
		CHECK (strstr (v1, "(8825,4,1:") != NULL);
		check_gps_in_file (now3, n3len, le, &gps_b);
		free (now3);
	}

	free (now);
	free (now2);
	exif_data_unref (d);
	close (fd);
	unlink (tmpname);
	free (orig);
}

/* A byte order different from the one of the file is converted. */
static void
test_other_byte_order (int le)
{
	size_t olen, nlen;
	uint8_t *orig = build_tiff (le, 0, 0, &olen), *now;
	int fd = make_file (orig, olen);
	ExifData *d = make_data (!le, &gps_a);

	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_OK);
	now = slurp (fd, &nlen);
	check_untouched (orig, olen, now, nlen, le, 0);
	check_gps_in_file (now, nlen, le, &gps_a);
	free (now);
	exif_data_unref (d);
	close (fd);
	unlink (tmpname);
	free (orig);
}

/* Runs an append expected to fail and verifies that the file is unchanged. */
static void
expect_error (const uint8_t *img, size_t len, ExifData *d, ExifTiffAppendResult want, const char *what)
{
	int fd = make_file (img, len);
	size_t nlen;
	uint8_t *now;
	ExifTiffAppendResult r = exif_data_save_tiff_append (d, fd);

	if (r != want) {
		fprintf (stderr, "%s: got result %d, wanted %d\n", what, (int) r, (int) want);
		failures++;
	}
	now = slurp (fd, &nlen);
	if (nlen != len || memcmp (now, img, len)) {
		fprintf (stderr, "%s: the file was modified\n", what);
		failures++;
	}
	free (now);
	close (fd);
	unlink (tmpname);
}

static void
test_errors (int le)
{
	size_t len;
	uint8_t *good = build_tiff (le, 0, 0, &len), *bad;
	ExifData *d = make_data (le, &gps_a), *empty = exif_data_new ();
	uint32_t ifd0 = get32 (good + 4, le);
	int fd;

	/* no GPS entries */
	expect_error (good, len, empty, EXIF_TIFF_APPEND_ERR_NO_GPS_DATA, "empty GPS IFD");

	/* not a TIFF: JPEG start, BigTIFF, garbage, tiny */
	bad = malloc (len);
	memcpy (bad, good, len);
	memcpy (bad, "\xff\xd8\xff\xe1", 4);
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_NOT_TIFF, "JPEG");
	memcpy (bad, good, len);
	memcpy (bad, le ? "II+\0" : "MM\0+", 4);
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_NOT_TIFF, "BigTIFF");
	memcpy (bad, good, len);
	bad[le ? 0 : 1] = le ? 'M' : 'I';	/* "MI" or "II" mixed up */
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_NOT_TIFF, "mixed byte order marks");
	expect_error (good, 3, d, EXIF_TIFF_APPEND_ERR_NOT_TIFF, "3 byte file");
	expect_error (good, 0, d, EXIF_TIFF_APPEND_ERR_NOT_TIFF, "empty file");

	/* truncated: header only, IFD0 offset beyond EOF, IFD0 cut off */
	expect_error (good, 8, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "header only");
	memcpy (bad, good, len);
	put32 (bad + 4, le, (uint32_t) len + 100);
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "IFD0 beyond EOF");
	memcpy (bad, good, len);
	put32 (bad + 4, le, 4);
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "IFD0 inside the header");
	memcpy (bad, good, len);
	put32 (bad + 4, le, 0);
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "IFD0 offset zero");
	expect_error (good, ifd0 + 2 + 12 * 3, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "IFD0 cut in the entries");
	expect_error (good, ifd0 + 2 + 12 * 6 + 2, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "IFD0 cut before next pointer");
	memcpy (bad, good, len);
	put16 (bad + ifd0, le, 65535);
	expect_error (bad, len, d, EXIF_TIFF_APPEND_ERR_CORRUPT, "huge IFD0 entry count");

	/* NULL arguments, bad descriptors */
	CHECK (exif_data_save_tiff_append (NULL, 0) == EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT);
	CHECK (exif_data_save_tiff_append (d, -1) == EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT);

	fd = make_file (good, len);
	close (fd);
	fd = open (tmpname, O_RDONLY);
	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT);
	close (fd);
	fd = open (tmpname, O_RDWR | O_APPEND);
	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT);
	close (fd);
	fd = open (tmpname, O_WRONLY);
	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT);
	close (fd);
	{
		size_t nlen;
		uint8_t *now;
		fd = open (tmpname, O_RDONLY);
		now = slurp (fd, &nlen);
		CHECK (nlen == len && !memcmp (now, good, len));
		free (now);
		close (fd);
	}
	unlink (tmpname);

	/* not a regular file */
	fd = open ("/dev/null", O_RDWR);
	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT);
	close (fd);

	/* GPS entry with inconsistent size */
	{
		ExifEntry *e = exif_content_get_entry (d->ifd[EXIF_IFD_GPS], (ExifTag) EXIF_TAG_GPS_LATITUDE);
		unsigned int s = e->size;
		e->size = 4;	/* shorter than 3 rationals */
		expect_error (good, len, d, EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT, "inconsistent entry size");
		e->size = s;
	}

	exif_data_unref (d);
	exif_data_unref (empty);
	free (bad);
	free (good);
}

/* The file may not grow beyond what 32-bit offsets can address. */
static void
test_too_large (int le)
{
	size_t len;
	uint8_t *good = build_tiff (le, 0, 0, &len);
	ExifData *d = make_data (le, &gps_a);
	int fd = make_file (good, len);
	uint8_t hdr[8];

	if (ftruncate (fd, (off_t) 0xFFFFFFF0u) == 0) {	/* sparse */
		CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_ERR_TOO_LARGE);
		CHECK (lseek (fd, 0, SEEK_END) == (off_t) 0xFFFFFFF0u);
		CHECK (pread (fd, hdr, 8, 0) == 8 && !memcmp (hdr, good, 8));
	} else {
		fprintf (stderr, "skipping the too large test: cannot make a sparse file\n");
	}
	exif_data_unref (d);
	close (fd);
	unlink (tmpname);
	free (good);
}

/* When writing fails (here: the file size limit is hit) the file is left
 * as it was. */
static void
test_write_failure (int le)
{
	size_t len, nlen;
	uint8_t *good = build_tiff (le, 0, 0, &len), *now;
	ExifData *d = make_data (le, &gps_a);
	int fd = make_file (good, len);
	struct rlimit rl, old;

	signal (SIGXFSZ, SIG_IGN);
	getrlimit (RLIMIT_FSIZE, &old);
	rl = old;
	rl.rlim_cur = len + 40;	/* room for part of the new data only */
	if (setrlimit (RLIMIT_FSIZE, &rl) == 0) {
		ExifTiffAppendResult r = exif_data_save_tiff_append (d, fd);
		setrlimit (RLIMIT_FSIZE, &old);
		CHECK (r == EXIF_TIFF_APPEND_ERR_IO);
		now = slurp (fd, &nlen);
		CHECK (nlen == len);
		CHECK (nlen == len && !memcmp (now, good, len));
		free (now);
		/* and it still works afterwards */
		CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_OK);
	}
	exif_data_unref (d);
	close (fd);
	unlink (tmpname);
	free (good);
}

/* The new IFD0 and GPS IFD end up beyond 64 KiB, the size of an Exif APP1
 * segment: libexif must still find them when reading the whole file. */
static void
test_big_file (int le)
{
	size_t olen, nlen;
	uint8_t *orig, *now;
	int fd;
	ExifData *d = make_data (le, &gps_a);

	image_len = 70000;
	orig = build_tiff (le, 0, 0, &olen);
	image_len = 100;
	fd = make_file (orig, olen);
	CHECK (olen > 0x10000);
	CHECK (exif_data_save_tiff_append (d, fd) == EXIF_TIFF_APPEND_OK);
	now = slurp (fd, &nlen);
	check_untouched (orig, olen, now, nlen, le, 0);
	check_gps_in_file (now, nlen, le, &gps_a);
	check_gps_via_libexif (now, nlen, le, &gps_a);
	{
		ExifData *r = exif_data_new_from_data (now, (unsigned) nlen);
		ExifEntry *e = r ? exif_content_get_entry (r->ifd[EXIF_IFD_EXIF], EXIF_TAG_EXIF_VERSION) : NULL;
		CHECK (e && e->size == 4 && !memcmp (e->data, "0230", 4));
		CHECK (r && exif_content_get_entry (r->ifd[EXIF_IFD_0], EXIF_TAG_MODEL) != NULL);
		exif_data_unref (r);
	}
	free (now);
	free (orig);
	exif_data_unref (d);
	close (fd);
	unlink (tmpname);
}

int
main (void)
{
	int le, odd, gps;

	for (le = 0; le <= 1; le++) {
		for (odd = 0; odd <= 1; odd++)
			for (gps = 0; gps <= 1; gps++)
				test_basic (le, odd, gps);
		test_other_byte_order (le);
		test_big_file (le);
		test_errors (le);
		test_too_large (le);
		test_write_failure (le);
	}

	if (failures) {
		fprintf (stderr, "%d check(s) failed\n", failures);
		return 1;
	}
	return 0;
}
