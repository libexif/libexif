/* exif-mem.h
 *
 * Copyright © 2003 Lutz Müller <lutz@users.sourceforge.net>
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

#ifndef __EXIF_MEM_H__
#define __EXIF_MEM_H__

#include <libexif/exif-utils.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void * (* ExifMemAllocFunc)   (ExifLong);
typedef void * (* ExifMemReallocFunc) (void *, ExifLong);
typedef void   (* ExifMemFreeFunc)    (void *);

typedef struct _ExifMem ExifMem;

ExifMem *exif_mem_new   (ExifMemAllocFunc, ExifMemReallocFunc,
			 ExifMemFreeFunc);
void     exif_mem_ref   (ExifMem *);
void     exif_mem_unref (ExifMem *);

void *exif_mem_alloc   (ExifMem *, ExifLong);
void *exif_mem_realloc (ExifMem *, void *, ExifLong);
void  exif_mem_free    (ExifMem *, void *);

/* For your convenience */
void *exif_mem_alloc_func   (ExifLong);
void *exif_mem_realloc_func (void *, ExifLong);
void  exif_mem_free_func    (void *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_MEM_H__ */
