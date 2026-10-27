/* Regression test for repeated recursive loading of pointer-only IFDs. */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "libexif/exif-data.h"
#include "libexif/exif-log.h"

#define EXIF_PTR 0x8769
#define GPS_PTR 0x8825
#define INTEROP_PTR 0xa005
#define FANOUT 2
#define IFD_SIZE (2 + FANOUT * 12 + 4)
#define TIFF_SIZE (8 + 4 * IFD_SIZE)
#define EXIF_SIZE (6 + TIFF_SIZE)

static unsigned int ifd_load_count;

static void put16(unsigned char *p, unsigned int v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
}

static void put32(unsigned char *p, unsigned int v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

static void write_pointer_ifd(unsigned char *tiff, unsigned int offset,
                              unsigned int tag, unsigned int target)
{
    unsigned int i;
    unsigned char *p = tiff + offset;

    put16(p, FANOUT);
    p += 2;

    for (i = 0; i < FANOUT; ++i, p += 12) {
        put16(p, tag);
        put16(p + 2, 4); /* LONG */
        put32(p + 4, 1);
        put32(p + 8, target);
    }

    put32(p, 0); /* no next IFD */
}

static void log_func(ExifLog *log, ExifLogCode code, const char *domain,
                     const char *format, va_list args, void *data)
{
    (void)log;
    (void)code;
    (void)domain;
    (void)args;
    (void)data;

    if (!strcmp(format, "Loading %hu entries..."))
        ++ifd_load_count;
}

int main(void)
{
    unsigned char exif[EXIF_SIZE] = {0};
    unsigned char *tiff = exif + 6;
    const unsigned int ifd0 = 8;
    const unsigned int ifd_exif = ifd0 + IFD_SIZE;
    const unsigned int ifd_gps = ifd_exif + IFD_SIZE;
    const unsigned int ifd_interop = ifd_gps + IFD_SIZE;
    ExifData *data;
    ExifLog *log;

    memcpy(exif, "Exif\0\0", 6);
    memcpy(tiff, "II", 2);
    put16(tiff + 2, 42);
    put32(tiff + 4, ifd0);

    write_pointer_ifd(tiff, ifd0, EXIF_PTR, ifd_exif);
    write_pointer_ifd(tiff, ifd_exif, GPS_PTR, ifd_gps);
    write_pointer_ifd(tiff, ifd_gps, INTEROP_PTR, ifd_interop);
    write_pointer_ifd(tiff, ifd_interop, EXIF_PTR, ifd_exif);

    data = exif_data_new();
    log = exif_log_new();
    if (!data || !log)
        return 2;

    exif_log_set_func(log, log_func, NULL);
    exif_data_log(data, log);
    exif_data_load_data(data, exif, sizeof(exif));

    exif_data_unref(data);
    exif_log_unref(log);

    if (ifd_load_count != 4) {
        fprintf(stderr, "expected 4 unique IFD loads, got %u\n", ifd_load_count);
        return 1;
    }

    return 0;
}
