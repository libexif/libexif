/* test-mem.c
 *
 * Copyright (C) 2002 Lutz Müller <lutz@users.sourceforge.net>
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

#include <config.h>

#include <stdio.h>

#include <libexif/exif-data.h>
#include <libexif/exif-ifd.h>

int
main (int argc, char **argv)
{
	ExifData *ed;
	ExifEntry *e;
	unsigned char *eb;
	unsigned int ebs;

	printf ("Creating EXIF data...\n");
	ed = exif_data_new ();

	printf ("Creating EXIF entry...\n");
	e = exif_entry_new ();
	exif_content_add_entry (ed->ifd[EXIF_IFD_0], e);
	exif_entry_initialize (e, EXIF_TAG_EXIF_VERSION);

	printf ("Creating another EXIF entry...\n");
	e = exif_entry_new ();
	exif_content_add_entry (ed->ifd[EXIF_IFD_0], e);
	exif_entry_initialize (e, EXIF_TAG_DATE_TIME);

	printf ("Creating an EXIF entry in the EXIF IFD...\n");
	e = exif_entry_new ();
	exif_content_add_entry (ed->ifd[EXIF_IFD_EXIF], e);
	exif_entry_initialize (e, EXIF_TAG_FLASH_PIX_VERSION);

	exif_data_dump (ed);

	printf ("Saving EXIF data to memory...\n");
	exif_data_save_data (ed, &eb, &ebs);
	exif_data_unref (ed);

	printf ("Loading EXIF data from memory...\n");
	ed = exif_data_new_from_data (eb, ebs);
	exif_data_dump (ed);
	exif_data_unref (ed);

	return 0;
}
