# Available settings:
#
# WK_STATIC_BUILD:BOOL     if true, build a static library
#
# Adapted from the libgenieutils CMakeLists.txt file.

cmake_minimum_required(VERSION 3.7)

set(Wololokingdoms_LIBRARY wololokingdoms)
set(CMAKE_CXX_STANDARD 17)

project(libwololokingdoms)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/)

# dependencies:

if (WK_STATIC_BUILD)
  set(GENIEUTILS_STATIC_BUILD 1)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/genieutils/)

# include directories:

include_directories(include/)
include_directories(third_party/genieutils/include/)
include_directories(third_party/miniz)

if (${ICONV_FOUND})
  include(${ICONV_INCLUDE_DIR})
endif (${ICONV_FOUND})

if(NOT MSVC)
    add_definitions(-Wall -Wsign-compare)
else()
    add_definitions(/wd4244 /wd4018 /wd4267 /wd4996 /wd4800)
endif()

#------------------------------------------------------------------------------#
# Source files:
#------------------------------------------------------------------------------#
set(ALL_SRC
  base64.cpp
  md5.cpp
  string_helpers.cpp
  wkconverter.cpp
  zr_map_creator.cpp
  missing_strings.cpp
  fixes/ai900unitidfix.cpp
  fixes/berbersutfix.cpp
  fixes/burmesefix.cpp
  fixes/cuttingfix.cpp
  fixes/demoshipfix.cpp
  fixes/disablenonworkingunits.cpp
  fixes/ethiopiansfreepikeupgradefix.cpp
  fixes/feitoriafix.cpp
  fixes/hotkeysfix.cpp
  fixes/khmerfix.cpp
  fixes/malayfix.cpp
  fixes/maliansfreeminingupgradefix.cpp
  fixes/portuguesefix.cpp
  fixes/siegetowerfix.cpp
  fixes/smallfixes.cpp
  fixes/tricklebuildingfix.cpp
  fixes/vietfix.cpp
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

  INSTALL(TARGETS ${Wololokingdoms_LIBRARY}
    LIBRARY DESTINATION lib
  )

  install (
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*"
  )
endif()

target_link_libraries(${Wololokingdoms_LIBRARY})
