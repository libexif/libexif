# This makefile is made for OpenWatcom/NT and runs on my system.
# It should not be too hard to make it run on other systems, too.
#
# FIRST, rename libexif\_stdint_watcom.h to libexif\_stdint.h 
#
# WARNING: there as no care been taken that the depencencies are
# correct. This should be fixed in the future. Currently, this makefile
# builds only, so after critical changes to the code, you should type
# "wmake clean" before rebuilding. Sorry...
#
# WARNING: This makefile might run on your system, however, I made it
# mainly for myself and don't want to waste my time to make it look "nice".
#
# WARNING: I used another makefile as template, so some things might be
# thrown away. Don't hesitate to improve this makefile!
#
# LICENSE: no restrictions at all, but USE ON YOUR OWN RISK ONLY.
#
# USAGE: Simply type "wmake -f Makefile.wat" or
#        rename "Makefile.wat" to "Makefile" and type "wmake"
#
#   Angela Wrobel <http://www.wrobelnet.de/>


# Uncomment line for desired system
#SYSTEM=DOS
#SYSTEM=OS2
SYSTEM=NT

# The name of your C compiler:
CC= wcl386

# We're using similar constants like wxWidgets
!ifeq FINAL 1
OPTFLAGS= -5r -zp8 -otexan
!else
OPTFLAGS= -5r -od -d2
!endif

LIBEXIFDIR=libexif
TESTEXIFDIR=test

IFLAGS= -i=.
WINVERFLAGS=
EXTRACPPFLAGS=-dEXIF_DONT_CHANGE_MAKER_NOTE
DEBUGCFLAGS=
EXTRACFLAGS=
OUTPUTDIR=$(LIBEXIFDIR)
CPPFLAGS = /dWIN32 /bm /fo=$(OUTPUTDIR)\ /fr -zq $(IFLAGS) $(OPTFLAGS) $(WINVERFLAGS) $(EXTRACPPFLAGS)

# zm and zv as well as the linker options below are used to make the resulting
# .exe smaller
CFLAGS = $(CPPFLAGS) $(DEBUGCFLAGS) $(EXTRACFLAGS) /zm

# Link-time cc options:
!ifeq SYSTEM DOS
LDFLAGS= -zq -l=dos4g
!else ifeq SYSTEM OS2
LDFLAGS= -zq -l=os2v2
!else ifeq SYSTEM NT
LDFLAGS= -zq -l=nt
!endif

# End of configurable options.



LIBOBJECTS = &
	$(LIBEXIFDIR)\exif-byte-order.obj $(LIBEXIFDIR)\exif-content.obj &
	$(LIBEXIFDIR)\exif-data.obj $(LIBEXIFDIR)\exif-entry.obj &
	$(LIBEXIFDIR)\exif-format.obj $(LIBEXIFDIR)\exif-ifd.obj &
	$(LIBEXIFDIR)\exif-loader.obj $(LIBEXIFDIR)\exif-log.obj &
	$(LIBEXIFDIR)\exif-mnote-data.obj $(LIBEXIFDIR)\exif-tag.obj &
        $(LIBEXIFDIR)\exif-utils.obj &
	$(LIBEXIFDIR)\exif-mnote-data-olympus.obj &
	$(LIBEXIFDIR)\mnote-olympus-entry.obj &
	$(LIBEXIFDIR)\mnote-olympus-tag.obj &
	$(LIBEXIFDIR)\exif-mnote-data-pentax.obj &
	$(LIBEXIFDIR)\mnote-pentax-entry.obj &
	$(LIBEXIFDIR)\mnote-pentax-tag.obj &
	$(LIBEXIFDIR)\exif-mnote-data-canon.obj &
	$(LIBEXIFDIR)\mnote-canon-entry.obj &
	$(LIBEXIFDIR)\mnote-canon-tag.obj 


#CFLAGS = /dWIN32 /bm /fr -zq -i=. -5r -od -d2 /d2 /zm /fo=$(LIBEXIFDIR)\

all : libexif.lib test-mem.exe test-mnote.exe test-value.exe
#	test-tree.exe 

libexif.lib: $(LIBOBJECTS)
	- del libexif.lib
	* wlib -n libexif.lib $(LIBOBJECTS)

$(LIBEXIFDIR)\exif-byte-order.obj : $(LIBEXIFDIR)\exif-byte-order.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-content.obj : $(LIBEXIFDIR)\exif-content.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-data.obj : $(LIBEXIFDIR)\exif-data.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-entry.obj : $(LIBEXIFDIR)\exif-entry.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-format.obj : $(LIBEXIFDIR)\exif-format.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-ifd.obj : $(LIBEXIFDIR)\exif-ifd.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-loader.obj : $(LIBEXIFDIR)\exif-loader.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-log.obj : $(LIBEXIFDIR)\exif-log.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-mnote-data.obj : $(LIBEXIFDIR)\exif-mnote-data.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-tag.obj : $(LIBEXIFDIR)\exif-tag.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-utils.obj : $(LIBEXIFDIR)\exif-utils.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-mnote-data-olympus.obj : $(LIBEXIFDIR)\olympus\exif-mnote-data-olympus.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\mnote-olympus-entry.obj : $(LIBEXIFDIR)\olympus\mnote-olympus-entry.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\mnote-olympus-tag.obj : $(LIBEXIFDIR)\olympus\mnote-olympus-tag.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-mnote-data-pentax.obj : $(LIBEXIFDIR)\pentax\exif-mnote-data-pentax.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\mnote-pentax-entry.obj : $(LIBEXIFDIR)\pentax\mnote-pentax-entry.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\mnote-pentax-tag.obj : $(LIBEXIFDIR)\pentax\mnote-pentax-tag.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\exif-mnote-data-canon.obj : $(LIBEXIFDIR)\canon\exif-mnote-data-canon.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\mnote-canon-entry.obj : $(LIBEXIFDIR)\canon\mnote-canon-entry.c
	$(CC) -c $(CFLAGS) $[*  

$(LIBEXIFDIR)\mnote-canon-tag.obj : $(LIBEXIFDIR)\canon\mnote-canon-tag.c
	$(CC) -c $(CFLAGS) $[*  


test-mem.exe : $(TESTEXIFDIR)\test-mem.c
	$(CC) $(CFLAGS) $(LDFRLAGS) $< libexif.lib

test-mnote.exe : $(TESTEXIFDIR)\test-mnote.c
	$(CC) $(CFLAGS) $(LDFRLAGS) $< libexif.lib

test-tree.exe : $(TESTEXIFDIR)\test-tree.c
	$(CC) $(CFLAGS) $(LDFRLAGS) $< libexif.lib

test-value.exe : $(TESTEXIFDIR)\test-value.c
	$(CC) $(CFLAGS) $(LDFRLAGS) $< libexif.lib


clean: .SYMBOLIC
	- del $(LIBEXIFDIR)\*.obj
	- del *.exe
	- del libexif.lib

