#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <libexif/exif-data.h>

int
main (int argc, char **argv)
{
	ExifData *d;
	ExifMnoteData *md;
	unsigned int c, i;
	char *v;

	if (argc <= 1) {
		fprintf (stderr, "You need to supply a filename!\n");
		return 1;
	}

	fprintf (stdout, "Loading '%s'...\n", argv[1]);
	d = exif_data_new_from_file (argv[1]);
	if (!d) {
		fprintf (stderr, "Could not load data from '%s'!\n", argv[1]);
		return 1;
	}

	fprintf (stdout, "Parsing maker note...\n");
	md = exif_data_get_mnote_data (d);
	if (!md) {
		fprintf (stderr, "Could not parse maker note!\n");
		exif_data_unref (d);
		return 1;
	}

	c = exif_mnote_data_count (md);
	for (i = 0; i < c; i++) {
		fprintf (stdout, "%s", exif_mnote_data_get_name (md, i));
		fprintf (stdout, "  Title: %s",
			 exif_mnote_data_get_title (md, i));
		fprintf (stdout, "  Description: %s",
			 exif_mnote_data_get_description (md, i));
		v = exif_mnote_data_get_value (md, i);
		if (v) {
			fprintf (stdout, "  Value: '%s'", v);
			free (v);
		}
	}

	exif_data_unref (d);
}
