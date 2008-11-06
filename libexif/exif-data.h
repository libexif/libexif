/*! \file exif-data.h
 * \brief Defines the ExifData type and the associated functions.
 *
 * \author Lutz Mueller <lutz@users.sourceforge.net>
 * \date 2001-2005
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

#ifndef __EXIF_DATA_H__
#define __EXIF_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libexif/exif-byte-order.h>
#include <libexif/exif-data-type.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-log.h>
#include <libexif/exif-tag.h>

typedef struct _ExifData        ExifData;
typedef struct _ExifDataPrivate ExifDataPrivate;

#include <libexif/exif-content.h>
#include <libexif/exif-mnote-data.h>
#include <libexif/exif-mem.h>

struct _ExifData
{
	ExifContent *ifd[EXIF_IFD_COUNT];

	unsigned char *data;
	unsigned int size;

	ExifDataPrivate *priv;
};

ExifData *exif_data_new           (void);
ExifData *exif_data_new_mem       (ExifMem *);

/*! \brief load exif data from file
 *  \param[in] path filename including path
 */
ExifData *exif_data_new_from_file (const char *path);
ExifData *exif_data_new_from_data (const unsigned char *data,
				   unsigned int size);

void      exif_data_load_data (ExifData *data, const unsigned char *d, 
			       unsigned int size);
void      exif_data_save_data (ExifData *data, unsigned char **d,
			       unsigned int *size);

void      exif_data_ref   (ExifData *data);
void      exif_data_unref (ExifData *data);
void      exif_data_free  (ExifData *data);

ExifByteOrder exif_data_get_byte_order  (ExifData *data);
void          exif_data_set_byte_order  (ExifData *data, ExifByteOrder order);

ExifMnoteData *exif_data_get_mnote_data (ExifData *);
void           exif_data_fix (ExifData *);

typedef void (* ExifDataForeachContentFunc) (ExifContent *, void *user_data);
void          exif_data_foreach_content (ExifData *data,
					 ExifDataForeachContentFunc func,
					 void *user_data);

typedef enum {
	EXIF_DATA_OPTION_IGNORE_UNKNOWN_TAGS = 1 << 0,
	EXIF_DATA_OPTION_FOLLOW_SPECIFICATION = 1 << 1,
	EXIF_DATA_OPTION_DONT_CHANGE_MAKER_NOTE = 1 << 2
} ExifDataOption;

const char *exif_data_option_get_name        (ExifDataOption);
const char *exif_data_option_get_description (ExifDataOption);
void        exif_data_set_option             (ExifData *, ExifDataOption);
void        exif_data_unset_option           (ExifData *, ExifDataOption);

void         exif_data_set_data_type (ExifData *, ExifDataType);
ExifDataType exif_data_get_data_type (ExifData *);

/* For debugging purposes and error reporting */
void exif_data_dump (ExifData *data);
void exif_data_log  (ExifData *data, ExifLog *log);

/** convenience macro. */
#define exif_data_get_entry(d,t)					\
	(exif_content_get_entry(d->ifd[EXIF_IFD_0],t) ?			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_0],t) :			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_1],t) ?			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_1],t) :			\
	 exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],t) ?		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_EXIF],t) :		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_GPS],t) ?		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_GPS],t) :		\
	 exif_content_get_entry(d->ifd[EXIF_IFD_INTEROPERABILITY],t) ?	\
	 exif_content_get_entry(d->ifd[EXIF_IFD_INTEROPERABILITY],t) : NULL)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_DATA_H__ */
