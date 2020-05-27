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

if (WK_STATIC_BUILD)
  # Set `GUTILS_STATIC` option for genieutils.
  set(GUTILS_STATIC ON CACHE BOOL "")
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Create library
if (WK_STATIC_BUILD)
  message(STATUS "Building libwololokingdoms as static library")
  add_library(${Wololokingdoms_LIBRARY} STATIC)
else()
  add_library(${Wololokingdoms_LIBRARY} SHARED)
endif()

target_include_directories(${Wololokingdoms_LIBRARY}
  PRIVATE include/
)

# Dependencies

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/genieutils/)

target_include_directories(${Wololokingdoms_LIBRARY}
  PRIVATE third_party/genieutils/include/
          third_party/miniz
)
if (WIN32)
  add_definitions(-DICONV_SECOND_ARGUMENT_IS_CONST)
  target_include_directories(${Wololokingdoms_LIBRARY}
    PRIVATE third_party/genieutils/extern/win-iconv
  )
endif()

if (${ICONV_FOUND})
    include(${ICONV_INCLUDE_DIR})
endif (${ICONV_FOUND})

if(NOT MSVC)
  target_compile_options(${Wololokingdoms_LIBRARY} PRIVATE -Wall -Wsign-compare -Wextra -Wpedantic)
else()
  target_compile_options(${Wololokingdoms_LIBRARY} PRIVATE /wd4244 /wd4018 /wd4267 /wd4996 /wd4800)
endif()

if (WIN32)
  target_compile_definitions(${Wololokingdoms_LIBRARY}
    PRIVATE UNICODE _UNICODE
  )
endif()

#------------------------------------------------------------------------------#
# Source files:
#------------------------------------------------------------------------------#
target_sources(${Wololokingdoms_LIBRARY} PRIVATE
  libwololokingdoms.cpp
  base64.cpp
  md5.cpp
  wk_xml.cpp
  string_helpers.cpp
  zr_map_creator.cpp
  drs_creator.cpp
  missing_strings.cpp
  fixes/addunits.cpp
  fixes/ai900unitidfix.cpp
  fixes/aztecbonusfix.cpp
  fixes/balance.cpp
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
  fixes/queuetechs.cpp
  fixes/separateshipgraphics.cpp
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
target_link_libraries(${Wololokingdoms_LIBRARY} genieutils)

install(TARGETS ${Wololokingdoms_LIBRARY}
  LIBRARY DESTINATION lib
)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h*"
)

if(ENABLE_LTO)
  message(STATUS "Building libwololokingdoms with link time optimization")
  set_target_properties(${Wololokingdoms_LIBRARY}
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()
