#include <config.h>
#include "exif-loader.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libexif/i18n.h>

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

	ExifLog *log;
	ExifMem *mem;
};

static void *
exif_loader_alloc (ExifLoader *l, unsigned int i)
{
	void *d;

	if (!l || !i) return NULL;

	d = exif_mem_alloc (l->mem, i);
	if (d) return d;

	EXIF_LOG_NO_MEMORY (l->log, "ExifLog", i);
	return NULL;
}

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

void
exif_loader_write_file (ExifLoader *l, const char *path)
{
	FILE *f;
	int size;
	unsigned char data[1024];

	if (!l) return;

	f = fopen (path, "rb");
	if (!f) {
		exif_log (l->log, EXIF_LOG_CODE_NONE, "ExifLoader",
			  _("The file '%s' could not be opened."), path);
		return;
	}
	while (1) {
		size = fread (data, 1, sizeof (data), f);
		if (size <= 0) break;
		if (!exif_loader_write (l, data, size)) break;
	}
	fclose (f);
}

unsigned char
exif_loader_write (ExifLoader *eld, unsigned char *buf, unsigned int len)
{
	unsigned int i, len_remain;

	if (!eld || !buf || !len) return 0;
	if (eld->state == EL_FAILED) return 0;
	if (eld->size && eld->bytes_read == eld->size) return 0;

	exif_log (eld->log, EXIF_LOG_CODE_DEBUG, "ExifLoader",
		  "Scanning %i byte(s) of data...", len);

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
			case JPEG_MARKER_COM:
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
				    buf[i] == JPEG_MARKER_APP13 ||
				    buf[i] == JPEG_MARKER_COM) {
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

	if (eld->state == EL_EXIF_FOUND) {
		if (eld->buf == NULL) {
			eld->buf = exif_loader_alloc (eld, eld->size);
			if (!eld->buf) return 0;
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
	ExifLoader *l;
	ExifMem *mem = exif_mem_new (exif_mem_alloc_func, 
			exif_mem_realloc_func, exif_mem_free_func);

	l = exif_loader_new_mem (mem);
	exif_mem_unref (mem);

	return l;
}

ExifLoader *
exif_loader_new_mem (ExifMem *mem)
{
	ExifLoader *loader;

	if (!mem) return NULL;
	
	loader = exif_mem_alloc (mem, sizeof (ExifLoader));
	if (!loader) return NULL;
	loader->ref_count = 1;

	loader->mem = mem;
	exif_mem_ref (mem);

	return loader;
}

void
exif_loader_ref (ExifLoader *loader)
{
	if (loader) loader->ref_count++;
}

static void
exif_loader_free (ExifLoader *loader)
{
	ExifMem *mem;

	if (!loader) return;

	mem = loader->mem;
	exif_loader_reset (loader);
	exif_mem_free (mem, loader);
	exif_mem_unref (mem);
}
	
void
exif_loader_unref (ExifLoader *loader)
{
	if (!loader) return;
	if (!--loader->ref_count)
		exif_loader_free (loader);
}

void
exif_loader_reset (ExifLoader *loader)
{
	if (!loader) return;
	exif_mem_free (loader->mem, loader->buf); loader->buf = NULL;
	loader->size = 0;
	loader->bytes_read = 0;
	loader->last_marker = 0;
	loader->state = 0;
}

ExifData *
exif_loader_get_data (ExifLoader *loader)
{
	ExifData *ed;

	if (!loader) return NULL;

	ed = exif_data_new_mem (loader->mem);
	exif_data_log (ed, loader->log);
	exif_data_load_data (ed, loader->buf, loader->bytes_read);

	return ed;
}

void
exif_loader_log (ExifLoader *loader, ExifLog *log)
{
	if (!loader) return;
	exif_log_unref (loader->log);
	loader->log = log;
	exif_log_ref (log);
}
