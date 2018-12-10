SOURCES = wkconverter.cpp md5.cpp
BOOST = -I./third_party/boost/libs/algorithm/include \
        -I./third_party/boost/libs/assert/include \
        -I./third_party/boost/libs/concept_check/include \
        -I./third_party/boost/libs/config/include \
        -I./third_party/boost/libs/container_hash/include \
        -I./third_party/boost/libs/core/include \
        -I./third_party/boost/libs/detail/include \
        -I./third_party/boost/libs/filesystem/include \
        -I./third_party/boost/libs/io/include \
        -I./third_party/boost/libs/iterator/include \
        -I./third_party/boost/libs/mpl/include \
        -I./third_party/boost/libs/predef/include \
        -I./third_party/boost/libs/preprocessor/include \
        -I./third_party/boost/libs/range/include \
        -I./third_party/boost/libs/smart_ptr/include \
        -I./third_party/boost/libs/static_assert/include \
        -I./third_party/boost/libs/system/include \
        -I./third_party/boost/libs/throw_exception/include \
        -I./third_party/boost/libs/type_traits/include \
        -I./third_party/boost/libs/utility/include \
        -I./third_party/boost/libs/winapi/include

CFLAGS = -std=c++17 -I./include -I./third_party/genieutils/include $(BOOST)
W_CFLAGS = $(CFLAGS) -I./third_party/win-iconv -DUNICODE
L_OBJECTS = $(addprefix bin/linux/intermediate/, $(SOURCES:.cpp=.o))
W_OBJECTS = $(addprefix bin/win32/intermediate/, $(SOURCES:.cpp=.o))

all: linux win32
linux: bin/linux/libwololokingdoms.lib bin/linux/libwololokingdoms.so
win32: bin/win32/libwololokingdoms.lib bin/win32/libwololokingdoms.dll
.PHONY: linux win32 all

bin/linux/libwololokingdoms.lib: bin/linux $(L_OBJECTS)
	ar rvs $@ $(L_OBJECTS)
bin/linux/libwololokingdoms.so: bin/linux/libgenieutils.so $(L_OBJECTS)
	g++ -shared -o $@ -L./bin/linux $(L_OBJECTS) -lgenieutils
bin/win32/libwololokingdoms.lib: bin/win32 $(W_OBJECTS)
	i686-w64-mingw32-ar rvs $@ $(W_OBJECTS)
bin/win32/libwololokingdoms.dll: bin/win32 $(W_OBJECTS)
	i686-w64-mingw32-g++ -shared -L./bin/win32 $(W_OBJECTS) -lgenieutils

bin/linux/intermediate/%.o: %.cpp
	g++ -c $(CFLAGS) -o $@ $<
bin/win32/intermediate/iconv.o: third_party/win-iconv/iconv.c
	i686-w64-mingw32-gcc $(W_CFLAGS) -o $@ $<
bin/win32/intermediate/%.o: %.cpp
	i686-w64-mingw32-g++ $(W_CFLAGS) -o $@ $<

bin/linux:
	mkdir -p bin/linux/intermediate
bin/win32:
	mkdir -p bin/win32/intermediate

bin/linux/intermediate/genieutils/Makefile:
	mkdir -p bin/linux/intermediate/genieutils
	cd bin/linux/intermediate/genieutils && \
	  cmake -D GENIEUTILS_STATIC_BUILD=1 ../../../../third_party/genieutils
bin/linux/libgenieutils.so: bin/linux/intermediate/genieutils/Makefile
	make -C bin/linux/intermediate/genieutils
bin/win32/genieutils.dll:
