TOPDIR=../..
include $(TOPDIR)/pre.mk

CSOURCES=loaders_suite.c PNG.c PBM.c PGM.c PPM.c ZIP.c GIF.c IO.c PNM.c PCX.c\
         JPG.c Loader.c DataStorage.c Exif.c line_convert.c

GENSOURCES=SaveLoad.gen.c SaveAbort.gen.c

APPS=loaders_suite PNG PBM PGM PPM PNM SaveLoad.gen SaveAbort.gen ZIP GIF PCX\
     IO JPG Loader DataStorage Exif line_convert

include ../tests.mk

include $(TOPDIR)/gen.mk
include $(TOPDIR)/app.mk
include $(TOPDIR)/post.mk
