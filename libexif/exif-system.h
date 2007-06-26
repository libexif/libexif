/** \file exif-system.h
 * \brief System specific definitions, not for installation!
 */

#ifndef EXIF_SYSTEM_H
#define EXIF_SYSTEM_H

#if defined(__GNUC__) && (__GNUC__ >= 2)
# define UNUSED(param) UNUSED_PARAM_##param __attribute__((unused))
#else
# define UNUSED(param) param
#endif

#endif /* !defined(EXIF_SYSTEM_H) */
