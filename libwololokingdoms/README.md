# WololoKingdoms Converter Library

This is the library code for the WololoKingdoms conversion mod. See [WololoKingdoms](https://github.com/aoe2communitygithub/wololokingdoms) for more information and for the installer application code.

## Building

You need to have CMake installed to build libwololokingdoms and genieutils. Your package manager likely contains a `cmake` package.

libwololokingdoms requires a C++ compiler that supports C++17 and its `<filesystem>` API.

First get the dependencies:

```bash
git submodule init
git submodule update
```

Then do:

```bash
cd build
cmake ..
make
```

CMake will build libwololokingdoms, and genieutils and its dependencies. You can find the `libwololokingdoms` dynamic library in `build/` and the genieutils dynamic library in `build/third_party/genieutils`.
