CC=gcc

CFLAGS=-g -D_REENTRANT
LD=ld
AR=ar

OBJS=exif.o

all: libexif.a exif

libexif.a: $(OBJS)
	$(AR) r $@ $^

exif: main.o libexif.a
	$(CC) -o $@ main.o -L. -lexif -lm



