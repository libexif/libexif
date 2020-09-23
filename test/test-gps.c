/* test-gps.c
 *
 * Creates ExifEntries for the GPS ifd and initializes them.
 * Checks for formats and component counts.
 *
 * Copyright 2020 Heiko Lewin <hlewin@gmx.de>
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
 */

#include <libexif/exif-utils.h>
#include <libexif/exif-data.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * List of tags to test, 
 */
const uint16_t test_tags [] = {
 EXIF_TAG_GPS_VERSION_ID,
 EXIF_TAG_GPS_LATITUDE_REF,
 EXIF_TAG_GPS_LATITUDE,
 EXIF_TAG_GPS_LONGITUDE_REF,     
 EXIF_TAG_GPS_LONGITUDE,         
 EXIF_TAG_GPS_ALTITUDE_REF,      
 EXIF_TAG_GPS_ALTITUDE,          
 EXIF_TAG_GPS_TIME_STAMP,        
 EXIF_TAG_GPS_SATELLITES,        
 EXIF_TAG_GPS_STATUS,            
 EXIF_TAG_GPS_MEASURE_MODE,      
 EXIF_TAG_GPS_DOP,               
 EXIF_TAG_GPS_SPEED_REF,         
 EXIF_TAG_GPS_SPEED,             
 EXIF_TAG_GPS_TRACK_REF,         
 EXIF_TAG_GPS_TRACK,             
 EXIF_TAG_GPS_IMG_DIRECTION_REF, 
 EXIF_TAG_GPS_IMG_DIRECTION,     
 EXIF_TAG_GPS_MAP_DATUM,         
 EXIF_TAG_GPS_DEST_LATITUDE_REF, 
 EXIF_TAG_GPS_DEST_LATITUDE,     
 EXIF_TAG_GPS_DEST_LONGITUDE_REF, 
 EXIF_TAG_GPS_DEST_LONGITUDE,     
 EXIF_TAG_GPS_DEST_BEARING_REF,   
 EXIF_TAG_GPS_DEST_BEARING,       
 EXIF_TAG_GPS_DEST_DISTANCE_REF,  
 EXIF_TAG_GPS_DEST_DISTANCE,      
 EXIF_TAG_GPS_PROCESSING_METHOD,  
 EXIF_TAG_GPS_AREA_INFORMATION,   
 EXIF_TAG_GPS_DATE_STAMP,         
 EXIF_TAG_GPS_DIFFERENTIAL       
};


/*
 * Verify that the entry is properly initialized.
 */
static void check_entry_format(ExifEntry *e)
{
	switch(e->format) {
	case EXIF_FORMAT_UNDEFINED:
	case EXIF_FORMAT_ASCII:
		/* entries with ASCII or UNDEFINED format do not necessarily need to have the component count set.
		   only check here is, if component count is set, the size should match the count */
		if(e->size != e->components) {
			fprintf (stderr, "check_entry_format: Entry has bad component count or size (tag=%x)\n", e->tag);
			exit(1);
		}
		break;
		
	default:
		/* All other formats should have a nonzero component count. */
		if(!e->components) {
			fprintf (stderr, "check_entry_format: Entry should have component count set (tag=%x)\n", e->tag);
			exit(2);	
		}
		return;	
	}

	/* If a value is present the size should be set to the right value */	
	if(e->data && e->size != e->components * exif_format_get_size(e->format)) {
		fprintf (stderr, "check_entry_format: Entry has bad size (tag=%x)\n", e->tag);
		exit(6);	
	}
}

int
main ()
{
	int i;
	ExifData *data;
	ExifEntry *e;

	data = exif_data_new ();
	if (!data) {
		fprintf (stderr, "Error running exif_data_new()\n");
		exit(13);
	}

	/* Run tests */
	for (i=0; i < sizeof(test_tags)/sizeof(test_tags[0]); ++i) {
		e = exif_entry_new ();
		if (!e) {
			fprintf (stderr, "Error running exif_entry_new()\n");
			exit(13);
		}		
		exif_content_add_entry(data->ifd[EXIF_IFD_GPS], e);
		exif_entry_initialize (e, (ExifTag)test_tags[i]);
		check_entry_format(e);
		exif_content_remove_entry (data->ifd[EXIF_IFD_GPS], e);
		exif_entry_unref (e);
	}
}
