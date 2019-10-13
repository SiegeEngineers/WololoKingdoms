# Available settings:
#
# WK_STATIC_BUILD:BOOL     if true, build a static library
#
# Adapted from the libgenieutils CMakeLists.txt file.

cmake_minimum_required(VERSION 3.9.4)

set(Wololokingdoms_LIBRARY wololokingdoms)
set(CMAKE_CXX_STANDARD 17)

project(libwololokingdoms)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/)

if (CMAKE_BUILD_TYPE EQUAL RELEASE)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ENABLE_LTO OUTPUT error)
endif()

# dependencies:

if (WK_STATIC_BUILD)
  set(GENIEUTILS_STATIC_BUILD 1)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/genieutils/)

# include directories:

include_directories(include/)
include_directories(third_party/genieutils/include/)
include_directories(third_party/miniz)
if (WIN32)
  add_definitions(-DICONV_SECOND_ARGUMENT_IS_CONST)
  include_directories(third_party/genieutils/extern/win-iconv)
endif()

if (${ICONV_FOUND})
    include(${ICONV_INCLUDE_DIR})
endif (${ICONV_FOUND})

if(NOT MSVC)
    add_definitions(-Wall -Wsign-compare -Wextra -Wpedantic)
else()
    add_definitions(/wd4244 /wd4018 /wd4267 /wd4996 /wd4800)
endif()

# check c++17 filesystem support
# (mingw's windows header has a bug and msvc doesn't have it)
include(HasWorkingFilesystem)
set(CMAKE_REQUIRED_FLAGS)
set(CMAKE_REQUIRED_LIBRARIES)
if (NOT WORKING_STD_FS)
  message(WARNING "builtin filesystem is bad, falling back to gulrak/filesystem")
    include_directories(third_party/filesystem/include/)
    add_definitions(-DUSE_USERLAND_FS)
endif()

if (WIN32)
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
endif()

#------------------------------------------------------------------------------#
# Source files:
#------------------------------------------------------------------------------#
set(ALL_SRC
    libwololokingdoms.cpp
    fs.cpp
    base64.cpp
    md5.cpp
    wk_xml.cpp
    string_helpers.cpp
    zr_map_creator.cpp
    drs_creator.cpp
    missing_strings.cpp
    fixes/ai900unitidfix.cpp
    fixes/berbersutfix.cpp
    fixes/burmesefix.cpp
    fixes/cuttingfix.cpp
    fixes/demoshipfix.cpp
    fixes/disablenonworkingunits.cpp
    fixes/ethiopiansfreepikeupgradefix.cpp
    fixes/feitoriafix.cpp
	fixes/houseattackfix.cpp
    fixes/hotkeysfix.cpp
    fixes/khmerfix.cpp
    fixes/malayfix.cpp
    fixes/maliansfreeminingupgradefix.cpp
	fixes/nomadsfix.cpp
    fixes/portuguesefix.cpp
    fixes/siegetowerfix.cpp
	fixes/slavteambonusfix.cpp
    fixes/smallfixes.cpp
	fixes/splituniquetechicons.cpp
    fixes/tricklebuildingfix.cpp
	fixes/uprootingfix.cpp
    fixes/vietfix.cpp
    wkconverter.cpp
    third_party/miniz/miniz.c
)

#------------------------------------------------------------------------------#
# Executeable:
#------------------------------------------------------------------------------#

if (WK_STATIC_BUILD)
      message(STATUS "Building libwololokingdoms as static library")
      add_library(${Wololokingdoms_LIBRARY} STATIC ${ALL_SRC})
else()
    add_library(${Wololokingdoms_LIBRARY} SHARED ${ALL_SRC})
    target_link_libraries(${Wololokingdoms_LIBRARY} genieutils)

    install(TARGETS ${Wololokingdoms_LIBRARY}
        LIBRARY DESTINATION lib
    )

    install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*"
    )
endif()

if(ENABLE_LTO)
  message(STATUS "Building libwololokingdoms with link time optimization")
  set_property(TARGET ${Wololokingdoms_LIBRARY} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
