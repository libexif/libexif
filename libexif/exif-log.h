/* exif-log.h
 *
 * Copyright © 2004 Lutz Müller <lutz@users.sourceforge.net>
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

#ifndef __EXIF_LOG_H__
#define __EXIF_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>

typedef struct _ExifLog        ExifLog;

ExifLog *exif_log_new   (void);
void     exif_log_ref   (ExifLog *log);
void     exif_log_unref (ExifLog *log);
void     exif_log_free  (ExifLog *log);

typedef enum {
	EXIF_LOG_CODE_NONE,
	EXIF_LOG_CODE_DEBUG,
	EXIF_LOG_CODE_NO_MEMORY
} ExifLogCode;

typedef void (* ExifLogFunc) (ExifLog *log, ExifLogCode, const char *domain,
			      const char *format, va_list args, void *data);

void     exif_log_set_func (ExifLog *log, ExifLogFunc func, void *data);

void     exif_log  (ExifLog *log, ExifLogCode, const char *domain,
		    const char *format, ...);
void     exif_logv (ExifLog *log, ExifLogCode, const char *domain,
		    const char *format, va_list args);

/* For your convenience */
#define EXIF_LOG_NO_MEMORY(l,d,s) exif_log (l, EXIF_LOG_CODE_NO_MEMORY, d, "Could not allocate %i byte(s).", s)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_LOG_H__ */
