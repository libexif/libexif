/*

Copyright (c) 2000 Curtis Galloway

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

$Id$

*/

#include <stdio.h>
#include "exif.h"

static void
usage(void)
{
  fprintf(stderr, "Usage: exif <file> [<tag> ...]\n");
}

static void
print_record(exif_record_t *rec)
{
  printf("%s: ", rec->rec_name);
  switch (rec->rec_type) {
  case 's':
    printf("\"%s\"", rec->rec_data.s);
    break;
  case 'f':
    printf("%f", rec->rec_data.f);
    break;
  case 'g':
    printf("%g", rec->rec_data.f);
    break;
  case 'l':
    printf("%ld", rec->rec_data.l);
    break;
  case 'r':
    printf("%ld/%ld", rec->rec_data.r.num,
	   rec->rec_data.r.denom);
    break;
  }
  printf("\n");
}


main(int argc, char **argv)
{
  int ret;
  exif_data_t *d;
  exif_record_t *rec;
  int i;

  exif_init(NULL, NULL, NULL);
  if (argc < 1) {
    usage();
    exit(1);
  }
  d = exif_parse_file(argv[1]);
  if (d == NULL) {
    printf("Null returned.\n");
    exit(1);
  }
  if (argc > 2) {
    for (i=2; i<argc; i++) {
      rec = exif_find_record(d, argv[i]);
      if (rec) {
	print_record(rec);
      }
    }
  } else {
    for (i=0; i<d->n_recs; i++) {
      print_record(&d->recs[i]);
    }
  }
  exif_free_data(d);
  return 0;
}
