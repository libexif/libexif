/*! \file exif-loader.h
 * \brief Defines the ExifLoader type
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

#ifndef __EXIF_LOADER_H__
#define __EXIF_LOADER_H__

#include <libexif/exif-data.h>
#include <libexif/exif-log.h>
#include <libexif/exif-mem.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! Data used by the loader interface */
typedef struct _ExifLoader ExifLoader;

/*! Allocate a new #ExifLoader
 *  \return allocated ExifLoader
 */
ExifLoader *exif_loader_new     (void);

/*! Allocate a new #ExifLoader using an #ExifMem
 *  \param[in] mem the ExifMem
 *  \return allocated ExifLoader
 */
ExifLoader *exif_loader_new_mem (ExifMem *mem);

/*! Increase the refcount of the #ExifLoader
 *  \param[in] loader the ExifLoader to increase the refcount of.
 */
void        exif_loader_ref     (ExifLoader *loader);

/*! Decrease the refcount of the #ExifLoader
 *  \param[in] loader ExifLoader to decrease the refcount of.
 *  If the refcount reaches 0, the ExifLoader is freed.
 */
void        exif_loader_unref   (ExifLoader *loader);

/*! Write a file into the #ExifLoader from the filesystem.
 * \param[in] loader loader
 * \param[in] fname path to the file to read
 */
void        exif_loader_write_file (ExifLoader *loader, const char *fname);

/*! Write a buffer into the ExifLoader from a memory block.
 * \param[in] loader loader to write to
 * \param[in] buf buffer to read from
 * \param[in] sz size of the buffer
 * \return 1 while EXIF data is read (or while there is still hope that there will be EXIF data later on), 0 otherwise.
 */
unsigned char exif_loader_write (ExifLoader *loader, unsigned char *buf, unsigned int sz);

/*! Reset the ExifLoader
 * \param[in] loader the loader
 */
void          exif_loader_reset (ExifLoader *loader);

/*! Get an ExifData out of an ExifLoader
 * \param[in] loader the loader
 * \return allocated ExifData
 */
ExifData     *exif_loader_get_data (ExifLoader *loader);

void exif_loader_log (ExifLoader *, ExifLog *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_LOADER_H__ */
