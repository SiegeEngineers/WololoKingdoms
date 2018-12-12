# WololoKingdoms Converter Library

This is the library code for the WololoKingdoms conversion mod. See [WololoKingdoms](https://github.com/aoe2communitygithub/wololokingdoms) for more information and for the installer application code.

## Building

The build script is currently only made for linux.

### On Linux

It's based on a Makefile right now but may be moved to CMake.

libwololokingdoms requires a C++ compiler that supports C++17 and its `<filesystem>` API.

You need to have CMake installed to build genieutils. Your package manager likely contains a `cmake` package.

First get the dependencies:

```bash
git submodule init
git submodule update
```

Then run `make $target_platform`:

```bash
make linux
# or
make win32
```

The Makefile will build libwololokingdoms, and genieutils and its dependencies. It outputs two files into `libwololokingdoms/bin/PLATFORM`: `libwololokingdoms` and `libgenieutils`.
