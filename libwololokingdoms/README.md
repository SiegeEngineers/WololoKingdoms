# WololoKingdoms Converter Library

This is the library code for the WololoKingdoms conversion mod. See [WololoKingdoms](https://github.com/aoe2communitygithub/wololokingdoms) for the installer application code.

## Building

The build is currently only made for linux. It's based on a Makefile right now but may be moved to CMake.

You need to have CMake installed to build genieutils.

First get the dependencies:

```bash
git submodule init
git submodule update
```

Then run `make $platform`:

```bash
make linux
# or
make win32
```

`make win32` doesn't work yet. It needs a brand new version of MinGW that contains `std::filesystem`, which hasn't been released yet.
