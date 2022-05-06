cmake_minimum_required(VERSION 3.20)
project(xwindows)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_SUPPRESS_REGENERATION true)

if(MSVC_TOOLSET_VERSION GREATER 80)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../0libs)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../0libs)
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../0libs/${CMAKE_BUILD_TYPE})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../0libs/${CMAKE_BUILD_TYPE})
endif()

#######################################

find_package(PkgConfig REQUIRED)
if(NOT PKG_CONFIG_FOUND)
    message(FATAL_ERROR "no Pkg-Config on the system")
    return()
endif()

pkg_check_modules(GTK3 REQUIRED gtk+-wayland-3.0)
if(NOT ${GTK3_FOUND})
    message(FATAL_ERROR "the GTK3 library is not found on the system")
    return()
endif()
message(STATUS "GTK3 include path: ${GTK3_INCLUDE_DIR}")

#######################################

add_library(xwindows STATIC XApp.h XApp.cpp XButton.h XButton.cpp XComboBox.h XComboBox.cpp XEvent.h XEvent.cpp XEventHandler.h XEventHandler.cpp XFrame.h XFrame.cpp XWidget.h XWidget.cpp XWindow.h XWindow.cpp)
target_sources(xwindows PRIVATE ${CMAKE_SOURCE_DIR}/unix/platform_specific.cpp ${CMAKE_SOURCE_DIR}/unix/platform_specific.cpp)

target_include_directories(xwindows PRIVATE ${CMAKE_SOURCE_DIR}/..)
include_directories(${GTK3_INCLUDE_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})