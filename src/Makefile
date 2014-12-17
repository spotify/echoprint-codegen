# Version of echoprint, as a list. Is expanded out
# for various version numbers.
EP_VERSION := 4 1 1
VERSION := $(word 1, $(EP_VERSION)).$(word 2, $(EP_VERSION)).$(word 3, $(EP_VERSION))
VERSION_MAJ := $(word 1, $(EP_VERSION))
VERSION_COMPAT := $(word 1, $(EP_VERSION)).$(word 2, $(EP_VERSION))
UNAME := $(shell uname -s)
CXX=g++
CC=gcc
#OPTFLAGS=-g -O0
OPTFLAGS=-O3 -DBOOST_UBLAS_NDEBUG -DNDEBUG
BOOST_CFLAGS=-I/usr/local/include/boost-1_35
TAGLIB_CFLAGS=`taglib-config --cflags`
TAGLIB_LIBS=`taglib-config --libs`
CXXFLAGS=-Wall $(BOOST_CFLAGS) $(TAGLIB_CFLAGS) -fPIC $(OPTFLAGS)
CFLAGS=-Wall -fPIC $(OPTFLAGS)
LDFLAGS=$(TAGLIB_LIBS) -lz -lpthread $(OPTFLAGS)
LIBNAME=libcodegen.so
SONAME=$(LIBNAME).$(VERSION_MAJ)

MODULES_LIB = \
    AudioBufferInput.o \
    AudioStreamInput.o \
    Base64.o \
    Codegen.o \
    Fingerprint.o \
    MatrixUtility.o \
    SubbandAnalysis.o \
    Whitening.o
MODULES = $(MODULES_LIB) Metadata.o

all: libcodegen echoprint-codegen

libcodegen: $(MODULES_LIB)
ifeq ($(UNAME),Darwin)
	libtool -dynamic -flat_namespace -install_name libcodegen.$(VERSION).dylib -lSystem -compatibility_version $(VERSION_COMPAT) \
		-macosx_version_min 10.6 -current_version $(VERSION) -o libcodegen.$(VERSION).dylib -undefined suppress \
	    $(MODULES_LIB) -framework Accelerate
else
	$(CXX) -shared -fPIC -Wl,-soname,$(SONAME) -o $(LIBNAME).$(VERSION) $(MODULES_LIB) -lz
endif

echoprint-codegen: $(MODULES) main.o
	$(CXX) $(MODULES) main.o $(LDFLAGS) -o ../echoprint-codegen

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cxx %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o ../echoprint-codegen
	rm -f libcodegen.so*
ifeq ($(UNAME),Darwin)
	rm -f *.dylib
endif

PREFIX ?= /usr/local
DESTDIR ?= $(PREFIX)
BINDIR ?= /bin
LIBDIR ?= /lib
INCLUDEDIR ?= /include

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	install ../echoprint-codegen $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(INCLUDEDIR)/echoprint
	install -pm 644 Codegen.h $(DESTDIR)$(INCLUDEDIR)/echoprint/
	mkdir -p $(DESTDIR)$(LIBDIR)
ifeq ($(UNAME),Darwin)
	install -m 755 libcodegen.$(VERSION).dylib $(DESTDIR)$(LIBDIR)
	ln -fs $(DESTDIR)$(LIBDIR)/libcodegen.$(VERSION) $(DESTDIR)$(LIBDIR)/libcodegen.$(VERSION_MAJ).dylib
	ln -fs $(DESTDIR)$(LIBDIR)/libcodegen.$(VERSION_MAJ).dylib $(DESTDIR)$(LIBDIR)/libcodegen.dylib
else
	install -m 755 $(LIBNAME).$(VERSION) $(DESTDIR)$(LIBDIR)
	ln -fs $(DESTDIR)$(LIBDIR)/$(LIBNAME).$(VERSION) $(DESTDIR)$(LIBDIR)/$(SONAME)
	ln -fs $(DESTDIR)$(LIBDIR)/$(SONAME) $(DESTDIR)$(LIBDIR)/$(LIBNAME)
endif

.PHONY: clean all libcodegen echoprint-codegen install
