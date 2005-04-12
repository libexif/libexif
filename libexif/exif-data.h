/*! \file exif-data.h
 * \brief FIXME foo bar blah
 *
 * \author Lutz Müller <lutz@users.sourceforge.net>
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
 *  
 *  Foo bar blah bleh baz.
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

typedef void (* ExifDataForeachContentFunc) (ExifContent *, void *user_data);
void          exif_data_foreach_content (ExifData *data,
					 ExifDataForeachContentFunc func,
					 void *user_data);

/* For debugging purposes and error reporting */
void exif_data_dump (ExifData *data);
void exif_data_log  (ExifData *data, ExifLog *log);

/* For your convenience */
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
