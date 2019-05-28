cmake_minimum_required(VERSION 3.7)

set(Wololokingdoms_INSTALLER WololoKingdoms)
set(CMAKE_CXX_STANDARD 17)

project(WololoKingdoms)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

set(WK_STATIC_BUILD 1)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
if (WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/win64/steam_api.dll ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
else()
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/linux64/libsteam_api.so ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/genieutils/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/pcrio/include)
if (WIN32)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/genieutils/extern/win-iconv)
endif()

if(NOT MSVC)
    add_definitions(-Wall -Wsign-compare -Wextra -Wpedantic)
else()
    add_definitions(/wd4244 /wd4018 /wd4267 /wd4996 /wd4800)
endif()

set(ALL_SRC
  main.cpp
  dialog.ui
  dialog.cpp
  mainwindow.ui
  mainwindow.cpp
  paths.cpp
)

if (UNIX)
  set(ALL_SRC ${ALL_SRC} paths_linux.cpp)
endif()

link_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${Wololokingdoms_INSTALLER} ${ALL_SRC})
if (MSVC) # Check if we are using the Visual Studio compiler
  set_target_properties(${Wololokingdoms_INSTALLER} PROPERTIES
    WIN32_EXECUTABLE YES
    LINK_FLAGS "/ENTRY:mainCRTStartup"
  )
endif()

target_link_libraries(${Wololokingdoms_INSTALLER} wololokingdoms)
target_link_libraries(${Wololokingdoms_INSTALLER} genieutils)
target_link_libraries(${Wololokingdoms_INSTALLER} stdc++fs)
target_link_libraries(${Wololokingdoms_INSTALLER} steam_api)
# Use the Widgets module from Qt 5
target_link_libraries(${Wololokingdoms_INSTALLER} Qt5::Widgets)
