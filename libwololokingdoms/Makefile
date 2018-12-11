CC = gcc
CXX = g++
W_CC = i686-w64-mingw32-gcc
W_CXX = i686-w64-mingw32-g++

SOURCES = $(wildcard *.cpp) $(wildcard fixes/*.cpp)
CFLAGS = -std=c++17 -fPIC -I./include -I./third_party/genieutils/include
W_CFLAGS = $(CFLAGS) -I./third_party/genieutils/extern/win-iconv -DUNICODE
L_OBJECTS = $(addprefix bin/linux/intermediate/, $(SOURCES:.cpp=.o))
W_OBJECTS = $(addprefix bin/win32/intermediate/, $(SOURCES:.cpp=.o))

all: linux win32
linux: bin/linux/libwololokingdoms.so
win32: bin/win32/libwololokingdoms.dll
.PHONY: linux win32 all

# bin/linux/libwololokingdoms.a: bin/linux/intermediate/genieutils/libgenieutils.a $(L_OBJECTS)
# 	g++ -static -o $@ -L./bin/linux/intermediate/genieutils $(L_OBJECTS) -lgenieutils
bin/linux/libwololokingdoms.so: bin/linux/libgenieutils.so $(L_OBJECTS)
	$(CXX) -shared -fPIC -o $@ -L./bin/linux/ $(L_OBJECTS) -lgenieutils
# bin/win32/libwololokingdoms.lib: bin/win32 $(W_OBJECTS)
# 	i686-w64-mingw32-ar rvs $@ $(W_OBJECTS)
bin/win32/libwololokingdoms.dll: bin/win32/libgenieutils.dll $(W_OBJECTS)
	$(W_CXX) -shared -L./bin/win32/ $(W_OBJECTS) -lgenieutils

bin/linux/intermediate/%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<
bin/win32/intermediate/iconv.o: third_party/win-iconv/iconv.c
	$(W_CXX) -c $(W_CFLAGS) -o $@ $<
bin/win32/intermediate/%.o: %.cpp
	$(W_CXX) -c $(W_CFLAGS) -o $@ $<

bin/linux:
	mkdir -p bin/linux/intermediate
	mkdir -p bin/linux/intermediate/fixes
bin/win32:
	mkdir -p bin/win32/intermediate
	mkdir -p bin/win32/intermediate/fixes

bin/linux/intermediate/genieutils/Makefile:
	mkdir -p bin/linux/intermediate/genieutils
	cd bin/linux/intermediate/genieutils && \
	  cmake ../../../../third_party/genieutils
bin/linux/libgenieutils.so: bin/linux/intermediate/genieutils/Makefile
	make -C bin/linux/intermediate/genieutils
	cp bin/linux/intermediate/genieutils/libgenieutils.so bin/linux

bin/win32/intermediate/genieutils/Makefile:
	mkdir -p bin/win32/intermediate/genieutils
	cd bin/win32/intermediate/genieutils && \
	  cmake ../../../../third_party/genieutils
bin/win32/libgenieutils.dll: bin/win32/intermediate/genieutils/Makefile
	make -C bin/win32/intermediate/genieutils
	cp bin/win32/intermediate/genieutils/libgenieutils.dll bin/win32
