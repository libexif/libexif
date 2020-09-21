//
// Created by hlewin on 9/21/20.
//

#ifndef LIBEXIF_GPS_IFD_GPS_IFD_H
#define LIBEXIF_GPS_IFD_GPS_IFD_H

#include <stdint.h>
#include <libexif/exif-format.h>
#include <libexif/exif-tag.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ExifGPSIfdTagInfo {

  /* The ExifTag this entry describes */
  uint16_t tag;

  /* The format of the tag (following the spec) */
  ExifFormat format;

  /* The expected number of components. Note for some ASCII values without a default this is indicated as 0 */
  uint16_t components;

  /* The size (in bytes) of the raw default value (or 0) */
  uint16_t default_size;

  /* A pointer to the default value. Using char* works here as there are only defaults for BYTE and ASCII values */
  const char *default_value;

} ExifGPSIfdTagInfo;


const ExifGPSIfdTagInfo* exif_get_gps_tag_info(ExifTag tag);


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif //LIBEXIF_GPS_IFD_GPS_IFD_H
