/*! \file exif-content.h
 *  \brief Handling EXIF IFDs
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

#ifndef __EXIF_CONTENT_H__
#define __EXIF_CONTENT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! Holds all EXIF tags in a single IFD */
typedef struct _ExifContent        ExifContent;
typedef struct _ExifContentPrivate ExifContentPrivate;

#include <libexif/exif-tag.h>
#include <libexif/exif-entry.h>
#include <libexif/exif-data.h>
#include <libexif/exif-log.h>
#include <libexif/exif-mem.h>

struct _ExifContent
{
        ExifEntry **entries;
        unsigned int count;

	/* Data containing this content */
	ExifData *parent;

	ExifContentPrivate *priv;
};

/* Lifecycle */
ExifContent *exif_content_new     (void);
ExifContent *exif_content_new_mem (ExifMem *);
void         exif_content_ref     (ExifContent *content);
void         exif_content_unref   (ExifContent *content);
void         exif_content_free    (ExifContent *content);

/*! Add an EXIF tag to an IFD.
 * If this tag already exists in the IFD, this function does nothing.
 * \param[out] c IFD
 * \param[in] entry EXIF entry to add
 */
void         exif_content_add_entry    (ExifContent *c, ExifEntry *entry);

/*! Remove an EXIF tag from an IFD.
 * If this tag does not exist in the IFD, this function does nothing.
 * \param[out] c IFD
 * \param[in] e EXIF entry to remove
 */
void         exif_content_remove_entry (ExifContent *c, ExifEntry *e);

/*! Return the #ExifEntry in this IFD corresponding to the given tag.
 * \param[in] content EXIF content for an IFD
 * \param[in] tag EXIF tag to return
 * \return #ExifEntry of the tag, or NULL on error
 */
ExifEntry   *exif_content_get_entry    (ExifContent *content, ExifTag tag);
void         exif_content_fix          (ExifContent *);

typedef void (* ExifContentForeachEntryFunc) (ExifEntry *, void *user_data);

/*! Executes function on each EXIF tag in turn.
 * \param[in] content IFD over which to iterate
 * \param[in] func function to call for each entry
 * \param[in] user_data data to pass into func on each call
 */
void         exif_content_foreach_entry (ExifContent *content,
					 ExifContentForeachEntryFunc func,
					 void *user_data);

/*! Returns the IFD number in which the given #ExifContent is found.
 * \param[in] c an #ExifContent*
 * \return IFD number, or EXIF_IFD_COUNT on error
 */
ExifIfd exif_content_get_ifd (ExifContent *c);

/*! Returns a textual representation of the EXIF data for a tag.
 * \param[in] c #ExifContent for an IFD
 * \param[in] t #ExifTag to return
 * \param[out] v buffer in which to store value
 * \param[in] m length of the buffer v
 * \return v pointer, or NULL on error
 */
#define exif_content_get_value(c,t,v,m)					\
	(exif_content_get_entry (c,t) ?					\
	 exif_entry_get_value (exif_content_get_entry (c,t),v,m) : NULL)

/*! Dump contents of the IFD to stdout.
 * This is intended for diagnostic purposes only.
 * \param[in] content IFD data
 * \param[in] indent how many levels deep to indent the data
 */
void exif_content_dump  (ExifContent *content, unsigned int indent);

/*! Set the log message object for this IFD.
 * \param[in] content IFD
 * \param[in] log #ExifLog
 */
void exif_content_log   (ExifContent *content, ExifLog *log);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_CONTENT_H__ */
