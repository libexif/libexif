/*! \file exif-log.h
 *  \brief log message infrastructure
 *
 * Copyright (c) 2004 Lutz Mueller <lutz@users.sourceforge.net>
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

#include <libexif/exif-mem.h>
#include <stdarg.h>

typedef struct _ExifLog        ExifLog;

ExifLog *exif_log_new     (void);
ExifLog *exif_log_new_mem (ExifMem *);
void     exif_log_ref     (ExifLog *log);
void     exif_log_unref   (ExifLog *log);
void     exif_log_free    (ExifLog *log);

typedef enum {
	EXIF_LOG_CODE_NONE,
	EXIF_LOG_CODE_DEBUG,
	EXIF_LOG_CODE_NO_MEMORY,
	EXIF_LOG_CODE_CORRUPT_DATA
} ExifLogCode;
const char *exif_log_code_get_title   (ExifLogCode); /* Title for dialog   */
const char *exif_log_code_get_message (ExifLogCode); /* Message for dialog */

/** Log callback function prototype.
 */
typedef void (* ExifLogFunc) (ExifLog *log, ExifLogCode, const char *domain,
			      const char *format, va_list args, void *data);

/** Register log callback function.
 */
void     exif_log_set_func (ExifLog *log, ExifLogFunc func, void *data);

#ifndef NO_VERBOSE_TAG_STRINGS
void     exif_log  (ExifLog *log, ExifLogCode, const char *domain,
		    const char *format, ...)
#ifdef __GNUC__
			__attribute__((__format__(printf,4,5)))
#endif
;
#else
#if defined(__STDC_VERSION__) &&  __STDC_VERSION__ >= 199901L
#define exif_log(...) do { } while (0)
#elif defined(__GNUC__)
#define exif_log(x...) do { } while (0)
#else
#define exif_log (void)
#endif
#endif

void     exif_logv (ExifLog *log, ExifLogCode, const char *domain,
		    const char *format, va_list args);

/* For your convenience */
#define EXIF_LOG_NO_MEMORY(l,d,s) exif_log ((l), EXIF_LOG_CODE_NO_MEMORY, (d), "Could not allocate %lu byte(s).", (unsigned long)(s))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EXIF_LOG_H__ */
