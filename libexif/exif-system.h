/*! \file exif-system.h
 * \brief System specific definitions, not for installation!
 */
/*
 * Copyright (c) 2007 Hans Ulrich Niedermann <gp@n-dimensional.de>
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
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA.
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LIBEXIF_EXIF_SYSTEM_H
#define LIBEXIF_EXIF_SYSTEM_H

#include <stddef.h>

#if defined(__GNUC__) && (__GNUC__ >= 2)
# define UNUSED(param) UNUSED_PARAM_##param __attribute__((unused))
#else
# define UNUSED(param) param
#endif

/* Not every file including this header includes config.h first */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
# define EXIF_INLINE inline
#elif defined(__GNUC__)
# define EXIF_INLINE __inline__
#elif defined(_MSC_VER)
# define EXIF_INLINE __inline
#else
# define EXIF_INLINE
#endif

/*! Check whether a structure does not fit inside a buffer.
 *
 * Each argument is evaluated exactly once and none of the arithmetic can
 * wrap around, so callers do not need to add offset and size themselves.
 *
 * \param[in] offset offset of the structure from the start of the buffer
 * \param[in] datasize total size of the buffer
 * \param[in] structsize size of the structure
 * \return 1 if [offset, offset + structsize) is not entirely within
 *   [0, datasize) or if offset is not inside the buffer, 0 otherwise
 */
static EXIF_INLINE int
exif_check_overflow (size_t offset, size_t datasize, size_t structsize)
{
	return (offset >= datasize) || (structsize > datasize) ||
	       (offset > datasize - structsize);
}

/*! Multiply two sizes, refusing any product larger than a limit.
 *
 * \param[in] a first factor
 * \param[in] b second factor
 * \param[in] max largest acceptable product
 * \param[out] res product of a and b; only written on success
 * \return 1 if a * b would be larger than max, 0 otherwise
 */
static EXIF_INLINE int
exif_check_mul_overflow (size_t a, size_t b, size_t max, size_t *res)
{
	if (a && (b > max / a))
		return 1;
	*res = a * b;
	return 0;
}

#endif /* !defined(LIBEXIF_EXIF_SYSTEM_H) */
