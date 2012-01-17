VERSION := 4.1.1
VERSION_MAJ := $(shell echo $(VERSION) |sed 's/\..*//')
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
	libtool -dynamic -flat_namespace -install_name libcodegen.4.1.1.dylib -lSystem -compatibility_version 4.1 -macosx_version_min 10.6 \
        -current_version 4.1.1 -o libcodegen.4.1.1.dylib -undefined suppress \
	    $(MODULES_LIB) -framework vecLib -framework Accelerate
else
	$(CXX) -shared -fPIC -Wl,-soname,$(SONAME) -o $(LIBNAME).$(VERSION) $(MODULES_LIB) -lz
endif

echoprint-codegen: $(MODULES) main.o
	$(CXX) $(MODULES) $(LDFLAGS) main.o -o ../echoprint-codegen

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
	rm -f *.so
ifeq ($(UNAME),Darwin)
	rm -f *.dylib
endif

PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include

# todo: dylib
install: all
	install -d $(DESTDIR)$(BINDIR)
	install ../echoprint-codegen $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(INCLUDEDIR)/echoprint
	install -pm 644 Codegen.h $(DESTDIR)$(INCLUDEDIR)/echoprint/
	install -d $(DESTDIR)$(LIBDIR)
	install -m 755 $(LIBNAME).$(VERSION) $(DESTDIR)$(LIBDIR)
	ln -s $(LIBNAME).$(VERSION) $(DESTDIR)$(LIBDIR)/$(SONAME)
	ln -s $(SONAME) $(DESTDIR)$(LIBDIR)/$(LIBNAME)

.PHONY: clean all libcodegen echoprint-codegen install
