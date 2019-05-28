cmake_minimum_required(VERSION 3.7)

set(Wololokingdoms_INSTALLER WololoKingdoms)
set(CMAKE_CXX_STANDARD 17)

project(WololoKingdoms)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/libwololokingdoms/cmake/)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

set(WK_STATIC_BUILD 1)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
if (WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/steam_api.dll ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
else()
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/linux64/libsteam_api.so ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/genieutils/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/pcrio/include)
if (WIN32)
  add_definitions(-DICONV_SECOND_ARGUMENT_IS_CONST)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/genieutils/extern/win-iconv)
endif()

# DUPLICATED FROM libwololokingdoms/CMakeLists!
# check c++17 filesystem support
# (mingw's windows header has a bug)
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS "-std=c++17")
set(CMAKE_REQUIRED_LIBRARIES stdc++fs)
check_cxx_source_compiles("
#include <filesystem>
int main() {
    std::filesystem::path aPath {\"hello\"};
    aPath /= \"world\";
    return 0;
}
" WORKING_STD_FS)
set(CMAKE_REQUIRED_FLAGS)
set(CMAKE_REQUIRED_LIBRARIES)
if (NOT WORKING_STD_FS)
  message("builtin filesystem is bad, falling back to gulrak/filesystem")
  include_directories(libwololokingdoms/third_party/filesystem/include/)
  add_definitions(-DUSE_USERLAND_FS)
endif()

if (WIN32)
  add_definitions(-DNOMINMAX)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
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
if (WIN32)
  # let it know where to find the .lib
  link_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/)
endif()

add_executable(${Wololokingdoms_INSTALLER} ${ALL_SRC})
if (MSVC) # Check if we are using the Visual Studio compiler
  set_target_properties(${Wololokingdoms_INSTALLER} PROPERTIES
    WIN32_EXECUTABLE YES
    LINK_FLAGS "/ENTRY:mainCRTStartup"
  )
endif()

target_link_libraries(${Wololokingdoms_INSTALLER} wololokingdoms)
target_link_libraries(${Wololokingdoms_INSTALLER} genieutils)
if (WORKING_STD_FS)
  target_link_libraries(${Wololokingdoms_INSTALLER} stdc++fs)
endif()
target_link_libraries(${Wololokingdoms_INSTALLER} steam_api)

# Use the Widgets module from Qt 5
target_link_libraries(${Wololokingdoms_INSTALLER} Qt5::Widgets)
