/* mnote-apple-entry.c
 *
 * Copyright (c) 2018 zhanwang-sky <zhanwang_sky@163.com>
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
 */

#include <config.h>
#include "mnote-apple-entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-entry.h>
#include <libexif/exif-format.h>
#include <libexif/exif-utils.h>

char *
mnote_apple_entry_get_value(MnoteAppleEntry *entry, char *v, unsigned int maxlen) {
    ExifSLong vsl = 0;

    if (!entry)
        return NULL;

    memset(v, 0, maxlen);
    maxlen--;

    switch (entry->tag) {
    case MNOTE_APPLE_TAG_HDR:
	if (entry->size < 4) return NULL;
	if (entry->format != EXIF_FORMAT_SLONG) return NULL;
	if (entry->components != 1) return NULL;

        vsl = exif_get_slong(entry->data, entry->order);
        snprintf(v, maxlen, "%d", vsl);
        break;
    default:
        snprintf(v, maxlen, "total %u unsupported data", entry->size);
        break;
    }

    return v;
}
