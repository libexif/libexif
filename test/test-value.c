#include <libexif/exif-utils.h>
#include <libexif/exif-data.h>

#include <stdio.h>

int
main (int argc, char **argv)
{
	ExifData *d;
	ExifEntry *e;
	char v[1024];
	ExifSRational r = {1., 20.};
	unsigned int i;

	d = exif_data_new ();
	e = exif_entry_new ();
	exif_content_add_entry (d->ifd[EXIF_IFD_0], e);
	exif_entry_initialize (e, EXIF_TAG_SHUTTER_SPEED_VALUE);
	exif_set_srational (e->data, exif_data_get_byte_order (d), r);

	for (i = 30; i > 0; i--)
		printf ("Length %2i: '%s'\n", i, 
			exif_entry_get_value (e, v, i));

	exif_data_unref (d);

	return 0;
}
