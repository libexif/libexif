/*! \file exif.h exif/exif.h
 * \brief Dummy header file for documentation purposes
 * \date 2007
 * \author Hans Ulrich Niedermann, et. al.
 *
 * \mainpage The libexif library
 *
 * \section general_notes General Notes
 *
 * This documentation is work in progress, as is the code itself.
 *
 * \section using_libexif Using libexif
 *
 * ##include <libexif/exif-data.h>
 *
 * libexif provides a libexif.pc file for use with pkgconfig on the
 * libexif installation. If you are using libtool to build your
 * package, you can also integrate make use of libexif-uninstalled.pc.
 *
 * \section string_conventions String Conventions
 *
 * Strings of 8 bit characters ("char*"). When libexif is compiled with
 * NLS, character set and encoding are as set in the current locale,
 * except for strings that come directly from the data in EXIF
 * tags which are returned in raw form.
 *
 * \section memory_management Memory Management Patterns
 *
 * For pointers to data objects, libexif uses reference counting. The
 * pattern is to use the foo_new() function to create a data object,
 * foo_ref() to increase the reference counter, and foo_unref() to
 * decrease the reference counter and possibly free(3)ing the memory.
 *
 * Libexif by default relies on the calloc(3), realloc(3), and free(3)
 * functions, but the libexif user can tell libexif to use their
 * special memory management functions at runtime.
 * 
 * \section thread_safety Thread Safety
 * 
 * libexif is thread safe when the underlying C library is also thread safe.
 * Some libraries may require defining a special macro (like _REENTRANT)
 * to ensure this, or may require linking to a special thread-safe version of
 * the library.
 *
 * The programmer must ensure that each object allocated by libexif is only
 * used in a single thread at once. For example, an ExifData* allocated
 * in one thread can't be used in a second thread if there is any chance
 * that the first thread could use it at the same time. Two threads
 * can use libexif without issues if they never share handles.
 *
 */
