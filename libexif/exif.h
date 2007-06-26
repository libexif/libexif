/*! \file exif.h exif/exif.h
 * \brief Dummy header file
 * \date 2007
 * \author Hans Ulrich Niedermann
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
 * Strings of 8 bit characters ("char*"). Character set and encoding
 * are currently undefined, but that will change in the future.
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
 * FIXME: Good question. Needs to be determined.
 *
 */
