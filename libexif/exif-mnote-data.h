/*! \file exif-mnote-data.h
 *  \brief Handling EXIF MakerNote tags
 */
/*
 * Copyright (c) 2003 Lutz Mueller <lutz@users.sourceforge.net>
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

#ifndef __EXIF_MNOTE_DATA_H__
#define __EXIF_MNOTE_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libexif/exif-log.h>

/*! Data found in the MakerNote tag */
typedef struct _ExifMnoteData ExifMnoteData;

void exif_mnote_data_ref   (ExifMnoteData *);
void exif_mnote_data_unref (ExifMnoteData *);

void exif_mnote_data_load (ExifMnoteData *, const unsigned char *,
			   unsigned int);
void exif_mnote_data_save (ExifMnoteData *, unsigned char **, unsigned int *);

/*! Return the number of tags in the MakerNote.
 * \param[in] d MakerNote data
 * \return number of tags, or 0 if no MakerNote or the type is not supported
 */
unsigned int exif_mnote_data_count           (ExifMnoteData *d);

/*! Return the MakerNote tag number for the tag at the specified index within
 * the MakerNote.
 * \param[in] d MakerNote data
 * \param[in] n index of the entry within the MakerNote data
 * \return MakerNote tag number
 */
unsigned int exif_mnote_data_get_id          (ExifMnoteData *d, unsigned int n);

/*! Returns textual name of the given MakerNote tag. The name is a short,
 * unique (within this type of MakerNote), non-localized text string
 * containing only US-ASCII alphanumeric characters.
 * \param[in] d MakerNote data
 * \param[in] n tag number within the namespace of this type of MakerNote
 * \return textual name of the tag
 */
const char  *exif_mnote_data_get_name        (ExifMnoteData *d, unsigned int n);

/*! Returns textual title of the given MakerNote tag.
 * \param[in] d MakerNote data
 * \param[in] n tag number within the namespace of this type of MakerNote
 * \return textual name of the tag
 */
const char  *exif_mnote_data_get_title       (ExifMnoteData *d, unsigned int n);

/*! Returns verbose textual description of the given MakerNote tag.
 * \param[in] d MakerNote data
 * \param[in] n tag number within the namespace of this type of MakerNote
 * \return textual description of the tag
 */
const char  *exif_mnote_data_get_description (ExifMnoteData *d, unsigned int n);

/*! Return a textual representation of the value of the MakerNote entry.
 *
 * CAUTION: The character set of the returned string may be in
 *          the encoding of the current locale or the native encoding
 *          of the camera.
 *
 * \param[in] d MakerNote data
 * \param[in] n tag number within the namespace of this type of MakerNote
 * \param[out] val buffer in which to store value
 * \param[in] maxlen length of the buffer val
 * \return val pointer, or NULL on error
 */
char  *exif_mnote_data_get_value (ExifMnoteData *d, unsigned int n, char *val, unsigned int maxlen);

void exif_mnote_data_log (ExifMnoteData *, ExifLog *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_MNOTE_DATA_H__ */
