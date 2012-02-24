ifndef LIBNAME
$(error LIBNAME not defined, fix your library Makefile)
endif

SWIG_SRC=$(LIBNAME).swig
SWIG_PY=$(LIBNAME)_c.py
SWIG_C=$(LIBNAME)_wrap.c
SWIG_LIB=_$(LIBNAME)_c.so

include $(TOPDIR)/config.gen.mk

ifneq ($(SWIG),)

INCLUDES+=$(addprefix -I$(TOPDIR)/include/, $(INCLUDE))

all: $(SWIG_LIB) $(SWIG_PY)

$(SWIG_C) $(SWIG_PY): $(SWIG_SRC)
ifdef VERBOSE
	$(SWIG) $(SWIGOPTS) -python $(INCLUDES) $<
else # VERBOSE
	@echo "SWIG $(LIBNAME)"
	@$(SWIG) $(SWIGOPTS) -python $(INCLUDES) $<
endif # VERBOSE

$(SWIG_LIB): $(SWIG_C)
ifdef VERBOSE
	$(CC) $< $(CFLAGS) $(LDFLAGS) -I$(PYTHON_INCLUDE) --shared -lGP -L$(TOPDIR)/build/ -o $@
else # VERBOSE
	@echo "LD  $@"
	@$(CC) $< $(CFLAGS) $(LDFLAGS) -I$(PYTHON_INCLUDE) --shared -lGP -L$(TOPDIR)/build/ -o $@
endif # VERBOSE

endif # ifneq ($(SWIG),)

CLEAN+=$(SWIG_C) $(SWIG_PY) $(SWIG_LIB)