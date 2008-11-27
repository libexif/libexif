/*! \file exif-entry.h
 *  \brief Handling EXIF entries
 */
/*
 * Copyright (c) 2001 Lutz Mueller <lutz@users.sourceforge.net>
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

#ifndef __EXIF_ENTRY_H__
#define __EXIF_ENTRY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! Data found in one EXIF tag */
typedef struct _ExifEntry        ExifEntry;
typedef struct _ExifEntryPrivate ExifEntryPrivate;

#include <libexif/exif-content.h>
#include <libexif/exif-format.h>
#include <libexif/exif-mem.h>

/*! */
struct _ExifEntry {
        ExifTag tag;
        ExifFormat format;
        unsigned long components;

        unsigned char *data;
        unsigned int size;

	/* Content containing this entry */
	ExifContent *parent;

	ExifEntryPrivate *priv;
};

/* Lifecycle */

/*! Reserve memory for and initialize new #ExifEntry.
 * \return new allocated #ExifEntry
 */
ExifEntry  *exif_entry_new     (void);

ExifEntry  *exif_entry_new_mem (ExifMem *);

/*! Increase reference counter for #ExifEntry.
 *
 * \param[in] entry #ExifEntry
 */
void        exif_entry_ref     (ExifEntry *entry);

/*! Decrease reference counter for #ExifEntry.
 * When the reference count drops to zero, free the entry.
 *
 * \param[in] entry #ExifEntry
 */
void        exif_entry_unref   (ExifEntry *entry);

/*! Actually free the #ExifEntry.
 *
 * \deprecated Should not be called directly. Use #exif_entry_ref and
 *             #exif_entry_unref instead.
 *
 * \param[in] entry EXIF entry
 */
void        exif_entry_free  (ExifEntry *entry);

/*! Initialize an empty #ExifEntry with default data in the correct format
 * for the given tag. If the entry is already initialized, this function
 * does nothing.
 *
 * \param[out] e entry to initialize
 * \param[in] tag tag number to initialize as
 */
void        exif_entry_initialize (ExifEntry *e, ExifTag tag);

/*! When necessary and possible, fix the type or format of the given
 * EXIF entry when it is of the wrong type or in an invalid format.
 *
 * \param[in,out] entry EXIF entry
 */
void        exif_entry_fix        (ExifEntry *entry);

/* For your convenience */

/*! Return a textual representation of the value of the EXIF entry.
 *
 * \warning The character set of the returned string may be in
 *          the encoding of the current locale or the native encoding
 *          of the camera.
 *
 * \param[in] entry EXIF entry
 * \param[out] val buffer in which to store value
 * \param[in] maxlen length of the buffer val
 * \return val pointer
 */
const char *exif_entry_get_value (ExifEntry *entry, char *val,
				  unsigned int maxlen);

/*! Dump text representation of #ExifEntry to stdout.
 * This is intended for diagnostic purposes only.
 *
 * \param[in] entry EXIF tag data
 * \param[in] indent how many levels deep to indent the data
 */
void        exif_entry_dump      (ExifEntry *entry, unsigned int indent);

/*! Returns the IFD number of the given #ExifEntry
 *
 * \param[in] e an #ExifEntry*
 * \return #ExifIfd, or #EXIF_IFD_COUNT on error
 */
#define exif_entry_get_ifd(e) ((e)?exif_content_get_ifd((e)->parent):EXIF_IFD_COUNT)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_ENTRY_H__ */
