/* exif-olympus-entry.c
 *
 * Copyright (C) 2002 Lutz Müller <lutz@users.sourceforge.net>
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
#include "exif-olympus-entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libexif/exif-format.h>
#include <libexif/exif-utils.h>

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

struct _ExifOlympusEntryPrivate {
	unsigned int ref_count;
};

ExifOlympusEntry *
exif_olympus_entry_new (void)
{
	ExifOlympusEntry *entry;

	entry = malloc (sizeof (ExifOlympusEntry));
	if (!entry)
		return (NULL);
	memset (entry, 0, sizeof (ExifOlympusEntry));

	entry->priv = malloc (sizeof (ExifOlympusEntry));
	if (!entry->priv) {
		exif_olympus_entry_free (entry);
		return (NULL);
	}
	memset (entry->priv, 0, sizeof (ExifOlympusEntryPrivate));

	entry->priv->ref_count = 1;

	return (entry);
}

void
exif_olympus_entry_ref (ExifOlympusEntry *entry)
{
	if (!entry || !entry->priv)
		return;

	entry->priv->ref_count++;
}

void
exif_olympus_entry_unref (ExifOlympusEntry *entry)
{
	if (!entry || !entry->priv)
		return;

	entry->priv->ref_count--;
	if (!entry->priv->ref_count)
		exif_olympus_entry_free (entry);
}

void
exif_olympus_entry_free (ExifOlympusEntry *entry)
{
	if (!entry)
		return;

	if (entry->priv) {
		free (entry->priv);
		entry->priv = NULL;
	}

	if (entry->data) {
		free (entry->data);
		entry->data = NULL;
	}

	free (entry);
}

#define CF(format,target,v)                                     \
{                                                               \
        if (format != target) {                                 \
                snprintf (v, sizeof (v),                        \
                        _("Invalid format '%s', "               \
                        "expected '%s'."),                      \
                        exif_format_get_name (format),          \
                        exif_format_get_name (target));         \
                break;                                          \
        }                                                       \
}

#define CC(number,target,v)                                             \
{                                                                       \
        if (number != target) {                                         \
                snprintf (v, sizeof (v),                                \
                        _("Invalid number of components (%i, "          \
                        "expected %i)."), (int) number, (int) target);  \
                break;                                                  \
        }                                                               \
}

#define EXIF_FORMAT_SSHORT 8

char *
exif_olympus_entry_get_value (ExifOlympusEntry *entry)
{
	char v[1024], buf[1024];
	ExifByteOrder order;
	ExifLong vl;
	ExifShort vs;

	if (!entry)
		return (NULL);
	if (!entry->parent)
		return (NULL);

	memset (v, 0, sizeof (v));
	order = exif_note_get_byte_order ((ExifNote *) &entry->parent->parent);
	switch (entry->tag) {
	case EXIF_OLYMPUS_TAG_MODE:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 3, v);
		strncpy (v, _("Speed/Sequence/Panorama direction: "),
			 sizeof (v));
		vl = exif_get_long (entry->data, order);
		switch (vl) {
		case 0:
			strncat (v, _("normal"), sizeof (v));
			break;
		case 1:
			strncat (v, _("unknown"), sizeof (v));
			break;
		case 2:
			strncat (v, _("fast"), sizeof (v));
			break;
		case 3:
			strncat (v, _("panorama"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%li", vl);
			strncat (v, buf, sizeof (v));
		}
		strncat (v, "/", sizeof (v));
		vl = exif_get_long (entry->data + 4, order);
		snprintf (buf, sizeof (buf), "%li", vl);
		strncat (v, buf, sizeof (v));
		strncat (v, "/", sizeof (v));
		vl = exif_get_long (entry->data + 4, order);
		switch (vl) {
		case 1:
			strncat (v, _("left to right"), sizeof (v));
			break;
		case 2:
			strncat (v, _("right to left"), sizeof (v));
			break;
		case 3:
			strncat (v, _("bottom to top"), sizeof (v));
			break;
		case 4:
			strncat (v, _("top to bottom"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%li", vl);
			strncat (v, buf, sizeof (v));
		}
		break;
	case EXIF_OLYMPUS_TAG_QUALITY:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, order);
		strncpy (v, _("Quality: "), sizeof (v));
		switch (vs) {
		case 1:
			strncat (v, _("SQ"), sizeof (v));
			break;
		case 2:
			strncat (v, _("HQ"), sizeof (v));
			break;
		case 3:
			strncat (v, _("SHQ"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncat (v, buf, sizeof (v));
		}
		break;
	case EXIF_OLYMPUS_TAG_MACRO:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, order);
		strncpy (v, _("Macro: "), sizeof (v));
		switch (vs) {
		case 0:
			strncat (v, _("no"), sizeof (v));
			break;
		case 1:
			strncat (v, _("yes"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncat (v, buf, sizeof (v));
		}
		break;
	case EXIF_OLYMPUS_TAG_UNKNOWN_1:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		strncpy (v, _("Unknown tag."), sizeof (v));
		break;
	case EXIF_OLYMPUS_TAG_ZOOM:
		CF (entry->format, EXIF_FORMAT_SHORT, v);
		CC (entry->components, 1, v);
		vs = exif_get_short (entry->data, order);
		strncpy (v, _("Zoom: "), sizeof (v));
		switch (vs) {
		case 0:
			strncat (v, _("1x"), sizeof (v));
			break;
		case 2:
			strncat (v, _("2x"), sizeof (v));
			break;
		default:
			snprintf (buf, sizeof (buf), "%i", vs);
			strncat (v, buf, sizeof (v));
		}
		break;
	case EXIF_OLYMPUS_TAG_UNKNOWN_2:
		CF (entry->format, EXIF_FORMAT_RATIONAL, v);
		CC (entry->components, 1, v);
		break;
	case EXIF_OLYMPUS_TAG_UNKNOWN_3:
		CF (entry->format, EXIF_FORMAT_SSHORT, v);
		CC (entry->components, 1, v);
		break;
	case EXIF_OLYMPUS_TAG_VERSION:
		CF (entry->format, EXIF_FORMAT_ASCII, v);
		CC (entry->components, 5, v);
		break;
	case EXIF_OLYMPUS_TAG_INFO:
		CF (entry->format, EXIF_FORMAT_ASCII, v);
		CC (entry->components, 52, v);
		break;
	case EXIF_OLYMPUS_TAG_ID:
		CF (entry->format, EXIF_FORMAT_UNDEFINED, v);
		CC (entry->components, 32, v);
		break;
	case EXIF_OLYMPUS_TAG_UNKNOWN_4:
		CF (entry->format, EXIF_FORMAT_LONG, v);
		CC (entry->components, 30, v);
		break;
	default:
		break;
	}

	return (strdup (v));
}
