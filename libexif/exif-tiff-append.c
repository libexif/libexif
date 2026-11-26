/* exif-tiff-append.c
 *
 * Writes the GPS IFD of an ExifData into a TIFF based file by appending to
 * the end of the file. See exif_data_save_tiff_append() in exif-data.h.
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

#include <config.h>

#include <libexif/exif-data.h>
#include <libexif/exif-utils.h>
#include <libexif/exif-format.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#  include <io.h>
#else
#  include <unistd.h>
#endif

/* The TIFF structures: an IFD is a 2 byte entry count, that many 12 byte
 * entries (tag, format, components, value or offset) and a 4 byte offset of
 * the next IFD. Offsets are 32 bit and count from the TIFF header, which is
 * at the start of the file. */
#define TIFF_HEADER_SIZE	8
#define TIFF_IFD0_OFFSET_POS	4
#define TIFF_ENTRY_SIZE		12
#define TIFF_MAX_OFFSET		0xffffffffUL	/* 32 bit offsets */
#define TAG_GPS_INFO		0x8825

/* Result codes of the small I/O helpers */
#define IO_OK		0
#define IO_ERROR	(-1)
#define IO_SHORT	(-2)	/* end of file reached */

static int
io_read_at (int fd, off_t pos, unsigned char *buf, size_t n)
{
	if (lseek (fd, pos, SEEK_SET) != pos)
		return IO_ERROR;
	while (n) {
		ssize_t r = read (fd, buf, n);
		if (r < 0) {
			if (errno == EINTR)
				continue;
			return IO_ERROR;
		}
		if (!r)
			return IO_SHORT;
		buf += r;
		n -= (size_t) r;
	}
	return IO_OK;
}

static int
io_write_at (int fd, off_t pos, const unsigned char *buf, size_t n)
{
	if (lseek (fd, pos, SEEK_SET) != pos)
		return IO_ERROR;
	while (n) {
		ssize_t r = write (fd, buf, n);
		if (r < 0) {
			if (errno == EINTR)
				continue;
			return IO_ERROR;
		}
		if (!r)
			return IO_ERROR;
		buf += r;
		n -= (size_t) r;
	}
	return IO_OK;
}

static int
io_sync (int fd)
{
#ifdef _WIN32
	return _commit (fd) ? IO_ERROR : IO_OK;
#else
	/* EINVAL: the descriptor does not support syncing; there is nothing
	 * more that can be done then. */
	return (fsync (fd) && errno != EINVAL) ? IO_ERROR : IO_OK;
#endif
}

/* Best effort to cut the file back to its old size after a failure. */
static void
io_truncate (int fd, off_t size)
{
#ifdef _WIN32
	(void) _chsize (fd, (long) size);
#else
	if (ftruncate (fd, size)) {
		/* nothing else can be done */
	}
#endif
}

/* Round up to an even number: values and IFDs start at even offsets. */
#define EVEN(x) (((x) + 1) & ~(size_t) 1)

/* Number of bytes of the value of an entry, or 0 if it is inconsistent */
static size_t
entry_value_size (const ExifEntry *e)
{
	unsigned char fs = exif_format_get_size (e->format);
	size_t len;

	if (!fs || !e->components || !e->data)
		return 0;
	if (e->components > TIFF_MAX_OFFSET / fs)
		return 0;
	len = (size_t) fs * e->components;
	if (e->size < len)
		return 0;
	return len;
}

static int
cmp_entries (const void *a, const void *b)
{
	const ExifEntry *ea = *(ExifEntry * const *) a;
	const ExifEntry *eb = *(ExifEntry * const *) b;
	return (ea->tag > eb->tag) - (ea->tag < eb->tag);
}

/* Is this a regular file open for reading and writing but not for append? */
static int
fd_is_usable (int fd)
{
	struct stat st;
#ifdef F_GETFL
	int flags;
#endif

	if (fd < 0 || fstat (fd, &st) || !S_ISREG (st.st_mode))
		return 0;
#ifdef F_GETFL
	flags = fcntl (fd, F_GETFL);
	if (flags < 0 || (flags & O_ACCMODE) != O_RDWR || (flags & O_APPEND))
		return 0;
#endif
	return 1;
}

/* Builds the GPS IFD (including its out-of-line values) as it will be at
 * file offset base, into out which must be large enough; the gps content has
 * n entries which are already sorted. Returns the number of bytes used. */
static size_t
build_gps_ifd (ExifByteOrder order, ExifEntry **sorted, unsigned int n,
	       unsigned char *out, ExifLong base)
{
	size_t pos = 2 + (size_t) TIFF_ENTRY_SIZE * n + 4;
	unsigned int i;

	exif_set_short (out, order, (ExifShort) n);
	for (i = 0; i < n; i++) {
		const ExifEntry *e = sorted[i];
		unsigned char *p = out + 2 + (size_t) TIFF_ENTRY_SIZE * i;
		size_t len = entry_value_size (e);

		exif_set_short (p, order, (ExifShort) e->tag);
		exif_set_short (p + 2, order, (ExifShort) e->format);
		exif_set_long (p + 4, order, (ExifLong) e->components);
		if (len <= 4) {
			memset (p + 8, 0, 4);
			memcpy (p + 8, e->data, len);
		} else {
			exif_set_long (p + 8, order, base + (ExifLong) pos);
			memcpy (out + pos, e->data, len);
			if (len & 1)
				out[pos + len] = 0;
			pos += EVEN (len);
		}
	}
	memset (out + 2 + (size_t) TIFF_ENTRY_SIZE * n, 0, 4);	/* no next IFD */
	return pos;
}

ExifTiffAppendResult
exif_data_save_tiff_append (ExifData *data, int fd)
{
	unsigned char hdr[TIFF_HEADER_SIZE], *ifd0 = NULL, *out = NULL;
	ExifEntry **sorted = NULL;
	ExifByteOrder order;
	ExifTiffAppendResult result;
	unsigned int ngps, n0, nnew, i;
	size_t gps_vals = 0, gps_size, pad, ifd0_size, new_ifd0_size, total;
	unsigned long long eof_ull, new_eof;
	off_t eof;
	ExifLong ifd0_off, gps_off, new_ifd0_off;
	unsigned char old_ifd0_field[4];
	unsigned char *w;
	int inserted, io;

	if (!data || !fd_is_usable (fd))
		return EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT;
	ngps = data->ifd[EXIF_IFD_GPS]->count;
	if (!ngps)
		return EXIF_TIFF_APPEND_ERR_NO_GPS_DATA;

	/* Look at the file: header, then IFD0 */
	eof = lseek (fd, 0, SEEK_END);
	if (eof < 0)
		return EXIF_TIFF_APPEND_ERR_IO;
	eof_ull = (unsigned long long) eof;
	if (eof_ull < 4)
		return EXIF_TIFF_APPEND_ERR_NOT_TIFF;
	io = io_read_at (fd, 0, hdr, 4);
	if (io != IO_OK)
		return EXIF_TIFF_APPEND_ERR_IO;
	if (!memcmp (hdr, "II*\0", 4))
		order = EXIF_BYTE_ORDER_INTEL;
	else if (!memcmp (hdr, "MM\0*", 4))
		order = EXIF_BYTE_ORDER_MOTOROLA;
	else
		return EXIF_TIFF_APPEND_ERR_NOT_TIFF;
	if (eof_ull < TIFF_HEADER_SIZE)
		return EXIF_TIFF_APPEND_ERR_CORRUPT;
	if (io_read_at (fd, TIFF_IFD0_OFFSET_POS, old_ifd0_field, 4) != IO_OK)
		return EXIF_TIFF_APPEND_ERR_IO;
	ifd0_off = exif_get_long (old_ifd0_field, order);
	if (ifd0_off < TIFF_HEADER_SIZE || (unsigned long long) ifd0_off + 2 > eof_ull)
		return EXIF_TIFF_APPEND_ERR_CORRUPT;
	if (io_read_at (fd, (off_t) ifd0_off, hdr, 2) != IO_OK)
		return EXIF_TIFF_APPEND_ERR_IO;
	n0 = exif_get_short (hdr, order);
	ifd0_size = 2 + (size_t) TIFF_ENTRY_SIZE * n0 + 4;
	if ((unsigned long long) ifd0_off + ifd0_size > eof_ull)
		return EXIF_TIFF_APPEND_ERR_CORRUPT;
	if (n0 >= 0xffff)
		return EXIF_TIFF_APPEND_ERR_TOO_LARGE;

	/* Sort the GPS entries and check them */
	sorted = malloc (sizeof (*sorted) * ngps);
	if (!sorted)
		return EXIF_TIFF_APPEND_ERR_NO_MEMORY;
	memcpy (sorted, data->ifd[EXIF_IFD_GPS]->entries, sizeof (*sorted) * ngps);
	qsort (sorted, ngps, sizeof (*sorted), cmp_entries);
	for (i = 0; i < ngps; i++) {
		size_t len = entry_value_size (sorted[i]);
		if (!len || (i && sorted[i]->tag == sorted[i - 1]->tag)) {
			result = EXIF_TIFF_APPEND_ERR_INVALID_ARGUMENT;
			goto done;
		}
		if (len > 4)
			gps_vals += EVEN (len);
	}
	gps_size = 2 + (size_t) TIFF_ENTRY_SIZE * ngps + 4 + gps_vals;

	/* Where everything goes: [pad][GPS IFD + values][new IFD0], all at even
	 * offsets, and nothing may need an offset above 32 bits. */
	pad = (size_t) (eof_ull & 1);
	gps_off = (ExifLong) (eof_ull + pad);
	new_ifd0_size = 2 + (size_t) TIFF_ENTRY_SIZE * (n0 + 1) + 4;
	new_eof = eof_ull + pad + gps_size + new_ifd0_size;
	if (new_eof > TIFF_MAX_OFFSET || (unsigned long long) (off_t) new_eof != new_eof
	    || (off_t) new_eof < 0) {
		result = EXIF_TIFF_APPEND_ERR_TOO_LARGE;
		goto done;
	}
	new_ifd0_off = (ExifLong) (eof_ull + pad + gps_size);

	/* The entries are written with the byte order of the file */
	if (exif_data_get_byte_order (data) != order)
		exif_data_set_byte_order (data, order);

	/* Build the new bytes in memory. */
	ifd0 = malloc (ifd0_size);
	total = pad + gps_size + new_ifd0_size;
	out = calloc (1, total);
	if (!ifd0 || !out) {
		result = EXIF_TIFF_APPEND_ERR_NO_MEMORY;
		goto done;
	}
	if (io_read_at (fd, (off_t) ifd0_off, ifd0, ifd0_size) != IO_OK) {
		result = EXIF_TIFF_APPEND_ERR_IO;
		goto done;
	}
	build_gps_ifd (order, sorted, ngps, out + pad, gps_off);

	/* Copy of IFD0: the entries as they were, the GPSInfo entry in tag
	 * order (replacing a GPSInfo entry that was there), the same next-IFD
	 * pointer. */
	w = out + pad + gps_size + 2;
	nnew = 0;
	inserted = 0;
	for (i = 0; i <= n0; i++) {
		const unsigned char *src = ifd0 + 2 + (size_t) TIFF_ENTRY_SIZE * i;
		unsigned int tag = (i < n0) ? exif_get_short (src, order) : 0xffffu + 1;

		if (tag == TAG_GPS_INFO || (!inserted && tag > TAG_GPS_INFO)) {
			if (!inserted) {
				exif_set_short (w, order, TAG_GPS_INFO);
				exif_set_short (w + 2, order, EXIF_FORMAT_LONG);
				exif_set_long (w + 4, order, 1);
				exif_set_long (w + 8, order, gps_off);
				w += TIFF_ENTRY_SIZE;
				nnew++;
				inserted = 1;
			}
			if (tag == TAG_GPS_INFO)
				continue;	/* a second old entry is dropped */
		}
		if (i < n0) {
			memcpy (w, src, TIFF_ENTRY_SIZE);
			w += TIFF_ENTRY_SIZE;
			nnew++;
		}
	}
	exif_set_short (out + pad + gps_size, order, (ExifShort) nnew);
	memcpy (w, ifd0 + 2 + (size_t) TIFF_ENTRY_SIZE * n0, 4);	/* next IFD */
	total = (size_t) (w + 4 - out);

	/* Step 1: append, and make sure it is on disk before it is used. */
	if (io_write_at (fd, eof, out, total) != IO_OK || io_sync (fd) != IO_OK) {
		io_truncate (fd, eof);
		result = EXIF_TIFF_APPEND_ERR_IO;
		goto done;
	}

	/* Step 2: switch to the new IFD0. This is the only change to bytes that
	 * were there before. */
	exif_set_long (hdr, order, new_ifd0_off);
	if (io_write_at (fd, TIFF_IFD0_OFFSET_POS, hdr, 4) != IO_OK) {
		/* The 4 bytes are in an unknown state: put the old ones back,
		 * and only if that worked, drop the appended data. */
		if (io_write_at (fd, TIFF_IFD0_OFFSET_POS, old_ifd0_field, 4) == IO_OK)
			io_truncate (fd, eof);
		result = EXIF_TIFF_APPEND_ERR_IO;
		goto done;
	}
	result = (io_sync (fd) == IO_OK) ? EXIF_TIFF_APPEND_OK : EXIF_TIFF_APPEND_ERR_IO;

done:
	free (out);
	free (ifd0);
	free (sorted);
	return result;
}
