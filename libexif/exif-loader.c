#include <config.h>
#include "exif-loader.h"

#include <stdlib.h>
#include <string.h>

#include <libjpeg/jpeg-marker.h>

typedef enum {
	EL_READ = 0,
	EL_READ_SIZE_HIGH_BYTE,
	EL_READ_SIZE_LOW_BYTE,
	EL_SKIP_BYTES,
	EL_EXIF_FOUND,
	EL_FAILED
} ExifLoaderState;

struct _ExifLoader {
	ExifLoaderState state;

	unsigned int size;
	int last_marker;
	unsigned char *buf;
	unsigned int bytes_read;

	unsigned int ref_count;
};

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

/*
 * This function imitates code from libexif, written by Lutz
 * Müller. See libexif/exif-data.c:exif_data_new_from_file. Here, it
 * can cope with a sequence of data chunks.
 */
unsigned char
exif_loader_write (ExifLoader *eld, unsigned char *buf, unsigned int len)
{
	int i, len_remain;

	if (!eld) return 0;
	if (eld->state == EL_FAILED) return 0;
	if (eld->size && eld->bytes_read == eld->size) return 0;

	for (i = 0; (i < len) && (eld->state != EL_EXIF_FOUND) &&
				 (eld->state != EL_FAILED); i++) 
		switch (eld->state) {
		case EL_SKIP_BYTES:
			eld->size--;
			if (eld->size == 0) {
				eld->state = EL_READ;
			}
			break;
			
		case EL_READ_SIZE_HIGH_BYTE:
			eld->size = buf [i] << 8;
			eld->state = EL_READ_SIZE_LOW_BYTE;
			break;
			
		case EL_READ_SIZE_LOW_BYTE:
			eld->size |= buf [i];

			switch (eld->last_marker) {
			case JPEG_MARKER_APP0:
			case JPEG_MARKER_APP13:
				eld->state = EL_SKIP_BYTES;
				eld->size -= 2;
				break;

			case JPEG_MARKER_APP1:
				eld->state = EL_EXIF_FOUND;
				break;

			case 0:
				/*
				 * Assume that we are reading EXIF data. 
				 * This should probably be verified by reading
				 * some bytes ahead.
				 */
				eld->state = EL_EXIF_FOUND;
				break;
			default:
				return 0;
			}

			eld->last_marker = 0;
			break;

		default:
			if (buf[i] != 0xff) {
				if (buf[i] == JPEG_MARKER_APP0 ||
				    buf[i] == JPEG_MARKER_APP1 ||
				    buf[i] == JPEG_MARKER_APP13) {
					eld->state = EL_READ_SIZE_HIGH_BYTE;
					eld->last_marker = buf [i];

				} else if (buf [i] == JPEG_MARKER_SOI) {
					/* Nothing */
				} else {
					/* Assume that we are reading EXIF
					 * data. This should probably be
					 * verified by reading some bytes
					 * ahead.
					 */
					eld->last_marker = JPEG_MARKER_APP1;
					eld->state = EL_READ_SIZE_HIGH_BYTE;
					i--;
				}
			}
		}

	len_remain = len - i;
	if (!len_remain) return 1;

	if (eld->state == EL_EXIF_FOUND && len_remain > 0) {
		if (eld->buf == NULL) {
			eld->buf = malloc (sizeof (unsigned char) * eld->size);
			eld->bytes_read = 0;
		}

		if (eld->bytes_read < eld->size) {
			int cp_len;

			/* the number of bytes we need to copy */
			cp_len = MIN (eld->size - eld->bytes_read, len_remain);
			
			if ((cp_len + eld->bytes_read) > eld->size) return 1;

			/* Copy memory */
			memcpy (eld->buf + eld->bytes_read, &buf[i], cp_len);
			eld->bytes_read += cp_len;
		}
	}

	return 1;
}

ExifLoader *
exif_loader_new (void)
{
	ExifLoader *loader = malloc (sizeof (ExifLoader));

	memset (loader, 0, sizeof (ExifLoader));
	loader->ref_count = 1;
	
	return loader;
}

void
exif_loader_ref (ExifLoader *loader)
{
	if (loader) loader->ref_count++;
}

void
exif_loader_unref (ExifLoader *loader)
{
	if (!loader) return;
	if (!--loader->ref_count) {
		exif_loader_reset (loader);
		free (loader);
	}
}

void
exif_loader_reset (ExifLoader *loader)
{
	if (!loader) return;
	free (loader->buf); loader->buf = NULL;
	loader->size = 0;
	loader->bytes_read = 0;
	loader->last_marker = 0;
	loader->state = 0;
}

ExifData *
exif_loader_get_data (ExifLoader *loader)
{
	return exif_data_new_from_data (loader->buf, loader->bytes_read);
}
