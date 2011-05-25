UNAME := $(shell uname -s)
CXX=g++
CC=gcc
PLATFORM=`uname`
ARCH=`uname -m`
#OPTFLAGS=-g -O0
OPTFLAGS=-O3
CXXFLAGS=-Wall -I/usr/local/include/boost-1_35 -fPIC $(OPTFLAGS)
CFLAGS=-Wall -Ifft -fPIC $(OPTFLAGS)
ifeq ($(UNAME),Darwin)
	LDFLAGS=-L/usr/local/lib ../lib/libtag-$(PLATFORM)-$(ARCH).a -lz -lpthread -framework Accelerate -framework vecLib $(OPTFLAGS)
else
	LDFLAGS=-L/usr/local/lib ../lib/libtag-$(PLATFORM)-$(ARCH).a -lz -lpthread $(OPTFLAGS)
endif

MODULES_LIB = \
    AudioBufferInput.o \
    AudioStreamInput.o \
    Base64.o \
    Codegen.o \
    Fingerprint.o \
    FingerprintLowRank.o \
    MatrixUtility.o \
    Spectrogram.o \
    VectorUtility.o \
    fft/kiss_fft.o \
    fft/kiss_fftr.o \
    easyzlib.o
MODULES = $(MODULES_LIB) Metadata.o

main: $(MODULES) main.o
	$(CXX) $(MODULES) $(LDFLAGS) main.o -o ../codegen.$(PLATFORM)-$(ARCH) 
ifeq ($(UNAME),Darwin)
	$(CXX) -shared -fPIC -o libcodegen.$(PLATFORM)-$(ARCH).so $(MODULES_LIB) -framework Accelerate -framework vecLib
	libtool -dynamic -flat_namespace -install_name libcodegen.4.0.0.dylib -lSystem -compatibility_version 4.0 -macosx_version_min 10.6 \
	    -current_version 4.0.0 -o libcodegen.4.0.0.dylib -undefined suppress \
	    $(MODULES) -framework vecLib -framework Accelerate
else
	$(CXX) -shared -fPIC -o libcodegen.$(PLATFORM)-$(ARCH).so $(MODULES_LIB) 
endif

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cxx %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o ../codegen.$(PLATFORM)-$(ARCH)
	rm -f fft/*.o
	rm -f *.so
ifeq ($(UNAME),Darwin)
	rm -f *.dylib
endif
