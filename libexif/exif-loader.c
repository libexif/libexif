#include <config.h>

#include <libexif/exif-loader.h>
#include <libexif/i18n.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libjpeg/jpeg-marker.h>

typedef enum {
	EL_READ = 0,
	EL_READ_SIZE_HIGH_BYTE,
	EL_READ_SIZE_LOW_BYTE,
	EL_READ_SIZE_HIGH_BYTE_FINAL,
	EL_READ_SIZE_LOW_BYTE_FINAL,
	EL_SKIP_BYTES,
	EL_EXIF_FOUND,
} ExifLoaderState;

struct _ExifLoader {
	ExifLoaderState state;

	/* Small buffer used for detection of format */
	unsigned char b[12];
	unsigned char b_len;

	unsigned int size;
	unsigned char *buf;
	unsigned int bytes_read;

	unsigned int ref_count;

	ExifLog *log;
	ExifMem *mem;
};

static const unsigned char ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};

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

static unsigned int
exif_loader_copy (ExifLoader *eld, unsigned char *buf, unsigned int len)
{
	if (!eld || (len && !buf) || (eld->bytes_read >= eld->size)) return 0;

	/* If needed, allocate the buffer. */
	if (!eld->buf) eld->buf = exif_loader_alloc (eld, eld->size);
	if (!eld->buf) return 0;

	/* Copy memory */
	len = MIN (len, eld->size - eld->bytes_read);
	memcpy (eld->buf + eld->bytes_read, buf, len);
	eld->bytes_read += len;

	return (eld->bytes_read >= eld->size) ? 0 : 1;
}

unsigned char
exif_loader_write (ExifLoader *eld, unsigned char *buf, unsigned int len)
{
	unsigned int i;

	if (!eld || (len && !buf)) return 0;

	switch (eld->state) {
	case EL_EXIF_FOUND:
		return exif_loader_copy (eld, buf, len);
	case EL_SKIP_BYTES:
		if (eld->size > len) { eld->size -= len; return 1; }
		eld->state = EL_READ;
		len -= eld->size;
		buf += eld->size;
		eld->b_len = 0;
		break;
	default:
		break;
	}

	exif_log (eld->log, EXIF_LOG_CODE_DEBUG, "ExifLoader",
		  "Scanning %i byte(s) of data...", len);

	/*
	 * First fill the small buffer. Only continue if the buffer
	 * is filled. Note that EXIF data contains at least 12 bytes.
	 */
	i = MIN (len, sizeof (eld->b) - eld->b_len);
	if (i) {
		memcpy (&eld->b[eld->b_len], buf, i);
		eld->b_len += i;
		if (eld->b_len < sizeof (eld->b)) return 1;
		buf += i;
		len -= i;
	}

	/* Check the small buffer against known formats. */
	if (!memcmp (eld->b, "FUJIFILM", 8)) {
		eld->state = EL_SKIP_BYTES;
		eld->size = 112;
	} else if (!memcmp (eld->b + 2, ExifHeader, sizeof (ExifHeader))) {
		eld->state = EL_READ_SIZE_HIGH_BYTE_FINAL;
	}

	for (i = 0; i < sizeof (eld->b); i++)
		switch (eld->state) {
		case EL_EXIF_FOUND:
			if (!exif_loader_write (eld, eld->b + i,
					sizeof (eld->b) - i)) return 0;
			return exif_loader_write (eld, buf, len);
		case EL_SKIP_BYTES:
			eld->size--;
			if (!eld->size) eld->state = EL_READ;
			break;
		case EL_READ_SIZE_HIGH_BYTE:
			eld->size = eld->b[i] << 8;
			eld->state = EL_READ_SIZE_LOW_BYTE;
			break;
		case EL_READ_SIZE_HIGH_BYTE_FINAL:
			eld->size = eld->b[i] << 8;
			eld->state = EL_READ_SIZE_LOW_BYTE_FINAL;
			break;
		case EL_READ_SIZE_LOW_BYTE:
			eld->size |= eld->b[i];
			eld->state = EL_SKIP_BYTES;
			eld->size -= 2;
			break;
		case EL_READ_SIZE_LOW_BYTE_FINAL:
			eld->size |= eld->b[i];
			eld->state = EL_EXIF_FOUND;
			break;
		default:
			switch (eld->b[i]) {
			case JPEG_MARKER_APP1:
				eld->state = EL_READ_SIZE_HIGH_BYTE_FINAL;
				break;
			case JPEG_MARKER_APP0:
			case JPEG_MARKER_APP13:
			case JPEG_MARKER_COM:
				eld->state = EL_READ_SIZE_HIGH_BYTE;
				break;
			case 0xff:
			case JPEG_MARKER_SOI:
				break;
			default:
				exif_log (eld->log,
					EXIF_LOG_CODE_CORRUPT_DATA,
					"ExifLoader", _("The data supplied "
						"does not seem to contain "
						"EXIF data."));
				exif_loader_reset (eld);
				return 0;
			}
		}

	/*
	 * If we reach this point, the buffer has not been big enough
	 * to read all data we need. Fill it with new data.
	 */
	eld->b_len = 0;
	return exif_loader_write (eld, buf, len);
}

ExifLoader *
exif_loader_new (void)
{
	ExifMem *mem = exif_mem_new_default ();
	ExifLoader *l = exif_loader_new_mem (mem);

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
	loader->state = 0;
	loader->b_len = 0;
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
