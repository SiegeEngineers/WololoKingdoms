cmake_minimum_required(VERSION 3.9.4)

set(Wololokingdoms_INSTALLER WololoKingdoms)
set(CMAKE_CXX_STANDARD 17)

project(WololoKingdoms)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/libwololokingdoms/cmake/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

include(CheckIPOSupported)
check_ipo_supported(RESULT ENABLE_LTO OUTPUT error)

set(WK_STATIC_BUILD 1)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/)

if (MSVC)
  if (CMAKE_BUILD_TYPE EQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MT")
  endif()
else()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif()

add_custom_target(build-time-make-directory ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/steam_appid.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_appid.txt COPYONLY)
if (WIN32)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/steam_api.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/steam_api.dll COPYONLY)
else()
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/third_party/steam_sdk/redistributable_bin/linux64/libsteam_api.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libsteam_api.so COPYONLY)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/genieutils/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/pcrio/include)
if (WIN32)
  add_definitions(-DICONV_SECOND_ARGUMENT_IS_CONST)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libwololokingdoms/third_party/genieutils/extern/win-iconv)
endif()

# check c++17 filesystem support
# (mingw's windows header has a bug and msvc doesn't have it)
include(HasWorkingFilesystem)
if (NOT WORKING_STD_FS)
  message(WARNING "builtin filesystem is bad, falling back to gulrak/filesystem")
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

link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
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
if(ENABLE_LTO)
  message(STATUS "Building with link time optimization")
  set_property(TARGET ${Wololokingdoms_INSTALLER} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_link_libraries(${Wololokingdoms_INSTALLER} wololokingdoms)
target_link_libraries(${Wololokingdoms_INSTALLER} genieutils)
if (WORKING_STD_FS)
  target_link_libraries(${Wololokingdoms_INSTALLER} stdc++fs)
endif()
target_link_libraries(${Wololokingdoms_INSTALLER} steam_api)

# Use the Widgets module from Qt 5
target_link_libraries(${Wololokingdoms_INSTALLER} Qt5::Widgets)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
