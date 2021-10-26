cmake_minimum_required(VERSION 3.0)

project(basic VERSION 0.7.0 LANGUAGES CXX)

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

add_library(basic STATIC Cryptor.h Cryptor.cpp Exception.cpp Exception.h chars.h Delegate.h dll_definitions.h FileSystem.h ImmutableString.h locale_init.h IProperties.h PropertiesFile.cpp PropertiesFile.h Process.h TextConv.h tstring.h tstream.h XConv.h XString.h)
target_sources(basic PRIVATE ${CMAKE_SOURCE_DIR}/unix/DirectoryImpl.cpp ${CMAKE_SOURCE_DIR}/unix/FileSystemImpl.cpp)

set_target_properties(basic PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
