/* actions.c
 *
 * Copyright (c) 2002-2008 Lutz Mueller <lutz@users.sourceforge.net>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "exif-ifd.h"
#include "exif-actions.h"
#include "i18n.h"

#define CN(s) ((s) ? (s) : "(NULL)")

#define TAG_VALUE_BUF 1024

#define SEP " "
typedef void (* ExifContentForeachEntryFuncExt) (ExifEntry *, void *user_data, int is_last);
typedef void (* ExifDataForeachContentFuncExt) (ExifContent *, void *user_data, int is_last);

/*!
 * Replace characters which are invalid in an XML tag with safe characters
 * in place.
 */
static inline void remove_bad_chars(char *s)
{
	while (*s) {
		if ((*s == '(') || (*s == ')') || (*s == ' '))
			*s = '_';
		++s;
	}
}

// TODO move to libexif
static void
exif_content_foreach_entry_ext (ExifContent *content,
                            ExifContentForeachEntryFuncExt func, void *data, int is_last)
{
    unsigned int i;

    if (!content || !func)
        return;

    for (i = 0; i < content->count; i++) {
        func (content->entries[i], data, i == content->count - 1 && is_last);
    }
}

// TODO refactor: while this works it really could be better
// TODO move to libexif
static void
exif_data_foreach_content_ext (ExifData *data, ExifDataForeachContentFuncExt func,
                           void *user_data)
{
    unsigned int i, j, total;

    if (!data || !func)
        return;

    total = 0;
    for (i = 0; i < EXIF_IFD_COUNT; i++) {
        total += data->ifd[i]->count;
    }

    j = 0;
    for (i = 0; i < EXIF_IFD_COUNT; i++) {
        j +=  data->ifd[i]->count;
        func (data->ifd[i], user_data, j >= total);
    }
}

static void
show_entry_json (ExifEntry *e, void *data, int is_last)
{
	ExifParams* p = (ExifParams*) data;
	if (!p || !p->fp)
		return;

    char v[TAG_VALUE_BUF];

	fprintf (p->fp, "\t\"%s\": \"", exif_tag_get_name(e->tag));
	fprintf (p->fp, "%s", exif_entry_get_value (e, v, sizeof (v)));
	fprintf (p->fp, "\"");
	if (!is_last)
		fprintf (p->fp, ",");
	fprintf (p->fp, "\n");
}

static void
show_json (ExifContent *content, void *data, int is_last)
{
    exif_content_foreach_entry_ext (content, show_entry_json, data, is_last);
}

void exif_action_dump_json (ExifData *ed, ExifParams p)
{
    if (!ed || ! p.fp) return;

    fprintf(p.fp, "{\n");
    /* Show contents of all IFDs */
	exif_data_foreach_content_ext (ed, show_json, &p);
    fprintf(p.fp, "}\n");
}


/*!
 * Escape any special XML characters in the text and return a new static string
 * buffer.
 */
static const char *
escape_xml(const char *text)
{
	static char *escaped;
	static size_t escaped_size;
	char *out;
	size_t len;

	for (out=escaped, len=0; *text; ++len, ++out, ++text) {
		/* Make sure there's plenty of room for a quoted character */
		if ((len + 8) > escaped_size) {
			char *bigger_escaped;
			escaped_size += 128;
			bigger_escaped = realloc(escaped, escaped_size);
			if (!bigger_escaped) {
				free(escaped);	/* avoid leaking memory */
				escaped = NULL;
				escaped_size = 0;
				/* Error string is cleverly chosen to fail XML validation */
				return ">>> out of memory <<<";
			}
			out = bigger_escaped + len;
			escaped = bigger_escaped;
		}
		switch (*text) {
			case '&':
				strcpy(out, "&amp;");
				len += strlen(out) - 1;
				out = escaped + len;
				break;
			case '<':
				strcpy(out, "&lt;");
				len += strlen(out) - 1;
				out = escaped + len;
				break;
			case '>':
				strcpy(out, "&gt;");
				len += strlen(out) - 1;
				out = escaped + len;
				break;
			default:
				*out = *text;
				break;
		}
	}
	*out = '\x0';  /* NUL terminate the string */
	return escaped;
}

static void
show_entry_xml (ExifEntry *e, void *recvd_data)
{
	ExifParams *data = (ExifParams*)recvd_data;
	char v[TAG_VALUE_BUF];

	if (data && data->fp) {
		fprintf (data->fp, "<%s>", exif_tag_get_name(e->tag));
		fprintf (data->fp, "%s", escape_xml(exif_entry_get_value (e, v, sizeof (v))));
		fprintf (data->fp, "</%s>\n", exif_tag_get_name(e->tag));
	}
}

static void
show_xml (ExifContent *content, void *data)
{
	exif_content_foreach_entry (content, show_entry_xml, data);
}

void
exif_action_dump_xml (ExifData *ed, ExifParams p)
{
	if (!ed || !p.fp)
		return;

	fprintf(p.fp, "<exif>\n");
	exif_data_foreach_content (ed, show_xml, (void*) &p);
	fprintf(p.fp, "</exif>\n");
}