/*

Copyright © 2000 Curtis Galloway

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

*/

typedef struct exif_rational {
  int      num;
  unsigned denom;
} exif_rational_t;

typedef union {
  long            l;
  float           f;
  double          g;
  char *          s;
  exif_rational_t r;
} exif_rec_data_t;

typedef struct exif_record {
  char *           rec_name;
  char             rec_type;
  int              exif_format;
  exif_rec_data_t  rec_data;
} exif_record_t;

typedef struct exif_data {
  int           n_recs;
  int           n_alloc;
  exif_record_t recs[0];
} exif_data_t;


/* EXIF data formats */

#define EXIF_FMT_COMPUTED  -1 /* Not in raw data */
#define EXIF_FMT_BYTE       1 
#define EXIF_FMT_STRING     2
#define EXIF_FMT_USHORT     3
#define EXIF_FMT_ULONG      4
#define EXIF_FMT_URATIONAL  5
#define EXIF_FMT_SBYTE      6
#define EXIF_FMT_UNDEFINED  7
#define EXIF_FMT_SSHORT     8
#define EXIF_FMT_SLONG      9
#define EXIF_FMT_SRATIONAL 10
#define EXIF_FMT_SINGLE    11
#define EXIF_FMT_DOUBLE    12

extern void
exif_init(void *(*malloc_fn)(int),
	  void (*free_fn)(void *),
	  void *(*realloc_fn)(void *, int));

extern exif_data_t *
exif_parse_fd(int fd);

extern exif_data_t *
exif_parse_file(const char *filename);

extern void
exif_free_data(exif_data_t *d);

extern exif_record_t *
exif_find_record(exif_data_t *d, const char *rec_name);


